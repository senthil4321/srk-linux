/*
 * Perf Spy Process
 * Uses Linux perf events to monitor cache behavior of victim process
 * Demonstrates side-channel attack via cache timing
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/perf_event.h>
#include <sys/ioctl.h>
#include <errno.h>

static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                            int cpu, int group_fd, unsigned long flags) {
    return syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}

typedef struct {
    int fd;
    const char *name;
} perf_counter_t;

int setup_perf_counter(pid_t target_pid, __u32 type, __u64 config, const char *name) {
    struct perf_event_attr pe;
    
    memset(&pe, 0, sizeof(struct perf_event_attr));
    pe.type = type;
    pe.size = sizeof(struct perf_event_attr);
    pe.config = config;
    pe.disabled = 1;
    pe.exclude_kernel = 0;
    pe.exclude_hv = 1;
    
    int fd = perf_event_open(&pe, target_pid, -1, -1, 0);
    if (fd == -1) {
        fprintf(stderr, "Error opening %s: %s\n", name, strerror(errno));
        return -1;
    }
    
    return fd;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <target_pid>\n", argv[0]);
        fprintf(stderr, "Example: %s $(pgrep aes_victim)\n", argv[0]);
        return 1;
    }
    
    pid_t target_pid = atoi(argv[1]);
    
    printf("=== Perf Spy Process ===\n");
    printf("Monitoring PID: %d\n", target_pid);
    printf("Setting up performance counters...\n\n");
    
    // Setup multiple cache-related performance counters
    perf_counter_t counters[4];
    int num_counters = 0;
    
    // L1 data cache misses
    counters[num_counters].fd = setup_perf_counter(target_pid, 
        PERF_TYPE_HW_CACHE,
        (PERF_COUNT_HW_CACHE_L1D) | 
        (PERF_COUNT_HW_CACHE_OP_READ << 8) |
        (PERF_COUNT_HW_CACHE_RESULT_MISS << 16),
        "L1D Cache Misses");
    counters[num_counters].name = "L1D Cache Misses";
    if (counters[num_counters].fd >= 0) num_counters++;
    
    // L1 instruction cache misses
    counters[num_counters].fd = setup_perf_counter(target_pid,
        PERF_TYPE_HW_CACHE,
        (PERF_COUNT_HW_CACHE_L1I) |
        (PERF_COUNT_HW_CACHE_OP_READ << 8) |
        (PERF_COUNT_HW_CACHE_RESULT_MISS << 16),
        "L1I Cache Misses");
    counters[num_counters].name = "L1I Cache Misses";
    if (counters[num_counters].fd >= 0) num_counters++;
    
    // LLC (Last Level Cache) misses
    counters[num_counters].fd = setup_perf_counter(target_pid,
        PERF_TYPE_HW_CACHE,
        (PERF_COUNT_HW_CACHE_LL) |
        (PERF_COUNT_HW_CACHE_OP_READ << 8) |
        (PERF_COUNT_HW_CACHE_RESULT_MISS << 16),
        "LLC Cache Misses");
    counters[num_counters].name = "LLC Cache Misses";
    if (counters[num_counters].fd >= 0) num_counters++;
    
    // Cache references
    counters[num_counters].fd = setup_perf_counter(target_pid,
        PERF_TYPE_HARDWARE,
        PERF_COUNT_HW_CACHE_REFERENCES,
        "Cache References");
    counters[num_counters].name = "Cache References";
    if (counters[num_counters].fd >= 0) num_counters++;
    
    if (num_counters == 0) {
        fprintf(stderr, "Failed to setup any performance counters\n");
        fprintf(stderr, "Make sure you have root privileges and perf events are enabled\n");
        fprintf(stderr, "Try: echo -1 | sudo tee /proc/sys/kernel/perf_event_paranoid\n");
        return 1;
    }
    
    printf("Successfully configured %d counters\n", num_counters);
    printf("Starting monitoring...\n\n");
    
    // Enable all counters
    for (int i = 0; i < num_counters; i++) {
        ioctl(counters[i].fd, PERF_EVENT_IOC_RESET, 0);
        ioctl(counters[i].fd, PERF_EVENT_IOC_ENABLE, 0);
    }
    
    // Monitor loop
    printf("%-20s ", "Time");
    for (int i = 0; i < num_counters; i++) {
        printf("%-20s ", counters[i].name);
    }
    printf("\n");
    printf("%s\n", "================================================================================");
    
    long long prev_counts[4] = {0};
    int sample = 0;
    
    while (1) {
        sleep(1);
        
        printf("%-20d ", sample++);
        
        for (int i = 0; i < num_counters; i++) {
            long long count;
            if (read(counters[i].fd, &count, sizeof(long long)) == sizeof(long long)) {
                long long delta = count - prev_counts[i];
                prev_counts[i] = count;
                printf("%-20lld ", delta);
            } else {
                printf("%-20s ", "ERROR");
            }
        }
        printf("\n");
        fflush(stdout);
    }
    
    // Cleanup
    for (int i = 0; i < num_counters; i++) {
        close(counters[i].fd);
    }
    
    return 0;
}
