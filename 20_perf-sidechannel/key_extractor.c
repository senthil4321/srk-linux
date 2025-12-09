/*
 * AES Key Extractor
 * Demonstrates cache timing attack to recover AES key bytes
 * Uses timing measurements during encryption to identify correct key
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <openssl/aes.h>
#include <time.h>
#include <sys/time.h>

#define KEY_SIZE 16
#define BLOCK_SIZE 16
#define NUM_ITERATIONS 1000000  // Increased for better resolution

// Known secret key (for demo, we recover the first byte)
static unsigned char known_key[KEY_SIZE] = {
    0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
    0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
};

double measure_encryption_time(unsigned char *key) {
    AES_KEY enc_key;
    unsigned char plaintext[BLOCK_SIZE] = {0};  // Fixed plaintext for consistency
    unsigned char ciphertext[BLOCK_SIZE];

    if (AES_set_encrypt_key(key, 128, &enc_key) < 0) {
        return -1.0;
    }

    const int num_measurements = 5;  // Average over multiple measurements
    double total_time = 0.0;

    for (int meas = 0; meas < num_measurements; meas++) {
        struct timeval start, end;
        gettimeofday(&start, NULL);

        // Perform encryptions
        for (int i = 0; i < NUM_ITERATIONS; i++) {
            AES_encrypt(plaintext, ciphertext, &enc_key);
        }

        gettimeofday(&end, NULL);

        double time_taken = (end.tv_sec - start.tv_sec) * 1000000.0 + (end.tv_usec - start.tv_usec);
        total_time += time_taken;
    }

    return total_time / (num_measurements * NUM_ITERATIONS);  // Average time per encryption in microseconds
}

int main() {
    printf("=== AES Key Extractor ===\n");
    printf("Recovering first byte of AES key using timing attack\n");
    printf("Performing %d encryptions per key guess...\n\n", NUM_ITERATIONS);

    unsigned char test_key[KEY_SIZE];
    memcpy(test_key, known_key, KEY_SIZE);

    double min_time = 1e9;  // Large initial value
    int best_byte = -1;

    printf("Testing key byte 0x00 to 0xFF...\n");
    for (int byte = 0; byte < 256; byte++) {
        test_key[0] = byte;
        double avg_time = measure_encryption_time(test_key);

        if (avg_time < 0) {
            fprintf(stderr, "Error measuring time for byte 0x%02x\n", byte);
            continue;
        }

        if (avg_time < min_time) {
            min_time = avg_time;
            best_byte = byte;
        }

        if (byte % 16 == 0) {
            printf("Byte 0x%02x: %.3f us/encryption\n", byte, avg_time);
        }
    }

    printf("\n=== Results ===\n");
    printf("Recovered first key byte: 0x%02x\n", best_byte);
    printf("Actual first key byte: 0x%02x\n", known_key[0]);
    printf("Best average time: %.3f us/encryption\n", min_time);

    if (best_byte == known_key[0]) {
        printf("SUCCESS: Key byte recovered correctly!\n");
    } else {
        printf("FAILED: Key byte not recovered (may need more iterations or different conditions)\n");
    }

    return 0;
}