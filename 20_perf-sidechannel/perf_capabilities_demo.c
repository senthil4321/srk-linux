/**
 * perf_capabilities_demo.c
 * 
 * Comprehensive demonstration of Linux perf_event capabilities
 * Shows various hardware and software performance counters
 * 
 * Capabilities demonstrated:
 * 1. Hardware counters (CPU cycles, instructions, cache, branches)
 * 2. Software counters (page faults, context switches)
 * 3. Cache hierarchy monitoring (L1, L2, LLC)
 * 4. Branch prediction analysis
 * 5. TLB monitoring
 * 6. Multiple counter groups
 * 
 * Compile: gcc -O2 -o perf_capabilities_demo perf_capabilities_demo.c
 * Run: sudo ./perf_capabilities_demo
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>
#include <errno.h>
#include <stdint.h>
#include <time.h>

// Perf event syscall wrapper
static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                           int cpu, int group_fd, unsigned long flags) {
    return syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}

// Hardware counter configuration
typedef struct {
    const char *name;
    uint32_t type;
    uint64_t config;
    int fd;
    uint64_t value;
} PerfCounter;

// Initialize a perf counter
int init_counter(PerfCounter *counter) {
    struct perf_event_attr pe;
    
    memset(&pe, 0, sizeof(struct perf_event_attr));
    pe.type = counter->type;
    pe.size = sizeof(struct perf_event_attr);
    pe.config = counter->config;
    pe.disabled = 1;
    pe.exclude_kernel = 0;
    pe.exclude_hv = 1;
    
    counter->fd = perf_event_open(&pe, 0, -1, -1, 0);
    if (counter->fd == -1) {
        fprintf(stderr, "Warning: Failed to open %s: %s\n", 
                counter->name, strerror(errno));
        return -1;
    }
    return 0;
}

// Read counter value
void read_counter(PerfCounter *counter) {
    if (counter->fd != -1) {
        if (read(counter->fd, &counter->value, sizeof(uint64_t)) == -1) {
            counter->value = 0;
        }
    } else {
        counter->value = 0;
    }
}

// Reset counter
void reset_counter(PerfCounter *counter) {
    if (counter->fd != -1) {
        ioctl(counter->fd, PERF_EVENT_IOC_RESET, 0);
    }
}

// Enable counter
void enable_counter(PerfCounter *counter) {
    if (counter->fd != -1) {
        ioctl(counter->fd, PERF_EVENT_IOC_ENABLE, 0);
    }
}

// Disable counter
void disable_counter(PerfCounter *counter) {
    if (counter->fd != -1) {
        ioctl(counter->fd, PERF_EVENT_IOC_DISABLE, 0);
    }
}

// Close counter
void close_counter(PerfCounter *counter) {
    if (counter->fd != -1) {
        close(counter->fd);
        counter->fd = -1;
    }
}

// ============================================================================
// Workload Functions for Testing
// ============================================================================

// CPU-intensive workload
void cpu_intensive_work(int iterations) {
    volatile double result = 0.0;
    for (int i = 0; i < iterations; i++) {
        result += i * 3.14159 / (i + 1);
    }
}

// Memory-intensive workload (cache misses)
void memory_intensive_work(int size_mb) {
    size_t size = size_mb * 1024 * 1024;
    char *buffer = malloc(size);
    if (!buffer) return;
    
    // Random access pattern to cause cache misses
    for (size_t i = 0; i < size; i += 64) {  // 64-byte cache line
        buffer[i] = (char)(i & 0xFF);
    }
    
    // Read back with stride to cause more misses
    volatile char sum = 0;
    for (size_t i = 0; i < size; i += 4096) {  // Page stride
        sum += buffer[i];
    }
    
    free(buffer);
}

// Branch-heavy workload (predictable)
void predictable_branches(int iterations) {
    int sum = 0;
    for (int i = 0; i < iterations; i++) {
        if (i % 2 == 0) {  // Predictable pattern
            sum += i;
        } else {
            sum -= i;
        }
    }
}

// Branch-heavy workload (unpredictable)
void unpredictable_branches(int iterations) {
    int sum = 0;
    srand(42);  // Fixed seed for reproducibility
    for (int i = 0; i < iterations; i++) {
        if (rand() % 2 == 0) {  // Random pattern
            sum += i;
        } else {
            sum -= i;
        }
    }
}

// TLB-intensive workload
void tlb_intensive_work() {
    const int num_pages = 10000;
    const int page_size = 4096;
    
    // Allocate many pages
    char **pages = malloc(num_pages * sizeof(char*));
    for (int i = 0; i < num_pages; i++) {
        pages[i] = malloc(page_size);
        if (pages[i]) {
            pages[i][0] = i & 0xFF;  // Touch first byte of each page
        }
    }
    
    // Random page access to stress TLB
    srand(42);
    volatile char sum = 0;
    for (int i = 0; i < num_pages * 10; i++) {
        int page_idx = rand() % num_pages;
        if (pages[page_idx]) {
            sum += pages[page_idx][0];
        }
    }
    
    // Cleanup
    for (int i = 0; i < num_pages; i++) {
        free(pages[i]);
    }
    free(pages);
}

// ============================================================================
// Demonstration Functions
// ============================================================================

void demo_basic_hw_counters() {
    printf("\n");
    printf("========================================\n");
    printf("Demo 1: Basic Hardware Counters\n");
    printf("========================================\n");
    
    PerfCounter counters[] = {
        {"CPU Cycles", PERF_TYPE_HARDWARE, PERF_COUNT_HW_CPU_CYCLES, -1, 0},
        {"Instructions", PERF_TYPE_HARDWARE, PERF_COUNT_HW_INSTRUCTIONS, -1, 0},
        {"Cache References", PERF_TYPE_HARDWARE, PERF_COUNT_HW_CACHE_REFERENCES, -1, 0},
        {"Cache Misses", PERF_TYPE_HARDWARE, PERF_COUNT_HW_CACHE_MISSES, -1, 0},
    };
    int num_counters = sizeof(counters) / sizeof(counters[0]);
    
    // Initialize counters
    for (int i = 0; i < num_counters; i++) {
        init_counter(&counters[i]);
    }
    
    // Enable and run workload
    for (int i = 0; i < num_counters; i++) {
        reset_counter(&counters[i]);
        enable_counter(&counters[i]);
    }
    
    printf("\nRunning CPU-intensive workload...\n");
    cpu_intensive_work(1000000);
    
    // Disable and read
    for (int i = 0; i < num_counters; i++) {
        disable_counter(&counters[i]);
        read_counter(&counters[i]);
    }
    
    // Display results
    printf("\nResults:\n");
    for (int i = 0; i < num_counters; i++) {
        printf("  %-20s: %lu\n", counters[i].name, counters[i].value);
    }
    
    // Calculate IPC (Instructions Per Cycle)
    if (counters[0].value > 0) {
        double ipc = (double)counters[1].value / counters[0].value;
        printf("\n  IPC (Instructions/Cycle): %.2f\n", ipc);
    }
    
    // Calculate cache miss rate
    if (counters[2].value > 0) {
        double miss_rate = (double)counters[3].value / counters[2].value * 100.0;
        printf("  Cache Miss Rate: %.2f%%\n", miss_rate);
    }
    
    // Cleanup
    for (int i = 0; i < num_counters; i++) {
        close_counter(&counters[i]);
    }
}

void demo_cache_hierarchy() {
    printf("\n");
    printf("========================================\n");
    printf("Demo 2: Cache Hierarchy Monitoring\n");
    printf("========================================\n");
    
    PerfCounter counters[] = {
        {"L1D Read Access", PERF_TYPE_HW_CACHE, 
         (PERF_COUNT_HW_CACHE_L1D) | (PERF_COUNT_HW_CACHE_OP_READ << 8) | 
         (PERF_COUNT_HW_CACHE_RESULT_ACCESS << 16), -1, 0},
        {"L1D Read Miss", PERF_TYPE_HW_CACHE,
         (PERF_COUNT_HW_CACHE_L1D) | (PERF_COUNT_HW_CACHE_OP_READ << 8) | 
         (PERF_COUNT_HW_CACHE_RESULT_MISS << 16), -1, 0},
        {"L1I Read Miss", PERF_TYPE_HW_CACHE,
         (PERF_COUNT_HW_CACHE_L1I) | (PERF_COUNT_HW_CACHE_OP_READ << 8) | 
         (PERF_COUNT_HW_CACHE_RESULT_MISS << 16), -1, 0},
        {"LLC Read Miss", PERF_TYPE_HW_CACHE,
         (PERF_COUNT_HW_CACHE_LL) | (PERF_COUNT_HW_CACHE_OP_READ << 8) | 
         (PERF_COUNT_HW_CACHE_RESULT_MISS << 16), -1, 0},
    };
    int num_counters = sizeof(counters) / sizeof(counters[0]);
    
    // Initialize counters
    int active_counters = 0;
    for (int i = 0; i < num_counters; i++) {
        if (init_counter(&counters[i]) == 0) {
            active_counters++;
        }
    }
    
    if (active_counters == 0) {
        printf("No cache counters available on this system.\n");
        return;
    }
    
    // Enable and run workload
    for (int i = 0; i < num_counters; i++) {
        reset_counter(&counters[i]);
        enable_counter(&counters[i]);
    }
    
    printf("\nRunning memory-intensive workload (10 MB)...\n");
    memory_intensive_work(10);
    
    // Disable and read
    for (int i = 0; i < num_counters; i++) {
        disable_counter(&counters[i]);
        read_counter(&counters[i]);
    }
    
    // Display results
    printf("\nResults:\n");
    for (int i = 0; i < num_counters; i++) {
        if (counters[i].fd != -1) {
            printf("  %-20s: %lu\n", counters[i].name, counters[i].value);
        }
    }
    
    // Calculate L1D miss rate
    if (counters[0].fd != -1 && counters[0].value > 0) {
        double l1d_miss_rate = (double)counters[1].value / counters[0].value * 100.0;
        printf("\n  L1D Miss Rate: %.2f%%\n", l1d_miss_rate);
    }
    
    // Cleanup
    for (int i = 0; i < num_counters; i++) {
        close_counter(&counters[i]);
    }
}

void demo_branch_prediction() {
    printf("\n");
    printf("========================================\n");
    printf("Demo 3: Branch Prediction Analysis\n");
    printf("========================================\n");
    
    PerfCounter counters[] = {
        {"Branches", PERF_TYPE_HARDWARE, PERF_COUNT_HW_BRANCH_INSTRUCTIONS, -1, 0},
        {"Branch Misses", PERF_TYPE_HARDWARE, PERF_COUNT_HW_BRANCH_MISSES, -1, 0},
    };
    int num_counters = sizeof(counters) / sizeof(counters[0]);
    
    // Initialize counters
    for (int i = 0; i < num_counters; i++) {
        init_counter(&counters[i]);
    }
    
    // Test 1: Predictable branches
    printf("\nTest 1: Predictable branches (alternating pattern)...\n");
    for (int i = 0; i < num_counters; i++) {
        reset_counter(&counters[i]);
        enable_counter(&counters[i]);
    }
    
    predictable_branches(100000);
    
    for (int i = 0; i < num_counters; i++) {
        disable_counter(&counters[i]);
        read_counter(&counters[i]);
    }
    
    printf("Results:\n");
    for (int i = 0; i < num_counters; i++) {
        printf("  %-20s: %lu\n", counters[i].name, counters[i].value);
    }
    if (counters[0].value > 0) {
        double miss_rate = (double)counters[1].value / counters[0].value * 100.0;
        printf("  Branch Miss Rate: %.2f%%\n", miss_rate);
    }
    
    // Test 2: Unpredictable branches
    printf("\nTest 2: Unpredictable branches (random pattern)...\n");
    for (int i = 0; i < num_counters; i++) {
        reset_counter(&counters[i]);
        enable_counter(&counters[i]);
    }
    
    unpredictable_branches(100000);
    
    for (int i = 0; i < num_counters; i++) {
        disable_counter(&counters[i]);
        read_counter(&counters[i]);
    }
    
    printf("Results:\n");
    for (int i = 0; i < num_counters; i++) {
        printf("  %-20s: %lu\n", counters[i].name, counters[i].value);
    }
    if (counters[0].value > 0) {
        double miss_rate = (double)counters[1].value / counters[0].value * 100.0;
        printf("  Branch Miss Rate: %.2f%% (higher due to randomness)\n", miss_rate);
    }
    
    // Cleanup
    for (int i = 0; i < num_counters; i++) {
        close_counter(&counters[i]);
    }
}

void demo_software_events() {
    printf("\n");
    printf("========================================\n");
    printf("Demo 4: Software Events\n");
    printf("========================================\n");
    
    PerfCounter counters[] = {
        {"Page Faults", PERF_TYPE_SOFTWARE, PERF_COUNT_SW_PAGE_FAULTS, -1, 0},
        {"Context Switches", PERF_TYPE_SOFTWARE, PERF_COUNT_SW_CONTEXT_SWITCHES, -1, 0},
        {"CPU Migrations", PERF_TYPE_SOFTWARE, PERF_COUNT_SW_CPU_MIGRATIONS, -1, 0},
        {"Minor Faults", PERF_TYPE_SOFTWARE, PERF_COUNT_SW_PAGE_FAULTS_MIN, -1, 0},
        {"Major Faults", PERF_TYPE_SOFTWARE, PERF_COUNT_SW_PAGE_FAULTS_MAJ, -1, 0},
    };
    int num_counters = sizeof(counters) / sizeof(counters[0]);
    
    // Initialize counters
    for (int i = 0; i < num_counters; i++) {
        init_counter(&counters[i]);
    }
    
    // Enable and run workload
    for (int i = 0; i < num_counters; i++) {
        reset_counter(&counters[i]);
        enable_counter(&counters[i]);
    }
    
    printf("\nRunning TLB-intensive workload...\n");
    tlb_intensive_work();
    
    // Disable and read
    for (int i = 0; i < num_counters; i++) {
        disable_counter(&counters[i]);
        read_counter(&counters[i]);
    }
    
    // Display results
    printf("\nResults:\n");
    for (int i = 0; i < num_counters; i++) {
        printf("  %-20s: %lu\n", counters[i].name, counters[i].value);
    }
    
    // Cleanup
    for (int i = 0; i < num_counters; i++) {
        close_counter(&counters[i]);
    }
}

void demo_tlb_monitoring() {
    printf("\n");
    printf("========================================\n");
    printf("Demo 5: TLB (Translation Lookaside Buffer)\n");
    printf("========================================\n");
    
    PerfCounter counters[] = {
        {"dTLB Read Access", PERF_TYPE_HW_CACHE,
         (PERF_COUNT_HW_CACHE_DTLB) | (PERF_COUNT_HW_CACHE_OP_READ << 8) | 
         (PERF_COUNT_HW_CACHE_RESULT_ACCESS << 16), -1, 0},
        {"dTLB Read Miss", PERF_TYPE_HW_CACHE,
         (PERF_COUNT_HW_CACHE_DTLB) | (PERF_COUNT_HW_CACHE_OP_READ << 8) | 
         (PERF_COUNT_HW_CACHE_RESULT_MISS << 16), -1, 0},
        {"iTLB Read Miss", PERF_TYPE_HW_CACHE,
         (PERF_COUNT_HW_CACHE_ITLB) | (PERF_COUNT_HW_CACHE_OP_READ << 8) | 
         (PERF_COUNT_HW_CACHE_RESULT_MISS << 16), -1, 0},
    };
    int num_counters = sizeof(counters) / sizeof(counters[0]);
    
    // Initialize counters
    int active_counters = 0;
    for (int i = 0; i < num_counters; i++) {
        if (init_counter(&counters[i]) == 0) {
            active_counters++;
        }
    }
    
    if (active_counters == 0) {
        printf("TLB counters not available on this system.\n");
        return;
    }
    
    // Enable and run workload
    for (int i = 0; i < num_counters; i++) {
        reset_counter(&counters[i]);
        enable_counter(&counters[i]);
    }
    
    printf("\nRunning TLB-intensive workload...\n");
    tlb_intensive_work();
    
    // Disable and read
    for (int i = 0; i < num_counters; i++) {
        disable_counter(&counters[i]);
        read_counter(&counters[i]);
    }
    
    // Display results
    printf("\nResults:\n");
    for (int i = 0; i < num_counters; i++) {
        if (counters[i].fd != -1) {
            printf("  %-20s: %lu\n", counters[i].name, counters[i].value);
        }
    }
    
    // Calculate dTLB miss rate
    if (counters[0].fd != -1 && counters[0].value > 0) {
        double dtlb_miss_rate = (double)counters[1].value / counters[0].value * 100.0;
        printf("\n  dTLB Miss Rate: %.2f%%\n", dtlb_miss_rate);
    }
    
    // Cleanup
    for (int i = 0; i < num_counters; i++) {
        close_counter(&counters[i]);
    }
}

// ============================================================================
// Main
// ============================================================================

int main() {
    printf("========================================\n");
    printf("Linux perf_event Capabilities Demo\n");
    printf("========================================\n");
    printf("\nThis program demonstrates various performance\n");
    printf("monitoring capabilities using perf_event API.\n");
    
    demo_basic_hw_counters();
    demo_cache_hierarchy();
    demo_branch_prediction();
    demo_software_events();
    demo_tlb_monitoring();
    
    printf("\n");
    printf("========================================\n");
    printf("Demo Complete!\n");
    printf("========================================\n");
    printf("\nKey Insights:\n");
    printf("- IPC shows CPU efficiency (higher is better)\n");
    printf("- Cache miss rate impacts performance\n");
    printf("- Branch mispredictions cause pipeline stalls\n");
    printf("- Page faults indicate memory pressure\n");
    printf("- TLB misses slow down virtual address translation\n");
    printf("\nNote: Some counters may not be available depending\n");
    printf("on your CPU architecture and kernel configuration.\n");
    
    return 0;
}
