import yfinance as yf 
import subprocess
import pandas as pd
import matplotlib.pyplot as plt
import argparse
import os
import numpy as np
from pathlib import Path

SCRIPT_DIR = Path(__file__).parent.parent.parent.resolve()
DATA_DIR = SCRIPT_DIR / "data"
OUTPUT_DIR = SCRIPT_DIR / "python/backtest/result"
PRICER_EXE = SCRIPT_DIR / "build/bin/pricer.exe"

os.makedirs(DATA_DIR, exist_ok=True)
os.makedirs(OUTPUT_DIR, exist_ok=True)

def run_cpp_pricer(S0: float, K: float, T: float, r: float, sigma: float, paths: int = 1000):
    """Run the C++ pricing engine and return prices."""
    if not PRICER_EXE.exists():
        print(f"Error: Pricer executable not found at {PRICER_EXE}")
        return None, None

    cmd = [
        str(PRICER_EXE),
        "--type", "european",
        "--strike", str(K),
        "--maturity", str(T),
        "--S0", str(S0),
        "--r", str(r),
        "--sigma", str(sigma),
        "--paths", str(paths)
    ]

    try:
        subprocess.run(cmd, cwd=SCRIPT_DIR, check=True, capture_output=True)
        file_path = DATA_DIR / "results.csv"
        
        if file_path.exists():
            df = pd.read_csv(file_path, header=None)
            if not df.empty:
                return df.iloc[-1][6], df.iloc[-1][7]  # MC and BS prices
    except Exception as e:
        print(f"Error running pricer: {e}")
    
    return None, None

def backtest(ticker: str, K: float, T_days: int, sigma: float, r: float = 0.01):
    T_years = T_days / 252.0
    
    print(f"Starting backtest for {ticker}")
    print(f"   Strike: ${K:.2f}, Maturity: {T_days} days, Volatility: {sigma:.1%}")
    print("-" * 60)
    
    try:
        hist = yf.Ticker(ticker).history(period="1y")
        if hist.empty:
            print(f"No data available for {ticker}")
            return pd.DataFrame()
            
        results = []
        total_windows = len(hist) - T_days + 1
        
        for i in range(total_windows):
            window = hist.iloc[i:i+T_days]
            S0 = window["Close"].iloc[0]
            date = window.index[0].strftime('%Y-%m-%d')
            
            mc_price, bs_price = run_cpp_pricer(S0, K, T_years, r, sigma)
            
            if mc_price and bs_price:
                intrinsic = max(S0 - K, 0)
                results.append({
                    "Date": date,
                    "S0": S0,
                    "Real": intrinsic,
                    "MonteCarlo": mc_price,
                    "BlackScholes": bs_price
                })
                
                progress = (i + 1) / total_windows * 100
                print(f"[{progress:5.1f}%] {date}: S0=${S0:6.2f} | MC=${mc_price:6.2f} | BS=${bs_price:6.2f}")

        df = pd.DataFrame(results).set_index("Date")
        
        df.to_csv(OUTPUT_DIR / "backtest_results.csv")
        
        stats = pd.DataFrame({
            "Metric": ["Total Observations"],
            "Value": [len(df)]
        })
        stats.to_csv(OUTPUT_DIR / "backtest_stats.csv", index=False)
        
        print("-" * 60)
        print(f"Backtest completed: {len(df)} observations")

        return df

    except Exception as e:
        print(f"Backtest failed: {e}")
        return pd.DataFrame()

def plot_results(df: pd.DataFrame, ticker: str, K: float):
    plt.style.use('seaborn-v0_8')
    
    fig, ax = plt.subplots(1, 1, figsize=(14, 8))
    
    ax.plot(df.index, df["MonteCarlo"], label="Monte Carlo", linewidth=2, color='#2E86AB')
    ax.plot(df.index, df["BlackScholes"], label="Black-Scholes", linewidth=2, color='#A23B72')
    ax.plot(df.index, df["Real"], label="Intrinsic Value", linestyle='--', linewidth=2, color='#F18F01')
    
    ax.set_title(f"Option Pricing Comparison: {ticker} (K=${K:.2f})", 
                  fontsize=16, fontweight='bold', pad=20)
    ax.set_ylabel("Option Value ($)", fontsize=12)
    ax.set_xlabel("Date", fontsize=12)
    ax.legend(fontsize=11, framealpha=0.9)
    ax.grid(True, alpha=0.3)
    
    n_dates = len(df.index)
    if n_dates > 20:
        step = max(1, n_dates // 10)  
        ax.set_xticks(range(0, n_dates, step))
        ax.set_xticklabels([df.index[i] for i in range(0, n_dates, step)], rotation=45)
    else:
        ax.tick_params(axis='x', rotation=45)
    
    plt.tight_layout()
    plt.savefig(OUTPUT_DIR / "backtest_plot.png", dpi=300, bbox_inches='tight')
    plt.show()

def main():
    parser = argparse.ArgumentParser(description="Option Pricing Backtester")
    parser.add_argument("--ticker", default="AAPL", help="Stock ticker")
    parser.add_argument("--K", type=float, default=150.0, help="Strike price")
    parser.add_argument("--T_days", type=int, default=30, help="Days to maturity")
    parser.add_argument("--sigma", type=float, default=0.3, help="Volatility")
    args = parser.parse_args()

    print("=" * 60)
    print("QUANT PRICER - OPTION BACKTESTING TOOL")
    print("=" * 60)
    
    df = backtest(args.ticker, args.K, args.T_days, args.sigma)
    
    if not df.empty:
        print("\nGenerating visualization...")
        plot_results(df, args.ticker, args.K)
        
        print("\nResults saved to:")
        print(f"   Pricing data: {OUTPUT_DIR / 'backtest_results.csv'}")
        print(f"   Statistics: {OUTPUT_DIR / 'backtest_stats.csv'}")
        print(f"   Plot: {OUTPUT_DIR / 'backtest_plot.png'}")
        print("\nBacktest completed successfully!")

if __name__ == "__main__":
    main() 