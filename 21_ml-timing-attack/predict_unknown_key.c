/**
 * predict_unknown_key.c
 * 
 * Demonstrates using trained ML models to predict an UNKNOWN secret key.
 * This simulates a real attack scenario where the attacker has:
 * 1. Trained models from previous data collection
 * 2. A new target system with an unknown key
 * 3. Access to measure encryption timing
 * 
 * The program collects timing data for a NEW secret key and saves it
 * for prediction by the trained ML models.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include "vulnerable_aes.h"

// NEW UNKNOWN SECRET KEY (attacker doesn't know this)
static const uint8_t UNKNOWN_SECRET_KEY[16] = {
    0xa3, 0x7f, 0x9c, 0x45, 0x2e, 0x81, 0xb6, 0xd4,
    0x1f, 0x58, 0xc2, 0x93, 0x6a, 0xe7, 0x14, 0xbb
};

#define SAMPLES_PER_KEY 5000
#define NUM_KEY_GUESSES 256

/**
 * Measure encryption timing in microseconds
 */
static double measure_encryption_time(const uint8_t *key, const uint8_t *plaintext, uint8_t *ciphertext) {
    struct timespec start, end;
    VulnerableAES_CTX ctx;
    
    // Expand key
    vulnerable_aes_key_expansion(&ctx, key);
    
    // Measure encryption time
    clock_gettime(CLOCK_MONOTONIC, &start);
    vulnerable_aes_encrypt(&ctx, plaintext, ciphertext);
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    // Calculate elapsed time in microseconds
    double elapsed = (end.tv_sec - start.tv_sec) * 1000000.0 +
                    (end.tv_nsec - start.tv_nsec) / 1000.0;
    
    return elapsed;
}

/**
 * Generate varied plaintext to avoid cache effects
 */
static void generate_plaintext(uint8_t *plaintext, uint32_t seed) {
    srand(seed);
    for (int i = 0; i < 16; i++) {
        plaintext[i] = rand() & 0xFF;
    }
}

int main() {
    FILE *fp;
    uint8_t test_key[16];
    uint8_t plaintext[16];
    uint8_t ciphertext[16];
    uint32_t total_samples = 0;
    
    printf("=== ML-Based AES Key Recovery - PREDICTION MODE ===\n\n");
    printf("Unknown Secret Key (first byte): 0x%02x\n", UNKNOWN_SECRET_KEY[0]);
    printf("Target: Predict the first key byte using trained ML models\n\n");
    
    // Open output file
    fp = fopen("unknown_key_data.csv", "w");
    if (!fp) {
        perror("Failed to open output file");
        return 1;
    }
    
    // Write CSV header
    fprintf(fp, "key_guess,plaintext_byte,timing_us,is_correct\n");
    
    printf("Phase 1: Collecting timing data for unknown key...\n");
    printf("Progress: [");
    fflush(stdout);
    
    // Test each possible first key byte
    for (int key_guess = 0; key_guess < NUM_KEY_GUESSES; key_guess++) {
        // Progress bar
        if (key_guess % 16 == 0) {
            printf("=");
            fflush(stdout);
        }
        
        // Construct test key with guessed first byte
        test_key[0] = key_guess;
        for (int i = 1; i < 16; i++) {
            test_key[i] = UNKNOWN_SECRET_KEY[i];
        }
        
        // Collect samples for this key guess
        for (int sample = 0; sample < SAMPLES_PER_KEY; sample++) {
            // Generate varied plaintext
            generate_plaintext(plaintext, key_guess * SAMPLES_PER_KEY + sample);
            
            // Measure encryption timing
            double timing = measure_encryption_time(test_key, plaintext, ciphertext);
            
            // Record: is this the correct key byte?
            int is_correct = (key_guess == UNKNOWN_SECRET_KEY[0]) ? 1 : 0;
            
            // Write to CSV
            fprintf(fp, "%d,%d,%.6f,%d\n", 
                   key_guess, plaintext[0], timing, is_correct);
            
            total_samples++;
        }
    }
    
    printf("]\n\n");
    fclose(fp);
    
    printf("Phase 2: Data collection complete!\n");
    printf("  Total samples: %u\n", total_samples);
    printf("  Output file: unknown_key_data.csv\n");
    printf("  File size: %.2f MB\n", total_samples * 30.0 / 1024.0 / 1024.0);
    
    printf("\n=== Next Step ===\n");
    printf("Run the trained ML models to predict the unknown key:\n");
    printf("  python predict_with_model.py unknown_key_data.csv\n\n");
    printf("The model will analyze timing patterns and predict: 0x%02x\n", UNKNOWN_SECRET_KEY[0]);
    
    return 0;
}
