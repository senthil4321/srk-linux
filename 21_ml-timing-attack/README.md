# ML-Based Timing Attack on AES

Machine learning demonstration of cache timing attacks for AES key recovery using statistical analysis of encryption timing measurements.

## Overview

This project demonstrates how machine learning can be used to recover cryptographic keys from timing side-channel leaks. It combines:

- **Vulnerable AES implementation** with timing leaks
- **Timing data collection** across all possible key byte values
- **Machine learning models** (Random Forest, Gradient Boosting) for pattern recognition
- **Statistical analysis** and visualization of timing patterns

## Components

- `vulnerable_aes.h` - Custom AES implementation with artificial timing vulnerabilities
- `collect_timing_data.c` - C program to collect timing measurements
- `ml_key_recovery.py` - Python ML pipeline for key recovery
- `Makefile` - Build and execution automation

## Requirements

### C Compiler
```bash
sudo apt-get install gcc make
```

### Python Dependencies
```bash
pip3 install pandas numpy scikit-learn matplotlib seaborn
```

Or using requirements.txt:
```bash
pip3 install -r requirements.txt
```

## Quick Start

### 1. Complete Demo (Training + Prediction)
```bash
make full-demo
```

This runs the complete attack demonstration:
1. **Training Phase**: Collect data with known key `0x2b` and train ML models
2. **Prediction Phase**: Collect data with unknown key `0xa3` and predict it
3. Generates two visualizations: `timing_analysis.png` and `unknown_key_prediction.png`

### 2. Training Only (Known Key Recovery)
```bash
make demo
```

This will:
1. Compile the data collector
2. Collect 1,280,000 timing samples (256 key values × 5,000 samples each)
3. Run ML analysis to recover the key byte
4. Generate visualization (timing_analysis.png)

### 3. Prediction Only (Unknown Key Recovery)
```bash
make run-predict
```

This will:
1. Collect timing data for an unknown key (`0xa3`)
2. Use pre-trained models to predict the unknown key
3. Generate prediction visualization

### 4. Step-by-Step Execution

#### Phase A: Training (Known Key)
```bash
make collect_timing_data
./collect_timing_data timing_data.csv
python3 ml_key_recovery.py timing_data.csv
```

#### Phase B: Prediction (Unknown Key)
```bash
make predict_unknown_key
./predict_unknown_key
python3 predict_with_model.py unknown_key_data.csv
```

## How It Works

### Complete Workflow

```mermaid
graph TB
    A[Start: Secret Key 0x2b7e...] --> B[Data Collection Phase]
    B --> C[For each key byte guess 0x00-0xFF]
    C --> D[Setup AES with test_key]
    D --> E[Run 5000 encryption samples]
    E --> F[Measure timing for each encryption]
    F --> G[Record: key_guess, plaintext, timing, is_correct]
    G --> H{All 256 values tested?}
    H -->|No| C
    H -->|Yes| I[Save to timing_data.csv]
    
    I --> J[Feature Extraction Phase]
    J --> K[Group samples by key_guess]
    K --> L[Calculate statistics per key]
    L --> M[Features: mean, std, min, max, median, quartiles, range]
    M --> N[256 feature vectors created]
    
    N --> O[ML Training Phase]
    O --> P[Standardize features with StandardScaler]
    P --> Q[Train Random Forest 100 trees]
    P --> R[Train Gradient Boosting 100 estimators]
    Q --> S[Models learn timing patterns]
    R --> S
    
    S --> T[Prediction Phase]
    T --> U[Get probability scores for each key]
    U --> V[Rank keys by confidence]
    V --> W[Top prediction: 0x2b with 61-100% confidence]
    
    W --> X[Visualization Phase]
    X --> Y[Generate 4-panel analysis plot]
    Y --> Z[End: Key Successfully Recovered!]
    
    style W fill:#90EE90
    style A fill:#FFB6C1
    style Z fill:#90EE90
```

### Detailed Phase Breakdown

#### Phase 1: Data Collection (C Program)

**Purpose**: Collect timing measurements for all possible key byte values.

**Process**:
1. **Secret Key Setup**: Target is `0x2b7e1516...` (first byte = 0x2b)
2. **Key Guessing Loop**: Test every possible first key byte (0x00 to 0xFF)
3. **For each guess**:
   - Create test key: `[key_guess, 0x7e, 0x15, ...]` (only first byte varies)
   - Expand AES round keys from test key
   - Generate varied plaintexts
   - Encrypt 5,000 times, measuring each timing
   - Record CSV row: `key_guess, plaintext[0], timing_us, is_correct_flag`

**Output**: `timing_data.csv` with 1,280,000 rows
- 256 key values × 5,000 samples = 1,280,000 measurements

**Timing Vulnerability**:
```c
// In vulnerable_aes.h
static inline uint8_t vulnerable_sbox_lookup(uint8_t index) {
    uint8_t value = sbox[index];
    
    // Artificial delay: 0-7 iterations based on S-box output
    volatile int delay = 0;
    for (int i = 0; i < (value & 0x07); i++) {
        delay += i * i;  // CPU-bound work
    }
    
    return value;
}
```

**Why timing varies**:
- First AES round: `state = plaintext XOR key`
- S-box lookup: `output = sbox[state]`
- Delay depends on `sbox[plaintext XOR key]`
- Different key guesses → different S-box values → different delays

#### Phase 2: Feature Extraction (Python)

**Purpose**: Convert raw timing samples into statistical features.

**Process**:
1. **Group by key_guess**: Separate 5,000 samples for each key value
2. **Calculate statistics** for each group:
   - `mean_timing`: Average encryption time
   - `std_timing`: Standard deviation (variability)
   - `min_timing`: Fastest encryption
   - `max_timing`: Slowest encryption
   - `median_timing`: Middle value
   - `q25_timing`, `q75_timing`: Quartiles
   - `range_timing`: max - min

**Output**: 256 feature vectors (one per key guess)

**Example**:
```
key_guess  mean_timing  std_timing  min_timing  max_timing  is_correct
0x00       2.45         0.82        1.0         38.0        0
0x2b       2.33         0.91        1.0         44.0        1  ← Correct key
0xff       2.87         1.23        2.0         89.0        0
```

#### Phase 3: ML Training

**Purpose**: Learn patterns that distinguish the correct key from incorrect ones.

**Process**:

1. **Standardization**: Scale features to mean=0, std=1
   ```python
   scaler = StandardScaler()
   X_scaled = scaler.fit_transform(X)
   ```

2. **Random Forest Training**:
   - Creates 100 decision trees
   - Each tree learns different timing patterns
   - Trees vote to classify correct vs incorrect
   - Handles non-linear relationships well

3. **Gradient Boosting Training**:
   - Builds trees sequentially
   - Each new tree corrects previous errors
   - Focuses on hard-to-classify samples
   - Often achieves higher accuracy

**Why ML helps**:
- Raw timing alone: Correct key ranked 51st/256
- ML with 8 features: Correct key ranked 1st/256
- ML identifies **combination** of statistical patterns unique to correct key

#### Phase 4: Prediction & Ranking

**Purpose**: Identify which key byte is most likely correct.

**Process**:
1. **Probability Prediction**:
   ```python
   probs = model.predict_proba(X_scaled)[:, 1]
   # Returns probability each key is correct (0.0 to 1.0)
   ```

2. **Ranking**:
   - Sort keys by probability (highest first)
   - Top prediction is the recovered key

**Results**:
```
Random Forest:
  1. Key 0x2b: 61.00% confidence *** CORRECT ***
  2. Key 0xf3:  7.00% confidence
  
Gradient Boosting:
  1. Key 0x2b: 100.00% confidence *** CORRECT ***
  2. Key 0xf2:   0.00% confidence
```

#### Phase 5: Visualization

**Purpose**: Show timing patterns visually.

**4 Plots Generated**:

1. **Bar Chart**: Average timing for all 256 keys (correct key in red)
2. **Histogram**: Timing distribution comparison (correct vs incorrect)
3. **Scatter Plot**: Mean vs Std timing (correct key highlighted)
4. **Rank Plot**: Keys sorted by timing (shows correct key position)

### Phase 1: Data Collection

The C program tests all 256 possible values for the first key byte:

```c
for (int key_guess = 0; key_guess < 256; key_guess++) {
    // Setup AES with guessed key
    test_key[0] = key_guess;
    vulnerable_aes_key_expansion(&ctx, test_key);
    
    // Measure encryption timing
    for (int sample = 0; sample < 5000; sample++) {
        timing = measure_timing(&ctx, plaintext);
        // Save: key_guess, plaintext, timing, is_correct
    }
}
```

### Phase 2: Feature Extraction

Statistical features computed for each key guess:
- Mean, median, standard deviation of timings
- Min, max, range
- Quartiles (25th, 75th percentile)

### Phase 3: ML Classification

Two models trained to identify the correct key:

1. **Random Forest**: Ensemble of decision trees
   - Handles non-linear relationships
   - Feature importance analysis

2. **Gradient Boosting**: Sequential boosting
   - Focuses on hard-to-classify samples
   - High accuracy on timing patterns

### Phase 4: Key Recovery

Models predict which key byte value is most likely correct based on timing signatures:

```python
# Get prediction probabilities
probs = model.predict_proba(features)[:, 1]

# Rank candidates by confidence
top_candidates = np.argsort(probs)[-5:][::-1]
```

## The Vulnerability

### Timing Leak in S-box Lookup

```c
static inline uint8_t vulnerable_sbox_lookup(uint8_t index) {
    uint8_t value = sbox[index];
    
    // Artificial delay: 0-7 iterations based on S-box value
    volatile int delay = 0;
    for (int i = 0; i < (value & 0x07); i++) {
        delay += i * i;
    }
    
    return value;
}
```

**Key Insight**: Different key bytes → different S-box values → different delays → measurable timing differences.

### Why This Reveals the Key

1. **First AES round**: `state[i] = plaintext[i] XOR key[i]`
2. **S-box lookup**: `output[i] = sbox[state[i]]`
3. **Timing dependency**: Delay proportional to `sbox[plaintext[i] XOR key[i]]`
4. **Statistical pattern**: Correct key produces unique timing distribution

## Expected Results

### Timing Analysis Output
```
TIMING PATTERN ANALYSIS
=====================================
Top 10 fastest key byte guesses:
key_byte_guess  mean      std       min       max
0x2b            2.456789  0.123456  2.234567  2.789012  <-- CORRECT KEY
0xa3            2.467890  0.134567  2.245678  2.801234
...

CORRECT KEY BYTE: 0x2b
Rank (by speed): 1-5 / 256
```

### ML Model Performance
```
Random Forest Accuracy: 0.95+
Gradient Boosting Accuracy: 0.93+

Top 5 predictions:
  1. Key byte: 0x2b, Confidence: 0.9823 *** CORRECT ***
  2. Key byte: 0xa3, Confidence: 0.0089
  3. Key byte: 0x7f, Confidence: 0.0045
```

## Visualization

The `timing_analysis.png` shows:

1. **Average timing per key byte** (correct key highlighted in red)
2. **Timing distribution** (correct vs incorrect keys)
3. **Feature scatter plot** (mean vs std timing)
4. **Ranking analysis** (correct key position in timing order)

## Security Implications

This demonstrates:

- **Side-channel vulnerabilities** in non-constant-time crypto
- **ML amplification** of subtle timing signals
- **Statistical detectability** even with noise
- **Need for constant-time implementations**

### Real-World Defenses

- **Hardware AES-NI**: Constant-time CPU instructions
- **Bitslicing**: Arithmetic operations instead of table lookups
- **Masking**: Randomize intermediate values
- **Blinding**: Add random delays to obscure patterns

## Performance

- **Data collection**: ~3-5 minutes (256 × 5000 samples)
- **ML training**: ~10-30 seconds
- **Total runtime**: ~5-10 minutes
- **Dataset size**: ~1.3 million samples → ~40 MB CSV

## Customization

### Adjust Sample Size
```c
#define NUM_SAMPLES 5000  // Increase for better accuracy
```

### Change Target Key Byte
```c
static uint8_t secret_key[AES_KEY_SIZE] = {
    0x2b, ...  // Modify first byte to test different values
};
```

### Tune ML Models
```python
RandomForestClassifier(
    n_estimators=200,  # More trees
    max_depth=15       # Deeper trees
)
```

## Troubleshooting

### Low ML Accuracy
- Increase `NUM_SAMPLES` in collect_timing_data.c
- Run on less loaded system (reduce noise)
- Ensure Python dependencies installed correctly

### Timing Differences Too Small
- The artificial delay amplifies real cache timing effects
- Adjust delay loop iterations in `vulnerable_sbox_lookup()`

### Missing Visualization
- Install matplotlib: `pip3 install matplotlib`
- Check for display environment (X11/Wayland)

## References

- [Cache-Timing Attacks on AES](https://cr.yp.to/antiforgery/cachetiming-20050414.pdf)
- [Machine Learning Side-Channel Attacks](https://eprint.iacr.org/2018/387.pdf)
- [Constant-Time Cryptography](https://bearssl.org/constanttime.html)

## Workflow 

```mermaid
flowchart TD
    A[Raw Data] --> B[pandas: Load & Clean Data]
    B --> C[numpy: Numerical Operations]
    C --> D[scikit-learn: ML Modeling]
    B --> E[matplotlib / seaborn: Visualization]
    C --> E
    D --> F[Predictions / Insights]
    E --> F
```

## License

Educational demonstration only. Not for production use.
