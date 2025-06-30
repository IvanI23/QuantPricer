import pandas as pd
import matplotlib.pyplot as plt

# Read results.csv
cols = ["S0", "K", "T", "r", "sigma", "paths", "MC", "BS", "AbsError"]
df = pd.read_csv('data/results.csv', header=None, names=cols)

# Plot 1: Error vs Number of Paths (log scale)
plt.figure(figsize=(10, 6))
plt.plot(df["paths"], df["AbsError"], marker='o')
plt.xlabel("Number of Paths")
plt.ylabel("Absolute Error")
plt.title("Monte Carlo Error vs Black-Scholes")
plt.grid(True)
plt.xscale("log")
plt.tight_layout()
plt.show()

# Plot 2: MC Price vs BS Price as Volatility changes (if sigma varies)
if df["sigma"].nunique() > 1:
    plt.figure(figsize=(10, 6))
    for k, group in df.groupby("paths"):
        plt.plot(group["sigma"], group["MC"], label=f"MC (paths={k})", marker='o')
        plt.plot(group["sigma"], group["BS"], label=f"BS (paths={k})", linestyle='--', marker='x')
    plt.xlabel("Volatility (σ)")
    plt.ylabel("Option Price")
    plt.title("MC vs Black-Scholes Price vs Volatility")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.show()

# Plot 3: MC Price vs BS Price as T changes (if T varies)
if df["T"].nunique() > 1:
    plt.figure(figsize=(10, 6))
    for k, group in df.groupby("paths"):
        plt.plot(group["T"], group["MC"], label=f"MC (paths={k})", marker='o')
        plt.plot(group["T"], group["BS"], label=f"BS (paths={k})", linestyle='--', marker='x')
    plt.xlabel("Time to Maturity (T)")
    plt.ylabel("Option Price")
    plt.title("MC vs Black-Scholes Price vs Time to Maturity")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.show() 