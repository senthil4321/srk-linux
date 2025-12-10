#!/usr/bin/env python3
"""
predict_with_model.py

Uses trained ML models to predict an UNKNOWN secret key from timing data.
This demonstrates the practical attack scenario:
1. Load pre-trained models (from training data)
2. Analyze timing data from new unknown key
3. Predict the unknown key byte

Usage:
    python predict_with_model.py unknown_key_data.csv
"""

import sys
import pandas as pd
import numpy as np
from sklearn.ensemble import RandomForestClassifier, GradientBoostingClassifier
from sklearn.preprocessing import StandardScaler
import matplotlib.pyplot as plt
import seaborn as sns

def extract_features(df):
    """Extract statistical features from timing data"""
    features = df.groupby('key_guess')['timing_us'].agg([
        'mean', 'std', 'min', 'max', 'median',
        ('q25', lambda x: x.quantile(0.25)),
        ('q75', lambda x: x.quantile(0.75)),
    ]).reset_index()
    
    features['range'] = features['max'] - features['min']
    features['is_correct'] = df.groupby('key_guess')['is_correct'].first().values
    
    return features

def train_models(training_data_file):
    """Train models on the original training data"""
    print("Loading training data...")
    df_train = pd.read_csv(training_data_file)
    
    # Extract features
    features_train = extract_features(df_train)
    X_train = features_train.drop(['key_guess', 'is_correct'], axis=1)
    y_train = features_train['is_correct']
    
    # Standardize features
    scaler = StandardScaler()
    X_train_scaled = scaler.fit_transform(X_train)
    
    print("Training Random Forest...")
    rf_model = RandomForestClassifier(n_estimators=100, random_state=42)
    rf_model.fit(X_train_scaled, y_train)
    
    print("Training Gradient Boosting...")
    gb_model = GradientBoostingClassifier(n_estimators=100, random_state=42)
    gb_model.fit(X_train_scaled, y_train)
    
    return rf_model, gb_model, scaler

def predict_unknown_key(rf_model, gb_model, scaler, unknown_data_file):
    """Predict the unknown key from timing data"""
    print("\n=== PREDICTING UNKNOWN KEY ===\n")
    
    # Load unknown key timing data
    print(f"Loading unknown key data from {unknown_data_file}...")
    df_unknown = pd.read_csv(unknown_data_file)
    print(f"Loaded {len(df_unknown):,} samples")
    
    # Extract features
    print("Extracting features...")
    features_unknown = extract_features(df_unknown)
    
    # Get the actual unknown key (for verification)
    actual_key = features_unknown[features_unknown['is_correct'] == 1]['key_guess'].values[0]
    print(f"\nActual unknown key byte: 0x{actual_key:02x}")
    
    # Prepare features for prediction
    X_unknown = features_unknown.drop(['key_guess', 'is_correct'], axis=1)
    X_unknown_scaled = scaler.transform(X_unknown)
    
    # Predict with both models
    print("\n" + "="*60)
    print("PREDICTION RESULTS")
    print("="*60)
    
    # Random Forest predictions
    rf_probs = rf_model.predict_proba(X_unknown_scaled)[:, 1]
    features_unknown['rf_probability'] = rf_probs
    rf_sorted = features_unknown.sort_values('rf_probability', ascending=False)
    
    print("\n--- Random Forest Predictions ---")
    print("Top 5 most likely key bytes:")
    for i, row in rf_sorted.head(5).iterrows():
        key_val = int(row['key_guess'])
        prob = row['rf_probability'] * 100
        is_correct = " *** CORRECT ***" if key_val == actual_key else ""
        print(f"  {i+1}. Key 0x{key_val:02x}: {prob:6.2f}% confidence{is_correct}")
    
    rf_rank = (rf_sorted.reset_index(drop=True)['key_guess'] == actual_key).idxmax() + 1
    print(f"\nActual key 0x{actual_key:02x} ranked: #{rf_rank}/256")
    
    # Gradient Boosting predictions
    gb_probs = gb_model.predict_proba(X_unknown_scaled)[:, 1]
    features_unknown['gb_probability'] = gb_probs
    gb_sorted = features_unknown.sort_values('gb_probability', ascending=False)
    
    print("\n--- Gradient Boosting Predictions ---")
    print("Top 5 most likely key bytes:")
    for i, row in gb_sorted.head(5).iterrows():
        key_val = int(row['key_guess'])
        prob = row['gb_probability'] * 100
        is_correct = " *** CORRECT ***" if key_val == actual_key else ""
        print(f"  {i+1}. Key 0x{key_val:02x}: {prob:6.2f}% confidence{is_correct}")
    
    gb_rank = (gb_sorted.reset_index(drop=True)['key_guess'] == actual_key).idxmax() + 1
    print(f"\nActual key 0x{actual_key:02x} ranked: #{gb_rank}/256")
    
    # Consensus prediction
    print("\n" + "="*60)
    print("CONSENSUS PREDICTION")
    print("="*60)
    
    features_unknown['avg_probability'] = (features_unknown['rf_probability'] + 
                                          features_unknown['gb_probability']) / 2
    consensus = features_unknown.sort_values('avg_probability', ascending=False)
    
    top_prediction = consensus.iloc[0]
    predicted_key = int(top_prediction['key_guess'])
    confidence = top_prediction['avg_probability'] * 100
    
    print(f"\nPredicted key byte: 0x{predicted_key:02x}")
    print(f"Average confidence: {confidence:.2f}%")
    print(f"Actual key byte: 0x{actual_key:02x}")
    print(f"Result: {'SUCCESS ✓' if predicted_key == actual_key else 'FAILED ✗'}")
    
    # Create visualization
    create_prediction_visualization(features_unknown, actual_key, predicted_key)
    
    return predicted_key == actual_key

def create_prediction_visualization(features, actual_key, predicted_key):
    """Create visualization of prediction results"""
    print("\nGenerating visualization...")
    
    fig, axes = plt.subplots(2, 2, figsize=(14, 10))
    fig.suptitle('Unknown Key Prediction Analysis', fontsize=16, fontweight='bold')
    
    # 1. Random Forest probabilities
    ax1 = axes[0, 0]
    key_values = features['key_guess'].values
    rf_probs = features['rf_probability'].values * 100
    
    colors = ['red' if k == actual_key else 'lightcoral' if k == predicted_key else 'gray' 
              for k in key_values]
    ax1.bar(key_values, rf_probs, color=colors, alpha=0.7, width=1.0)
    ax1.set_xlabel('Key Guess')
    ax1.set_ylabel('Probability (%)')
    ax1.set_title('Random Forest Predictions\n(Red=Actual, Light Red=Predicted)')
    ax1.grid(axis='y', alpha=0.3)
    
    # 2. Gradient Boosting probabilities
    ax2 = axes[0, 1]
    gb_probs = features['gb_probability'].values * 100
    
    ax2.bar(key_values, gb_probs, color=colors, alpha=0.7, width=1.0)
    ax2.set_xlabel('Key Guess')
    ax2.set_ylabel('Probability (%)')
    ax2.set_title('Gradient Boosting Predictions\n(Red=Actual, Light Red=Predicted)')
    ax2.grid(axis='y', alpha=0.3)
    
    # 3. Consensus ranking
    ax3 = axes[1, 0]
    avg_probs = features['avg_probability'].values * 100
    sorted_indices = np.argsort(avg_probs)[::-1]
    
    top_20_idx = sorted_indices[:20]
    top_20_keys = key_values[top_20_idx]
    top_20_probs = avg_probs[top_20_idx]
    
    colors_top20 = ['red' if k == actual_key else 'lightcoral' if k == predicted_key else 'steelblue' 
                    for k in top_20_keys]
    ax3.barh(range(20), top_20_probs, color=colors_top20, alpha=0.7)
    ax3.set_yticks(range(20))
    ax3.set_yticklabels([f'0x{k:02x}' for k in top_20_keys])
    ax3.set_xlabel('Average Probability (%)')
    ax3.set_ylabel('Key Byte')
    ax3.set_title('Top 20 Predictions (Average)\n(Red=Actual, Light Red=Predicted)')
    ax3.invert_yaxis()
    ax3.grid(axis='x', alpha=0.3)
    
    # 4. Model agreement scatter
    ax4 = axes[1, 1]
    rf_probs_full = features['rf_probability'].values * 100
    gb_probs_full = features['gb_probability'].values * 100
    
    # Plot all points
    ax4.scatter(rf_probs_full, gb_probs_full, alpha=0.3, s=20, c='gray', label='Other keys')
    
    # Highlight predicted key
    if predicted_key != actual_key:
        pred_idx = np.where(key_values == predicted_key)[0][0]
        ax4.scatter(rf_probs_full[pred_idx], gb_probs_full[pred_idx], 
                   s=200, c='lightcoral', marker='*', edgecolors='black', 
                   linewidth=2, label=f'Predicted (0x{predicted_key:02x})', zorder=5)
    
    # Highlight actual key
    actual_idx = np.where(key_values == actual_key)[0][0]
    ax4.scatter(rf_probs_full[actual_idx], gb_probs_full[actual_idx], 
               s=300, c='red', marker='*', edgecolors='black', 
               linewidth=2, label=f'Actual (0x{actual_key:02x})', zorder=6)
    
    ax4.plot([0, 100], [0, 100], 'k--', alpha=0.3, label='Perfect agreement')
    ax4.set_xlabel('Random Forest Probability (%)')
    ax4.set_ylabel('Gradient Boosting Probability (%)')
    ax4.set_title('Model Agreement Analysis')
    ax4.legend()
    ax4.grid(alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('unknown_key_prediction.png', dpi=150, bbox_inches='tight')
    print("Saved visualization: unknown_key_prediction.png")

def main():
    if len(sys.argv) != 2:
        print("Usage: python predict_with_model.py <unknown_key_data.csv>")
        sys.exit(1)
    
    unknown_data_file = sys.argv[1]
    training_data_file = "timing_data.csv"  # Original training data
    
    print("=== ML-Based Key Recovery - PREDICTION MODE ===\n")
    
    # Check if training data exists
    try:
        # Train models on original data
        rf_model, gb_model, scaler = train_models(training_data_file)
        
        # Predict unknown key
        success = predict_unknown_key(rf_model, gb_model, scaler, unknown_data_file)
        
        if success:
            print("\n" + "="*60)
            print("KEY RECOVERY SUCCESSFUL!")
            print("="*60)
            print("\nThe ML models successfully predicted the unknown key byte!")
        else:
            print("\n" + "="*60)
            print("KEY RECOVERY FAILED")
            print("="*60)
            print("\nThe ML models did not correctly predict the top key.")
            print("However, check the ranking - it may still be in the top candidates.")
        
    except FileNotFoundError as e:
        print(f"Error: {e}")
        print("\nMake sure you have the original training data:")
        print("  1. Run: make run (to generate timing_data.csv)")
        print("  2. Then run this prediction script")
        sys.exit(1)

if __name__ == '__main__':
    main()
