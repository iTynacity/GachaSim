import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from scipy.signal import find_peaks
import argparse

def plot_target_souls(filename="pull_results.csv"):
    """Plots the histogram for the 'Target Souls' simulation."""
    try:
        data = pd.read_csv(filename)
    except FileNotFoundError:
        print(f"Error: '{filename}' not found. Run the C++ simulation first.")
        return

    pulls_results = data['result']
    
    # Calculate spikes
    counts, bins = np.histogram(pulls_results, bins=50)
    bin_centers = (bins[:-1] + bins[1:]) / 2
    peaks, _ = find_peaks(counts, prominence=np.max(counts)*0.05) # Prominence relative to max height
    top_peaks_indices = sorted(peaks, key=lambda p: counts[p], reverse=True)[:3]
    spike_data = [(int(bin_centers[p]), counts[p]) for p in top_peaks_indices]

    # Plotting
    plt.style.use('seaborn-v0_8-whitegrid')
    plt.figure(figsize=(12, 7))
    plt.hist(pulls_results, bins=50, alpha=0.75, edgecolor='black', label='Frequency')

    max_height = max(counts) if len(counts) > 0 else 1

    for i, (x, _) in enumerate(spike_data):
        label = f"Spike {i+1}: {x} Pulls"
        plt.axvline(x, linestyle='--', color='purple', linewidth=2, label=label)
        plt.text(x + 5, max_height * 0.8, f"{x} Pulls", rotation=90, color='darkred', ha='center', va='center', fontsize=10)

    plt.title("Distribution of Pulls Needed (from C++ Simulation)")
    plt.xlabel("Number of Pulls")
    plt.ylabel("Frequency")
    plt.legend()
    plt.tight_layout()
    plt.show()

def plot_fixed_pulls(filename="soul_results.csv"):
    """Plots the histogram for the 'Fixed Pulls' simulation."""
    try:
        data = pd.read_csv(filename)
    except FileNotFoundError:
        print(f"Error: '{filename}' not found. Run the C++ simulation first.")
        return

    results = data['result']
    max_souls = results.max()

    plt.style.use('seaborn-v0_8-whitegrid')
    plt.figure(figsize=(12, 7))
    # Bins are set to align with integer soul counts
    bins = np.arange(0, max_souls + 2) - 0.5
    plt.hist(results, bins=bins, alpha=0.75, edgecolor='black')
    
    plt.title("Distribution of Souls Obtained (from C++ Simulation)")
    plt.xlabel("Souls Obtained")
    plt.ylabel("Frequency")
    plt.xticks(np.arange(0, max_souls + 1, step=max(1, max_souls // 15))) # Dynamic ticks
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Visualize simulation results from the C++ program.")
    parser.add_argument('mode', type=int, choices=[1, 2], help="Simulation mode to visualize (1: Target Souls, 2: Fixed Pulls)")
    args = parser.parse_args()

    if args.mode == 1:
        plot_target_souls()
    elif args.mode == 2:
        plot_fixed_pulls()