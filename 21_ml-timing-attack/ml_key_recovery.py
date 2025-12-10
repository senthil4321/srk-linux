#!/usr/bin/env python3
"""
ML-Based AES Key Recovery using Timing Analysis
Uses machine learning to identify the correct key byte from timing measurements
"""

import pandas as pd
import numpy as np
from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestClassifier, GradientBoostingClassifier
from sklearn.preprocessing import StandardScaler
from sklearn.metrics import accuracy_score, classification_report, confusion_matrix
import matplotlib.pyplot as plt
import seaborn as sns
import sys

def load_data(filename):
    """Load timing data from CSV file"""
    print(f"Loading data from {filename}...")
    df = pd.read_csv(filename)
    print(f"Loaded {len(df)} samples")
    print(f"Columns: {df.columns.tolist()}")
    return df

def extract_features(df):
    """Extract features from timing data"""
    print("\nExtracting features...")
    
    # Group by key_byte_guess to calculate statistics
    features_list = []
    
    for key_guess in range(256):
        subset = df[df['key_byte_guess'] == key_guess]
        
        if len(subset) == 0:
            continue
            
        timings = subset['timing_us'].values
        
        # Statistical features
        features = {
            'key_byte_guess': key_guess,
            'mean_timing': np.mean(timings),
            'std_timing': np.std(timings),
            'min_timing': np.min(timings),
            'max_timing': np.max(timings),
            'median_timing': np.median(timings),
            'q25_timing': np.percentile(timings, 25),
            'q75_timing': np.percentile(timings, 75),
            'range_timing': np.max(timings) - np.min(timings),
            'is_correct': subset['is_correct'].iloc[0]
        }
        
        features_list.append(features)
    
    features_df = pd.DataFrame(features_list)
    print(f"Extracted features for {len(features_df)} key byte values")
    
    return features_df

def train_model(X_train, X_test, y_train, y_test):
    """Train and evaluate ML models"""
    print("\nTraining models...")
    
    # Standardize features
    scaler = StandardScaler()
    X_train_scaled = scaler.fit_transform(X_train)
    X_test_scaled = scaler.transform(X_test)
    
    models = {
        'Random Forest': RandomForestClassifier(n_estimators=100, max_depth=10, random_state=42),
        'Gradient Boosting': GradientBoostingClassifier(n_estimators=100, max_depth=5, random_state=42)
    }
    
    results = {}
    
    for name, model in models.items():
        print(f"\nTraining {name}...")
        model.fit(X_train_scaled, y_train)
        
        y_pred = model.predict(X_test_scaled)
        accuracy = accuracy_score(y_test, y_pred)
        
        print(f"{name} Accuracy: {accuracy:.4f}")
        
        results[name] = {
            'model': model,
            'scaler': scaler,
            'accuracy': accuracy,
            'predictions': y_pred
        }
    
    return results

def analyze_timing_patterns(df):
    """Analyze timing patterns to identify correct key"""
    print("\n" + "="*60)
    print("TIMING PATTERN ANALYSIS")
    print("="*60)
    
    # Calculate average timing for each key guess
    timing_stats = df.groupby('key_byte_guess')['timing_us'].agg(['mean', 'std', 'min', 'max'])
    timing_stats = timing_stats.sort_values('mean')
    
    print("\nTop 10 fastest key byte guesses (most likely candidates):")
    print(timing_stats.head(10))
    
    print("\nTop 10 slowest key byte guesses:")
    print(timing_stats.tail(10))
    
    # Find correct key
    correct_key = df[df['is_correct'] == 1]['key_byte_guess'].iloc[0]
    correct_timing = timing_stats.loc[correct_key, 'mean']
    correct_rank = (timing_stats.index.tolist().index(correct_key) + 1)
    
    print(f"\n{'='*60}")
    print(f"CORRECT KEY BYTE: 0x{correct_key:02x}")
    print(f"Average timing: {correct_timing:.6f} us")
    print(f"Rank (by speed): {correct_rank}/256")
    print(f"{'='*60}\n")
    
    return timing_stats, correct_key

def predict_key_byte(features_df, results):
    """Predict the most likely key byte using ML models"""
    print("\n" + "="*60)
    print("ML MODEL PREDICTIONS")
    print("="*60)
    
    correct_key = features_df[features_df['is_correct'] == 1]['key_byte_guess'].iloc[0]
    
    for name, result in results.items():
        model = result['model']
        scaler = result['scaler']
        
        # Prepare features for prediction
        X = features_df.drop(['key_byte_guess', 'is_correct'], axis=1)
        X_scaled = scaler.transform(X)
        
        # Get prediction probabilities
        if hasattr(model, 'predict_proba'):
            probs = model.predict_proba(X_scaled)[:, 1]  # Probability of being correct
            features_df[f'{name}_prob'] = probs
            
            # Get top 5 predictions
            top_indices = np.argsort(probs)[-5:][::-1]
            
            print(f"\n{name} - Top 5 predictions:")
            for i, idx in enumerate(top_indices, 1):
                key_guess = features_df.iloc[idx]['key_byte_guess']
                prob = probs[idx]
                is_correct = "*** CORRECT ***" if key_guess == correct_key else ""
                print(f"  {i}. Key byte: 0x{int(key_guess):02x}, Confidence: {prob:.4f} {is_correct}")
    
    return features_df

def visualize_results(df, features_df):
    """Create visualizations of timing patterns"""
    print("\nGenerating visualizations...")
    
    fig, axes = plt.subplots(2, 2, figsize=(15, 12))
    
    # 1. Timing distribution for all key guesses
    ax1 = axes[0, 0]
    correct_key = df[df['is_correct'] == 1]['key_byte_guess'].iloc[0]
    
    timing_by_key = df.groupby('key_byte_guess')['timing_us'].mean().sort_values()
    colors = ['red' if k == correct_key else 'blue' for k in timing_by_key.index]
    
    ax1.bar(range(len(timing_by_key)), timing_by_key.values, color=colors, alpha=0.6)
    ax1.set_xlabel('Key Byte Guess (sorted by timing)')
    ax1.set_ylabel('Average Timing (us)')
    ax1.set_title('Average Encryption Timing for Each Key Byte Guess\n(Red = Correct Key)')
    ax1.grid(True, alpha=0.3)
    
    # 2. Timing distribution comparison: correct vs incorrect
    ax2 = axes[0, 1]
    correct_timings = df[df['is_correct'] == 1]['timing_us']
    incorrect_timings = df[df['is_correct'] == 0]['timing_us']
    
    ax2.hist(correct_timings, bins=50, alpha=0.7, label='Correct Key', color='green')
    ax2.hist(incorrect_timings, bins=50, alpha=0.5, label='Incorrect Keys', color='red')
    ax2.set_xlabel('Timing (us)')
    ax2.set_ylabel('Frequency')
    ax2.set_title('Timing Distribution: Correct vs Incorrect Key')
    ax2.legend()
    ax2.grid(True, alpha=0.3)
    
    # 3. Feature importance (mean timing vs std timing)
    ax3 = axes[1, 0]
    correct_features = features_df[features_df['is_correct'] == 1]
    incorrect_features = features_df[features_df['is_correct'] == 0]
    
    ax3.scatter(incorrect_features['mean_timing'], incorrect_features['std_timing'], 
                alpha=0.5, s=20, label='Incorrect Keys', color='blue')
    ax3.scatter(correct_features['mean_timing'], correct_features['std_timing'], 
                alpha=1.0, s=200, marker='*', label='Correct Key', color='red')
    ax3.set_xlabel('Mean Timing (us)')
    ax3.set_ylabel('Std Timing (us)')
    ax3.set_title('Timing Features: Mean vs Standard Deviation')
    ax3.legend()
    ax3.grid(True, alpha=0.3)
    
    # 4. Timing rank analysis
    ax4 = axes[1, 1]
    timing_sorted = features_df.sort_values('mean_timing')
    timing_sorted['rank'] = range(len(timing_sorted))
    
    colors_rank = ['red' if row['is_correct'] == 1 else 'blue' 
                   for _, row in timing_sorted.iterrows()]
    
    ax4.scatter(timing_sorted['rank'], timing_sorted['mean_timing'], 
                c=colors_rank, alpha=0.6, s=30)
    ax4.set_xlabel('Rank (by timing)')
    ax4.set_ylabel('Mean Timing (us)')
    ax4.set_title('Key Byte Candidates Ranked by Timing\n(Red = Correct Key)')
    ax4.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('timing_analysis.png', dpi=150)
    print("Visualization saved to timing_analysis.png")
    
    plt.show()

def main():
    if len(sys.argv) < 2:
        print("Usage: python3 ml_key_recovery.py <timing_data.csv>")
        sys.exit(1)
    
    data_file = sys.argv[1]
    
    print("="*60)
    print("ML-BASED AES KEY RECOVERY USING TIMING ANALYSIS")
    print("="*60)
    
    # Load data
    df = load_data(data_file)
    
    # Analyze timing patterns
    timing_stats, correct_key = analyze_timing_patterns(df)
    
    # Extract features
    features_df = extract_features(df)
    
    # Prepare ML training data
    X = features_df.drop(['key_byte_guess', 'is_correct'], axis=1)
    y = features_df['is_correct']
    
    # Split data
    X_train, X_test, y_train, y_test = train_test_split(
        X, y, test_size=0.3, random_state=42, stratify=y
    )
    
    # Train models
    results = train_model(X_train, X_test, y_train, y_test)
    
    # Predict key byte
    features_df = predict_key_byte(features_df, results)
    
    # Visualize
    visualize_results(df, features_df)
    
    print("\n" + "="*60)
    print("ANALYSIS COMPLETE")
    print("="*60)
    print(f"\nThe correct key byte is: 0x{correct_key:02x}")
    print("\nKey insights:")
    print("1. Timing differences reveal the secret key")
    print("2. ML models can amplify subtle timing signals")
    print("3. Statistical features improve detection accuracy")
    print("4. This demonstrates why constant-time crypto is critical")

if __name__ == "__main__":
    main()
