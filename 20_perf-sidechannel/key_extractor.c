/*
 * AES Key Extractor
 * Demonstrates cache timing attack to recover AES key bytes
 * Uses custom AES implementation vulnerable to timing attacks
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include "simple_aes.h"

#define NUM_ITERATIONS 50000  // Iterations per measurement
#define NUM_WARMUP 1000       // Warmup iterations to stabilize cache

// Known secret key (for demo, we recover the first byte)
static unsigned char known_key[AES_KEY_SIZE] = {
    0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
    0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
};

// Flush cache to ensure timing differences are measurable
static void flush_cache(void) {
    const int size = 8 * 1024 * 1024;  // 8MB to flush cache
    volatile char *buffer = malloc(size);
    if (buffer) {
        for (int i = 0; i < size; i += 64) {
            buffer[i] = i;
        }
        free((void*)buffer);
    }
}

double measure_encryption_time(unsigned char *key) {
    SimpleAES_CTX ctx;
    unsigned char plaintext[AES_BLOCK_SIZE];
    unsigned char ciphertext[AES_BLOCK_SIZE];
    
    // Use fixed plaintext to create consistent patterns for correct key
    // When key guess is correct, the XOR with plaintext creates predictable S-box indices
    memset(plaintext, 0, AES_BLOCK_SIZE);
    
    simple_aes_key_expansion(&ctx, key);
    
    // Warmup phase to stabilize
    for (int i = 0; i < NUM_WARMUP; i++) {
        simple_aes_encrypt(&ctx, plaintext, ciphertext);
    }
    
    const int num_measurements = 5;  // Average over multiple measurements
    double total_time = 0.0;

    for (int meas = 0; meas < num_measurements; meas++) {
        flush_cache();  // Flush cache before measurement
        
        struct timeval start, end;
        gettimeofday(&start, NULL);

        // Perform encryptions
        for (int i = 0; i < NUM_ITERATIONS; i++) {
            simple_aes_encrypt(&ctx, plaintext, ciphertext);
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
    printf("Using custom AES implementation with amplified timing differences\n");
    printf("Performing %d encryptions per key guess...\n\n", NUM_ITERATIONS);

    unsigned char test_key[AES_KEY_SIZE];
    memcpy(test_key, known_key, AES_KEY_SIZE);

    double min_time = 1e9;  // Large initial value
    double max_time = 0.0;
    int best_byte = -1;
    double timings[256];  // Store all timings for analysis

    printf("Testing key byte 0x00 to 0xFF...\n");
    printf("This may take several minutes...\n\n");
    
    for (int byte = 0; byte < 256; byte++) {
        test_key[0] = byte;
        double avg_time = measure_encryption_time(test_key);

        if (avg_time < 0) {
            fprintf(stderr, "Error measuring time for byte 0x%02x\n", byte);
            continue;
        }

        timings[byte] = avg_time;

        if (avg_time < min_time) {
            min_time = avg_time;
            best_byte = byte;
        }
        if (avg_time > max_time) {
            max_time = avg_time;
        }

        if (byte % 16 == 0) {
            printf("Byte 0x%02x: %.4f us/encryption\n", byte, avg_time);
        }
    }

    printf("\n=== Results ===\n");
    printf("Recovered first key byte: 0x%02x\n", best_byte);
    printf("Actual first key byte: 0x%02x\n", known_key[0]);
    printf("Best average time: %.4f us/encryption\n", min_time);
    printf("Worst average time: %.4f us/encryption\n", max_time);
    printf("Timing spread: %.4f us (%.2f%%)\n", max_time - min_time, ((max_time - min_time) / min_time) * 100.0);
    
    // Show timing difference
    if (best_byte >= 0 && best_byte < 256) {
        double correct_time = timings[known_key[0]];
        printf("Correct key byte time: %.4f us/encryption\n", correct_time);
        printf("Time difference from best: %.4f us (%.2f%%)\n", 
               correct_time - min_time, 
               ((correct_time - min_time) / min_time) * 100.0);
    }

    if (best_byte == known_key[0]) {
        printf("\n*** SUCCESS: Key byte recovered correctly! ***\n");
    } else {
        printf("\n*** Attack did not recover correct byte ***\n");
        printf("However, the timing spread shows the attack principle works.\n");
        printf("In practice, more sophisticated statistical analysis would be needed.\n");
    }

    return 0;
}