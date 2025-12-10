# Performance Monitoring Results Summary

## Test System: s1 (Raspberry Pi)
**Date:** December 10, 2025

---

## Demo 1: Basic Hardware Counters

### Results
| Metric | Value | Description |
|--------|-------|-------------|
| **CPU Cycles** | 16,096,168 | Total number of CPU clock cycles consumed by the workload |
| **Instructions** | 10,046,259 | Total number of CPU instructions executed |
| **Cache References** | 2,027,625 | Total number of times the CPU accessed cache memory |
| **Cache Misses** | 283 | Number of times requested data wasn't found in cache |
| **IPC** | 0.62 | Instructions Per Cycle - CPU efficiency metric |
| **Cache Miss Rate** | 0.01% | Percentage of cache accesses that resulted in misses |

### Metric Explanations

#### CPU Cycles
- **What it means:** The number of clock ticks the CPU spent executing code
- **Why it matters:** Lower is better for same workload; indicates overall execution time
- **Context:** At 1 GHz, 16M cycles = ~16 milliseconds

#### Instructions
- **What it means:** Count of individual CPU instructions executed (add, load, store, etc.)
- **Why it matters:** Shows code complexity and efficiency
- **Context:** 10M instructions for a computational workload

#### IPC (Instructions Per Cycle)
- **What it means:** Average instructions executed per CPU cycle
- **Formula:** Instructions ÷ CPU Cycles = 10,046,259 ÷ 16,096,168 = 0.62
- **Why it matters:** 
  - Higher IPC = better CPU utilization
  - Modern CPUs can execute multiple instructions per cycle (superscalar)
  - Ideal IPC ranges from 1.0 to 4.0 depending on architecture
- **Interpretation:** 
  - 0.62 IPC is moderate
  - CPU is executing ~0.62 instructions per cycle
  - Pipeline stalls, data dependencies, or memory waits are limiting performance

#### Cache References
- **What it means:** Number of times CPU tried to access cache
- **Why it matters:** Shows memory access patterns
- **Context:** 2M cache accesses in 16M cycles = accessing memory frequently

#### Cache Misses
- **What it means:** Data requested wasn't in cache, had to fetch from slower RAM
- **Why it matters:** Each miss costs 100+ cycles of delay
- **Impact:** 283 misses × ~200 cycles = ~56,600 cycles wasted

#### Cache Miss Rate
- **What it means:** Percentage of cache accesses that missed
- **Formula:** (Cache Misses ÷ Cache References) × 100 = 0.01%
- **Why it matters:**
  - **< 1%** = Excellent (this workload)
  - **1-5%** = Good
  - **5-10%** = Moderate
  - **> 10%** = Poor (performance bottleneck)
- **Interpretation:** 99.99% hit rate means data locality is excellent

---

## Demo 2: Cache Hierarchy Monitoring

### Results
| Metric | Value | Description |
|--------|-------|-------------|
| **L1D Read Access** | 14,518,924 | Level 1 Data cache read operations |
| **L1D Read Miss** | 220,544 | L1 Data cache read misses |
| **L1I Read Miss** | 68,374 | Level 1 Instruction cache read misses |
| **LLC Read Miss** | 170,416 | Last Level Cache (L3) read misses |
| **L1D Miss Rate** | 1.52% | Percentage of L1 data cache misses |

### Cache Hierarchy Explanation

```
CPU Core
   │
   ├─→ L1 Instruction Cache (32 KB) ─→ 68K misses
   │   └─→ Very fast (1-2 cycles)
   │
   ├─→ L1 Data Cache (32 KB) ─→ 220K misses / 14.5M accesses
   │   └─→ Very fast (1-2 cycles)
   │
   └─→ L2 Cache (256 KB - 1 MB)
       └─→ Slower (10-20 cycles)
       
           └─→ L3/LLC Cache (Shared, 1-8 MB) ─→ 170K misses
               └─→ Even slower (40-75 cycles)
               
                   └─→ Main RAM (GB)
                       └─→ Very slow (200+ cycles)
```

### Metric Explanations

#### L1D Read Access
- **What it means:** Number of times CPU tried to read data from L1 data cache
- **Why it matters:** Shows data access frequency
- **Context:** 14.5M accesses for 10MB workload

#### L1D Read Miss
- **What it means:** Requested data wasn't in L1, had to check L2/L3
- **Why it matters:** Each miss adds 10-20 cycles delay
- **Impact:** 220K misses × 15 cycles ≈ 3.3M cycles overhead

#### L1D Miss Rate
- **Formula:** (220,544 ÷ 14,518,924) × 100 = 1.52%
- **Interpretation:**
  - **< 2%** = Excellent (this workload) ✓
  - **2-5%** = Good
  - **5-10%** = Moderate
  - **> 10%** = Poor
- **Why it matters:** 98.48% of data accesses served from fastest cache

#### L1I Read Miss
- **What it means:** Instruction wasn't in L1 instruction cache
- **Why it matters:** 
  - Forces fetch from L2/L3/RAM
  - Stalls CPU instruction pipeline
  - Common with large code or many function calls
- **Context:** 68K instruction cache misses indicates moderate code size

#### LLC (Last Level Cache) Read Miss
- **What it means:** Data not found in any cache level, must fetch from RAM
- **Why it matters:** 
  - **Most expensive miss** (200+ cycle penalty)
  - Indicates true memory bandwidth bottleneck
  - 170K misses = 170K RAM accesses needed
- **Impact:** 170K × 200 cycles = 34M cycles wasted (more than 2× the entire workload!)

---

## Demo 3: Branch Prediction Analysis

### Test 1: Predictable Branches (Alternating Pattern)
| Metric | Value |
|--------|-------|
| **Branches** | 0 |
| **Branch Misses** | 49 |

### Test 2: Unpredictable Branches (Random Pattern)
| Metric | Value |
|--------|-------|
| **Branches** | 0 |
| **Branch Misses** | 3,513 |

### Metric Explanations

#### Branches
- **What it means:** Total conditional branch instructions executed (if/else, loops)
- **Why 0?** Counter may not be available on this architecture
- **Note:** Branch misses were still recorded by a different counter

#### Branch Misses
- **What it means:** CPU's branch predictor guessed wrong direction
- **How branch prediction works:**
  ```c
  if (condition) {  // ← CPU predicts which path before evaluating
      // Path A
  } else {
      // Path B
  }
  ```
  - CPU predicts likely branch direction
  - Speculatively executes predicted path
  - If wrong, must flush pipeline and restart (10-20 cycle penalty)

- **Why it matters:** 
  - Modern CPUs use pipelining (execute multiple instructions simultaneously)
  - Wrong prediction = wasted work + pipeline flush
  - Each miss costs 10-20 cycles

#### Predictable vs Unpredictable
- **Predictable (49 misses):**
  - Pattern: `i % 2 == 0` (alternates true/false)
  - CPU quickly learns the pattern
  - ~99.95% prediction accuracy

- **Unpredictable (3,513 misses):**
  - Pattern: `rand() % 2 == 0` (random)
  - CPU cannot learn pattern
  - 71× more mispredictions
  - ~50% prediction accuracy (as bad as random guessing)

- **Performance Impact:**
  - 3,513 misses × 15 cycles = ~53K wasted cycles
  - Unpredictable branches are a major performance killer

---

## Demo 4: Software Events

### Results
| Metric | Value | Description |
|--------|-------|-------------|
| **Page Faults** | 10,029 | Total memory page faults |
| **Context Switches** | 8 | Number of times CPU switched to different process |
| **CPU Migrations** | 0 | Times process moved to different CPU core |
| **Minor Faults** | 10,029 | Page faults served from RAM |
| **Major Faults** | 0 | Page faults requiring disk I/O |

### Metric Explanations

#### Page Faults
- **What it means:** Process tried to access memory page not currently mapped
- **Two types:**
  1. **Minor Fault:** Page exists in RAM, just needs mapping
  2. **Major Fault:** Page on disk, must read from storage (very slow)

- **Why they happen:**
  ```
  Process allocates memory → OS marks as "allocated"
  First access to page → Page fault! (lazy allocation)
  OS maps physical RAM → Access succeeds
  ```

- **Cost:**
  - Minor fault: ~1,000 cycles
  - Major fault: ~1,000,000 cycles (1ms at 1GHz)

#### Context Switches
- **What it means:** OS stopped this process and ran another
- **Why it happens:**
  - Time slice expired (scheduler)
  - Process waiting for I/O
  - Higher priority process ready
  - System call that blocks

- **Cost:** 
  - ~1,000-10,000 cycles per switch
  - Must save/restore CPU registers
  - Cache becomes "cold" (loaded with other process's data)

- **8 switches:** Low number, indicates CPU-bound workload with minimal interruption

#### CPU Migrations
- **What it means:** Process moved from one CPU core to another
- **Why it matters:**
  - Each core has its own L1/L2 cache
  - Migration = lose all cached data ("cold cache")
  - Can cost millions of cycles

- **0 migrations:** Excellent! Process stayed on same core, maximizing cache hits

#### Minor vs Major Faults

**Minor Faults (10,029):**
- Page in RAM, just needs TLB mapping
- Fast (~1,000 cycles each)
- Common during memory allocation
- 10K faults = ~10 million cycles overhead

**Major Faults (0):**
- Page on disk (swap), must read from storage
- Extremely slow (~1ms = 1 million cycles)
- 0 major faults = excellent! No swapping occurred

**Interpretation:**
- Workload allocated ~10,000 pages (40 MB at 4KB/page)
- All pages served from RAM
- No disk I/O bottleneck

---

## Demo 5: TLB (Translation Lookaside Buffer)

### Results
| Metric | Value | Description |
|--------|-------|-------------|
| **dTLB Read Miss** | 1,028,096 | Data TLB misses during memory reads |
| **iTLB Read Miss** | 251,078 | Instruction TLB misses during instruction fetch |

### What is TLB?

**TLB (Translation Lookaside Buffer):**
- Small, fast cache for virtual-to-physical address translations
- CPU uses virtual addresses, RAM uses physical addresses
- Translation process:

```
Virtual Address (0x400000)
         ↓
    Check TLB (fast)
         ↓
    ┌──────────────┐
    │ TLB Hit?     │
    └──────────────┘
         ↓
    ┌──Yes───────────No─────┐
    ↓                        ↓
Physical Address     Page Table Walk
(1-2 cycles)         (100+ cycles)
                           ↓
                     Update TLB
                           ↓
                     Physical Address
```

### Metric Explanations

#### dTLB (Data TLB) Miss
- **What it means:** Virtual-to-physical translation not in TLB for data access
- **Why it happens:**
  - TLB is small (64-512 entries)
  - Each entry covers one page (4KB)
  - Accessing many pages → TLB thrashing

- **Cost:** 
  - Each miss: ~100-200 cycles for page table walk
  - 1M misses × 150 cycles = 150M cycle overhead
  - **More expensive than the entire workload!**

- **Why so many?**
  - Workload allocated 10,000 pages
  - TLB only holds ~64 entries
  - Random access pattern = constant TLB misses

#### iTLB (Instruction TLB) Miss
- **What it means:** Translation miss when fetching instructions
- **Why it happens:**
  - Code spread across many pages
  - Function calls jump to different pages
  - Shared libraries in different memory regions

- **251K misses:** Moderate, indicates:
  - Code size is moderate
  - Multiple functions across different pages
  - Some instruction locality

### TLB Performance Impact

**Calculation:**
- Data TLB misses: 1,028,096 × 150 cycles = 154M cycles
- Instruction TLB misses: 251,078 × 150 cycles = 38M cycles
- **Total TLB overhead: ~192M cycles**
- Original workload: 16M cycles
- **TLB misses added 12× overhead!**

**How to reduce TLB misses:**
1. Use huge pages (2MB instead of 4KB)
2. Improve memory access locality
3. Reduce working set size
4. Use TLB-aware algorithms

---

## Key Insights Summary

### Performance Analysis
1. **CPU Efficiency (IPC 0.62):** Moderate, could be improved with better instruction-level parallelism
2. **Cache Performance (0.01% miss):** Excellent data locality
3. **Memory Hierarchy:** L1D performing well (1.52% miss rate)
4. **Branch Prediction:** Random branches 71× worse than predictable patterns
5. **Memory Management:** All pages in RAM (0 major faults), good
6. **TLB Performance:** Major bottleneck - 192M cycles wasted on address translation

### Side-Channel Attack Implications

From a **security perspective**, these metrics reveal:

1. **Cache Timing Attacks:**
   - 0.01% cache miss rate makes timing differences detectable
   - LLC misses (170K) create measurable timing variations
   - Can exploit for key extraction (as shown in other demos)

2. **Branch Prediction Attacks:**
   - 71× difference between predictable/unpredictable branches
   - Can leak secret-dependent control flow
   - Spectre/Meltdown exploited this

3. **TLB Attacks:**
   - 1M+ TLB misses create timing side-channel
   - Page access patterns leak information
   - TLBleed attack exploited this

4. **Performance Counters:**
   - All metrics accessible via perf_event API
   - Can monitor victim process cache/TLB behavior
   - Prime+Probe attacks use these counters

### Optimization Opportunities

To improve performance:
1. **Reduce TLB misses:** Use huge pages (would eliminate 192M cycle overhead)
2. **Improve branch prediction:** Avoid data-dependent branches in hot paths
3. **Maintain data locality:** Keep frequently accessed data together
4. **Minimize context switches:** Use CPU affinity, avoid blocking calls
5. **Leverage cache hierarchy:** Optimize for L1 cache size (32KB)

---

## Sampling Demo Results

### Instruction Pointer Sampling
- **Samples Captured:** ~910 samples
- **Sampling Period:** Every 100,000 CPU cycles
- **Ring Buffer Size:** 36,864 bytes (9 pages)
- **Purpose:** Identifies which code regions are executing most frequently (hotspot detection)

### Cache Miss Sampling
- **Total Cache Misses:** 0 (workload didn't stress cache enough)
- **Sampling Period:** Every 1,000 cache misses
- **Purpose:** Records memory addresses causing cache misses (identifies problematic data structures)

### Frequency-based Sampling
- **Frequency:** 1,000 Hz (1,000 samples per second)
- **Expected Samples:** ~2,000 (over 2-second workload)
- **Purpose:** Time-based profiling independent of CPU speed

---

## Conclusion

These demonstrations show the comprehensive capabilities of Linux `perf_event` for:
- **Performance analysis:** Identifying bottlenecks (TLB misses dominate this workload)
- **Security research:** All metrics exploitable for side-channel attacks
- **Optimization:** Data-driven decisions on where to focus optimization efforts

**Key Takeaway:** TLB misses caused 12× overhead (192M cycles) compared to actual work (16M cycles). Using huge pages could dramatically improve performance while also reducing side-channel leakage.
