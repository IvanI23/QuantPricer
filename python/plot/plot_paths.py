import argparse
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os

def plot_paths(S0=None, r=None, sigma=None, T=None, steps=None, csv_path='data/sample_paths.csv', out_path='python/static/plot.png'):
    paths_df = pd.read_csv(csv_path)
    plt.figure(figsize=(10, 6))
    num_paths_to_plot = min(5, len(paths_df.columns))
    for i in range(num_paths_to_plot):
        plt.plot(paths_df.iloc[:, i], lw=1, label=f'Path {i+1}' if i == 0 else None)
        plt.scatter([len(paths_df)-1], [paths_df.iloc[-1, i]], color='blue', s=30)

    if S0 is None:
        S0 = paths_df.iloc[0, 0]
    if steps is None:
        steps = len(paths_df) - 1
    if T is None:
        T = 1.0
    if r is None:
        r = 0.05
    if sigma is None:
        sigma = 0.2
    times = np.linspace(0, T, steps+1)
    bs_path = S0 * np.exp(r * times)
    plt.plot(bs_path, 'k--', lw=2, label='Black-Scholes Expected Path')
    plt.scatter([steps], [bs_path[-1]], color='red', s=50, label='BS End')

    plt.title('Monte Carlo Simulated Paths vs. Black-Scholes')
    plt.xlabel('Time Steps')
    plt.ylabel('Stock Price')
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(out_path)
    plt.close()

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Plot Monte Carlo simulated paths and Black-Scholes reference.')
    parser.add_argument('--S0', type=float, default=None, help='Initial stock price')
    parser.add_argument('--r', type=float, default=None, help='Risk-free rate')
    parser.add_argument('--sigma', type=float, default=None, help='Volatility')
    parser.add_argument('--T', type=float, default=None, help='Maturity')
    parser.add_argument('--steps', type=int, default=None, help='Number of time steps')
    parser.add_argument('--csv', type=str, default='data/paths.csv', help='CSV file with paths')
    parser.add_argument('--out', type=str, default='python/static/plot.png', help='Output plot path')
    args = parser.parse_args()
    plot_paths(args.S0, args.r, args.sigma, args.T, args.steps, args.csv, args.out) 