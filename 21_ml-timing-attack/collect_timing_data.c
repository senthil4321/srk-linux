/*
 * Timing Data Collector for ML-based AES Key Recovery
 * Collects encryption timing measurements for different key guesses
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include "vulnerable_aes.h"

#define NUM_SAMPLES 5000      // Samples per key byte guess
#define NUM_PLAINTEXTS 10     // Different plaintexts to test

// Target secret key
static uint8_t secret_key[AES_KEY_SIZE] = {
    0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
    0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
};

// Measure encryption time in microseconds
double measure_timing(VulnerableAES_CTX *ctx, const uint8_t *plaintext) {
    uint8_t ciphertext[AES_BLOCK_SIZE];
    struct timeval start, end;
    
    gettimeofday(&start, NULL);
    vulnerable_aes_encrypt(ctx, plaintext, ciphertext);
    gettimeofday(&end, NULL);
    
    return (end.tv_sec - start.tv_sec) * 1000000.0 + (end.tv_usec - start.tv_usec);
}

// Generate training data for first key byte
void generate_training_data(const char *output_file) {
    FILE *fp = fopen(output_file, "w");
    if (!fp) {
        perror("Failed to open output file");
        return;
    }
    
    // CSV header
    fprintf(fp, "key_byte_guess,plaintext,timing_us,is_correct\n");
    
    printf("Generating training data for ML model...\n");
    printf("Collecting %d samples for each of 256 key byte values...\n\n", NUM_SAMPLES);
    
    uint8_t test_key[AES_KEY_SIZE];
    uint8_t plaintext[AES_BLOCK_SIZE];
    VulnerableAES_CTX ctx;
    
    // Test all 256 possible values for first key byte
    for (int key_guess = 0; key_guess < 256; key_guess++) {
        // Setup test key with guessed first byte
        memcpy(test_key, secret_key, AES_KEY_SIZE);
        test_key[0] = key_guess;
        
        vulnerable_aes_key_expansion(&ctx, test_key);
        
        // Collect samples with different plaintexts
        for (int sample = 0; sample < NUM_SAMPLES; sample++) {
            // Generate varied plaintext
            for (int i = 0; i < AES_BLOCK_SIZE; i++) {
                plaintext[i] = (sample * 17 + i * 23 + key_guess) & 0xFF;
            }
            
            double timing = measure_timing(&ctx, plaintext);
            int is_correct = (key_guess == secret_key[0]) ? 1 : 0;
            
            // Output: key_guess, plaintext_first_byte, timing, is_correct
            fprintf(fp, "%d,%d,%.6f,%d\n", key_guess, plaintext[0], timing, is_correct);
        }
        
        if (key_guess % 32 == 0) {
            printf("Progress: %d/256 key values tested\n", key_guess);
        }
    }
    
    fclose(fp);
    printf("\nTraining data saved to %s\n", output_file);
    printf("Total samples: %d\n", 256 * NUM_SAMPLES);
}

int main(int argc, char *argv[]) {
    const char *output_file = "timing_data.csv";
    
    if (argc > 1) {
        output_file = argv[1];
    }
    
    printf("=== ML-Based AES Timing Attack - Data Collector ===\n");
    printf("Target key first byte: 0x%02x\n", secret_key[0]);
    printf("Output file: %s\n\n", output_file);
    
    generate_training_data(output_file);
    
    printf("\nNext steps:\n");
    printf("1. Run: python3 ml_key_recovery.py %s\n", output_file);
    printf("2. The ML model will analyze timing patterns to recover the key byte\n");
    
    return 0;
}
