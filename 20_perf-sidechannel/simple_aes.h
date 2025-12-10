/*
 * Simple AES Implementation
 * Intentionally vulnerable to cache timing attacks for demonstration
 * Uses lookup tables without constant-time protections
 */

#ifndef SIMPLE_AES_H
#define SIMPLE_AES_H

#include <stdint.h>

#define AES_BLOCK_SIZE 16
#define AES_KEY_SIZE 16

// AES S-box (forward)
static const uint8_t sbox[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

// Rcon used in key expansion
static const uint8_t rcon[10] = {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
};

typedef struct {
    uint8_t round_keys[11][AES_BLOCK_SIZE];
    int rounds;
} SimpleAES_CTX;

// Key expansion for 128-bit key (10 rounds)
void simple_aes_key_expansion(SimpleAES_CTX *ctx, const uint8_t *key) {
    int i, j;
    uint8_t temp[4];
    
    ctx->rounds = 10;
    
    // First round key is the key itself
    for (i = 0; i < AES_KEY_SIZE; i++) {
        ctx->round_keys[0][i] = key[i];
    }
    
    // Generate remaining round keys
    for (i = 1; i <= ctx->rounds; i++) {
        // Copy last 4 bytes of previous round key
        for (j = 0; j < 4; j++) {
            temp[j] = ctx->round_keys[i-1][12 + j];
        }
        
        // Rotate and substitute
        uint8_t t = temp[0];
        temp[0] = sbox[temp[1]] ^ rcon[i-1];
        temp[1] = sbox[temp[2]];
        temp[2] = sbox[temp[3]];
        temp[3] = sbox[t];
        
        // XOR with first 4 bytes of previous round key
        for (j = 0; j < 4; j++) {
            ctx->round_keys[i][j] = ctx->round_keys[i-1][j] ^ temp[j];
        }
        
        // Generate remaining 12 bytes
        for (j = 4; j < AES_KEY_SIZE; j++) {
            ctx->round_keys[i][j] = ctx->round_keys[i-1][j] ^ ctx->round_keys[i][j-4];
        }
    }
}

// SubBytes transformation - VULNERABLE to cache timing!
static void sub_bytes(uint8_t *state) {
    int i;
    for (i = 0; i < AES_BLOCK_SIZE; i++) {
        // Introduce artificial delay based on S-box value to amplify timing
        // This simulates cache miss penalty in a more exaggerated way
        uint8_t val = sbox[state[i]];  // Cache timing leak here!
        
        // Add CPU-bound delay proportional to value to amplify differences
        volatile int dummy = 0;
        for (int j = 0; j < (val & 0x0F); j++) {
            dummy += j;
        }
        
        state[i] = val;
    }
}

// ShiftRows transformation
static void shift_rows(uint8_t *state) {
    uint8_t temp;
    
    // Row 1: shift left by 1
    temp = state[1];
    state[1] = state[5];
    state[5] = state[9];
    state[9] = state[13];
    state[13] = temp;
    
    // Row 2: shift left by 2
    temp = state[2];
    state[2] = state[10];
    state[10] = temp;
    temp = state[6];
    state[6] = state[14];
    state[14] = temp;
    
    // Row 3: shift left by 3
    temp = state[15];
    state[15] = state[11];
    state[11] = state[7];
    state[7] = state[3];
    state[3] = temp;
}

// GF(2^8) multiplication
static uint8_t gf_mul(uint8_t a, uint8_t b) {
    uint8_t p = 0;
    uint8_t high_bit;
    int i;
    
    for (i = 0; i < 8; i++) {
        if (b & 1) {
            p ^= a;
        }
        high_bit = a & 0x80;
        a <<= 1;
        if (high_bit) {
            a ^= 0x1b;  // AES irreducible polynomial
        }
        b >>= 1;
    }
    return p;
}

// MixColumns transformation
static void mix_columns(uint8_t *state) {
    uint8_t temp[4];
    int i;
    
    for (i = 0; i < 4; i++) {
        temp[0] = state[i*4 + 0];
        temp[1] = state[i*4 + 1];
        temp[2] = state[i*4 + 2];
        temp[3] = state[i*4 + 3];
        
        state[i*4 + 0] = gf_mul(temp[0], 2) ^ gf_mul(temp[1], 3) ^ temp[2] ^ temp[3];
        state[i*4 + 1] = temp[0] ^ gf_mul(temp[1], 2) ^ gf_mul(temp[2], 3) ^ temp[3];
        state[i*4 + 2] = temp[0] ^ temp[1] ^ gf_mul(temp[2], 2) ^ gf_mul(temp[3], 3);
        state[i*4 + 3] = gf_mul(temp[0], 3) ^ temp[1] ^ temp[2] ^ gf_mul(temp[3], 2);
    }
}

// AddRoundKey transformation
static void add_round_key(uint8_t *state, const uint8_t *round_key) {
    int i;
    for (i = 0; i < AES_BLOCK_SIZE; i++) {
        state[i] ^= round_key[i];
    }
}

// AES encryption
void simple_aes_encrypt(SimpleAES_CTX *ctx, const uint8_t *plaintext, uint8_t *ciphertext) {
    uint8_t state[AES_BLOCK_SIZE];
    int i, round;
    
    // Copy plaintext to state
    for (i = 0; i < AES_BLOCK_SIZE; i++) {
        state[i] = plaintext[i];
    }
    
    // Initial round
    add_round_key(state, ctx->round_keys[0]);
    
    // Main rounds (9 rounds for AES-128)
    for (round = 1; round < ctx->rounds; round++) {
        sub_bytes(state);      // VULNERABLE: Cache timing leak!
        shift_rows(state);
        mix_columns(state);
        add_round_key(state, ctx->round_keys[round]);
    }
    
    // Final round (no MixColumns)
    sub_bytes(state);          // VULNERABLE: Cache timing leak!
    shift_rows(state);
    add_round_key(state, ctx->round_keys[ctx->rounds]);
    
    // Copy state to ciphertext
    for (i = 0; i < AES_BLOCK_SIZE; i++) {
        ciphertext[i] = state[i];
    }
}

#endif // SIMPLE_AES_H
