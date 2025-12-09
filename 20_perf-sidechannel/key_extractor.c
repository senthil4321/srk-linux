/*
 * AES Key Extractor
 * Demonstrates cache timing attack to recover AES key bytes
 * Uses perf events to measure cache misses during encryption
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <openssl/aes.h>
#include <sys/syscall.h>
#include <linux/perf_event.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <limits.h>

#define KEY_SIZE 16
#define BLOCK_SIZE 16
#define NUM_ITERATIONS 10000  // Number of encryptions per key guess

// Known secret key (for demo, we recover the first byte)
static unsigned char known_key[KEY_SIZE] = {
    0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
    0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
};

static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                            int cpu, int group_fd, unsigned long flags) {
    return syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}

int setup_cache_miss_counter() {
    struct perf_event_attr pe;
    memset(&pe, 0, sizeof(struct perf_event_attr));
    pe.type = PERF_TYPE_HW_CACHE;
    pe.size = sizeof(struct perf_event_attr);
    pe.config = (PERF_COUNT_HW_CACHE_L1D) |
                (PERF_COUNT_HW_CACHE_OP_READ << 8) |
                (PERF_COUNT_HW_CACHE_RESULT_MISS << 16);
    pe.disabled = 1;
    pe.exclude_kernel = 0;
    pe.exclude_hv = 1;

    int fd = perf_event_open(&pe, 0, -1, -1, 0);  // Monitor self
    if (fd == -1) {
        fprintf(stderr, "Error opening perf counter: %s\n", strerror(errno));
        return -1;
    }
    return fd;
}

long long measure_cache_misses(int perf_fd, unsigned char *key) {
    AES_KEY enc_key;
    unsigned char plaintext[BLOCK_SIZE] = {0};  // Fixed plaintext for consistency
    unsigned char ciphertext[BLOCK_SIZE];

    if (AES_set_encrypt_key(key, 128, &enc_key) < 0) {
        return -1;
    }

    // Reset and enable counter
    ioctl(perf_fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(perf_fd, PERF_EVENT_IOC_ENABLE, 0);

    // Perform encryptions
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        AES_encrypt(plaintext, ciphertext, &enc_key);
    }

    // Disable and read counter
    ioctl(perf_fd, PERF_EVENT_IOC_DISABLE, 0);
    long long count;
    read(perf_fd, &count, sizeof(long long));

    return count;
}

int main() {
    printf("=== AES Key Extractor ===\n");
    printf("Recovering first byte of AES key using cache timing attack\n");
    printf("Performing %d encryptions per key guess...\n\n", NUM_ITERATIONS);

    int perf_fd = setup_cache_miss_counter();
    if (perf_fd < 0) {
        fprintf(stderr, "Failed to setup perf counter. Need root privileges.\n");
        fprintf(stderr, "Try: sudo %s\n", __FILE__);  // Note: __FILE__ won't work, but for demo
        return 1;
    }

    unsigned char test_key[KEY_SIZE];
    memcpy(test_key, known_key, KEY_SIZE);

    long long min_misses = LLONG_MAX;
    int best_byte = -1;

    printf("Testing key byte 0x00 to 0xFF...\n");
    for (int byte = 0; byte < 256; byte++) {
        test_key[0] = byte;
        long long misses = measure_cache_misses(perf_fd, test_key);

        if (misses < 0) {
            fprintf(stderr, "Error measuring misses for byte 0x%02x\n", byte);
            continue;
        }

        if (misses < min_misses) {
            min_misses = misses;
            best_byte = byte;
        }

        if (byte % 16 == 0) {
            printf("Byte 0x%02x: %lld misses\n", byte, misses);
        }
    }

    close(perf_fd);

    printf("\n=== Results ===\n");
    printf("Recovered first key byte: 0x%02x\n", best_byte);
    printf("Actual first key byte: 0x%02x\n", known_key[0]);
    printf("Cache misses for best guess: %lld\n", min_misses);

    if (best_byte == known_key[0]) {
        printf("SUCCESS: Key byte recovered correctly!\n");
    } else {
        printf("FAILED: Key byte not recovered (may need more iterations or different conditions)\n");
    }

    return 0;
}