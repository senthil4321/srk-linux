/*
 * AES Victim Process
 * Performs AES encryption operations that can be observed via cache timing
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <openssl/aes.h>
#include <time.h>

#define KEY_SIZE 16  // 128-bit key
#define BLOCK_SIZE 16

// Secret key we're trying to extract via side-channel
static unsigned char secret_key[KEY_SIZE] = {
    0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
    0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
};

void print_hex(const char *label, unsigned char *data, int len) {
    printf("%s: ", label);
    for (int i = 0; i < len; i++) {
        printf("%02x", data[i]);
    }
    printf("\n");
}

int main() {
    AES_KEY enc_key;
    unsigned char plaintext[BLOCK_SIZE];
    unsigned char ciphertext[BLOCK_SIZE];
    
    printf("=== AES Victim Process ===\n");
    printf("PID: %d\n", getpid());
    print_hex("Secret Key", secret_key, KEY_SIZE);
    
    // Initialize AES key
    if (AES_set_encrypt_key(secret_key, 128, &enc_key) < 0) {
        fprintf(stderr, "AES key setup failed\n");
        return 1;
    }
    
    printf("\nPerforming continuous AES encryptions...\n");
    printf("Monitor this process with perf_spy to observe cache patterns\n\n");
    
    // Continuous encryption loop
    unsigned long iterations = 0;
    while (1) {
        // Generate random plaintext
        for (int i = 0; i < BLOCK_SIZE; i++) {
            plaintext[i] = rand() & 0xFF;
        }
        
        // Perform AES encryption (table-based, vulnerable to cache timing)
        AES_encrypt(plaintext, ciphertext, &enc_key);
        
        iterations++;
        if (iterations % 100000 == 0) {
            printf("Iterations: %lu\r", iterations);
            fflush(stdout);
        }
        
        // Small delay to make cache observations easier
        usleep(10);
    }
    
    return 0;
}
