/**
 * perf_sampling_demo.c
 * 
 * Demonstrates perf event sampling capabilities:
 * - Sample-based profiling (interrupt every N events)
 * - Call stack sampling
 * - Instruction pointer recording
 * - Period-based sampling
 * 
 * This is different from counting mode - it captures data
 * at specific intervals to identify hotspots.
 * 
 * Compile: gcc -O2 -o perf_sampling_demo perf_sampling_demo.c
 * Run: sudo ./perf_sampling_demo
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>
#include <errno.h>
#include <stdint.h>
#include <signal.h>

#define SAMPLE_PERIOD 100000  // Sample every 100k events
#define MMAP_PAGES 8          // Number of pages for ring buffer

static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                           int cpu, int group_fd, unsigned long flags) {
    return syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}

// Sample record header
struct sample_header {
    uint32_t type;
    uint16_t misc;
    uint16_t size;
};

// Workload functions to sample
__attribute__((noinline))
void hotspot_function_1() {
    volatile double result = 0.0;
    for (int i = 0; i < 500000; i++) {
        result += i * 3.14159;
    }
}

__attribute__((noinline))
void hotspot_function_2() {
    volatile int sum = 0;
    for (int i = 0; i < 1000000; i++) {
        sum += i % 17;
    }
}

__attribute__((noinline))
void cache_intensive_function() {
    size_t size = 10 * 1024 * 1024;  // 10 MB
    char *buffer = malloc(size);
    if (!buffer) return;
    
    for (size_t i = 0; i < size; i += 64) {
        buffer[i] = (char)(i & 0xFF);
    }
    
    free(buffer);
}

void mixed_workload() {
    for (int i = 0; i < 5; i++) {
        hotspot_function_1();
        hotspot_function_2();
        cache_intensive_function();
    }
}

void demo_instruction_sampling() {
    printf("\n");
    printf("========================================\n");
    printf("Demo: Instruction Pointer Sampling\n");
    printf("========================================\n");
    
    struct perf_event_attr pe;
    memset(&pe, 0, sizeof(struct perf_event_attr));
    
    pe.type = PERF_TYPE_HARDWARE;
    pe.size = sizeof(struct perf_event_attr);
    pe.config = PERF_COUNT_HW_CPU_CYCLES;
    pe.sample_period = SAMPLE_PERIOD;
    pe.sample_type = PERF_SAMPLE_IP | PERF_SAMPLE_TID | PERF_SAMPLE_TIME;
    pe.disabled = 1;
    pe.exclude_kernel = 0;
    pe.exclude_hv = 1;
    pe.wakeup_events = 1;
    
    int fd = perf_event_open(&pe, 0, -1, -1, 0);
    if (fd == -1) {
        fprintf(stderr, "Error: perf_event_open failed: %s\n", strerror(errno));
        fprintf(stderr, "Try running with: sudo\n");
        return;
    }
    
    // Setup memory mapping for sample buffer
    size_t mmap_size = (MMAP_PAGES + 1) * sysconf(_SC_PAGESIZE);
    void *buffer = mmap(NULL, mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (buffer == MAP_FAILED) {
        fprintf(stderr, "Error: mmap failed: %s\n", strerror(errno));
        close(fd);
        return;
    }
    
    printf("\nSampling configuration:\n");
    printf("  Event: CPU Cycles\n");
    printf("  Period: %d cycles\n", SAMPLE_PERIOD);
    printf("  Sample: Instruction Pointer, Thread ID, Time\n");
    
    // Enable sampling
    ioctl(fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
    
    printf("\nRunning mixed workload...\n");
    mixed_workload();
    
    // Disable sampling
    ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
    
    // Read samples from ring buffer
    struct perf_event_mmap_page *header = (struct perf_event_mmap_page *)buffer;
    printf("\nSampling results:\n");
    printf("  Samples captured: ~%llu\n", header->data_head / 32);  // Rough estimate
    printf("  Ring buffer size: %zu bytes\n", mmap_size);
    
    // Note: Full sample parsing requires complex ring buffer handling
    printf("\nNote: Full sample analysis requires parsing the ring buffer.\n");
    printf("This demo shows the setup. Use 'perf record' for production use.\n");
    
    munmap(buffer, mmap_size);
    close(fd);
}

void demo_cache_miss_sampling() {
    printf("\n");
    printf("========================================\n");
    printf("Demo: Cache Miss Sampling\n");
    printf("========================================\n");
    
    struct perf_event_attr pe;
    memset(&pe, 0, sizeof(struct perf_event_attr));
    
    pe.type = PERF_TYPE_HARDWARE;
    pe.size = sizeof(struct perf_event_attr);
    pe.config = PERF_COUNT_HW_CACHE_MISSES;
    pe.sample_period = 1000;  // Sample every 1000 cache misses
    pe.sample_type = PERF_SAMPLE_IP | PERF_SAMPLE_ADDR;
    pe.disabled = 1;
    pe.exclude_kernel = 0;
    pe.exclude_hv = 1;
    
    int fd = perf_event_open(&pe, 0, -1, -1, 0);
    if (fd == -1) {
        fprintf(stderr, "Error: perf_event_open failed: %s\n", strerror(errno));
        return;
    }
    
    printf("\nSampling configuration:\n");
    printf("  Event: Cache Misses\n");
    printf("  Period: 1000 misses\n");
    printf("  Sample: IP + Memory Address\n");
    
    // Enable sampling
    ioctl(fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
    
    printf("\nRunning cache-intensive workload...\n");
    cache_intensive_function();
    
    // Disable and read total count
    ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
    
    uint64_t total_misses = 0;
    read(fd, &total_misses, sizeof(uint64_t));
    
    printf("\nResults:\n");
    printf("  Total cache misses: %lu\n", total_misses);
    printf("  Estimated samples: ~%lu\n", total_misses / 1000);
    printf("  (Samples capture addresses causing cache misses)\n");
    
    close(fd);
}

void demo_frequency_sampling() {
    printf("\n");
    printf("========================================\n");
    printf("Demo: Frequency-based Sampling\n");
    printf("========================================\n");
    
    struct perf_event_attr pe;
    memset(&pe, 0, sizeof(struct perf_event_attr));
    
    pe.type = PERF_TYPE_HARDWARE;
    pe.size = sizeof(struct perf_event_attr);
    pe.config = PERF_COUNT_HW_CPU_CYCLES;
    pe.sample_freq = 1000;  // 1000 Hz (samples per second)
    pe.freq = 1;            // Enable frequency mode
    pe.sample_type = PERF_SAMPLE_IP;
    pe.disabled = 1;
    pe.exclude_kernel = 1;
    pe.exclude_hv = 1;
    
    int fd = perf_event_open(&pe, 0, -1, -1, 0);
    if (fd == -1) {
        fprintf(stderr, "Error: perf_event_open failed: %s\n", strerror(errno));
        return;
    }
    
    printf("\nSampling configuration:\n");
    printf("  Mode: Frequency-based\n");
    printf("  Frequency: 1000 Hz\n");
    printf("  (Automatically adjusts period for consistent sample rate)\n");
    
    // Enable sampling
    ioctl(fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
    
    printf("\nRunning workload for 2 seconds...\n");
    sleep(1);
    mixed_workload();
    sleep(1);
    
    ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
    
    printf("\nExpected samples: ~2000 (1000 Hz × 2 seconds)\n");
    printf("This mode is ideal for time-based profiling.\n");
    
    close(fd);
}

int main() {
    printf("========================================\n");
    printf("perf_event Sampling Demonstrations\n");
    printf("========================================\n");
    printf("\nSampling vs Counting:\n");
    printf("- Counting: Total event counts (what happened)\n");
    printf("- Sampling: Periodic snapshots (where it happened)\n");
    printf("\nSampling helps identify:\n");
    printf("- Which functions are hotspots\n");
    printf("- Which memory addresses cause cache misses\n");
    printf("- Which code paths execute most frequently\n");
    
    demo_instruction_sampling();
    demo_cache_miss_sampling();
    demo_frequency_sampling();
    
    printf("\n");
    printf("========================================\n");
    printf("Demo Complete!\n");
    printf("========================================\n");
    printf("\nFor production profiling, use:\n");
    printf("  perf record -e cycles -g ./your_program\n");
    printf("  perf report\n");
    printf("\nThis provides full call-graph analysis and\n");
    printf("interactive exploration of samples.\n");
    
    return 0;
}
