# QuantPricer: Monte Carlo Option Pricing Suite

This project implements a robust, modular C++ Monte Carlo simulation framework for pricing a variety of financial options, with a modern Python/Flask UI for visualization and user interaction, plus comprehensive backtesting capabilities.

---

## Features

- **Option Styles Supported:**
  - European (vanilla, Asian, barrier, binary)
  - American (vanilla, with true early exercise via LSM)

- **Payoff Types Supported:**
  - Vanilla (standard call/put)
  - Asian (average price)
  - Barrier (knock-in/knock-out)
  - Binary (fixed payout)

- **Dividends:**
  - Supports discrete dividends (e.g., `30:1.5,90:2.0` for $1.5 at step 30, $2.0 at step 90)
  - Black-Scholes pricing for European options automatically adjusts for discrete dividends (present value adjustment)

- **Variance Reduction:**
  - Antithetic variates
  - Control variate (Black-Scholes as control for European options)

- **Pricing Methods:**
  - Monte Carlo simulation (modular, supports all option types)
  - Black-Scholes analytical pricing (for European vanilla, with or without discrete dividends)
  - Least Squares Monte Carlo (LSM) for American options

- **Simulation:**
  - Geometric Brownian Motion (GBM) for asset price paths

- **Backtesting:**
  - Historical data integration using Yahoo Finance
  - Rolling window analysis over real market data
  - Performance comparison between pricing methods
  - Automated backtest execution and result visualization

- **Utilities:**
  - CSV export of simulation results and sample paths
  - Python plotting scripts for path visualization and result analysis
  - Flask web interface for interactive pricing and visualization
  - Comprehensive backtest analysis and reporting

---

## Getting Started

### Prerequisites

- **C++ Compiler** (C++11 or later)
- **CMake** and **Make**
- **Python 3.x** (for plotting, web interface, and backtesting)
- **pip** (for Python dependencies)

### Build and Run (C++)

```sh
make all
make run
# or with CMake:
cmake -S . -B build
cmake --build build
./build/bin/pricer --S0 100 --K 100 --T 1 --r 0.05 --sigma 0.2 --paths 100000 --verbose
```

### Python Visualization and Web Interface

```sh
pip install -r requirements.txt
python python/plot/plot_paths.py
python python/plot/plot_results.py
python python/app.py
# Then open http://127.0.0.1:5000 in your browser
```

### Backtesting

```sh
# Run backtest with default parameters (AAPL, K=150, 30 days, sigma=0.3)
python python/backtest/backtest.py

# Custom backtest parameters
python python/backtest/backtest.py --ticker MSFT --K 300 --T_days 60 --sigma 0.25
```

---

## Usage

### CLI Options

- `--S0` (initial stock price)
- `--K` (strike price)
- `--T` (time to maturity in years)
- `--r` (risk-free rate)
- `--sigma` (volatility)
- `--paths` (number of Monte Carlo paths)
- `--steps` (number of time steps)
- `--option-type` (`european` or `american`)
- `--side` (`call` or `put`)
- `--complexity` (`vanilla`, `asian`, `barrier`, `binary`)
- `--barrier` (barrier level for barrier options)
- `--barrier-type` (barrier type for barrier options)
- `--payout` (fixed payout for binary options)
- `--dividends` (e.g., `30:1.5,90:2.0`)
- `--variates` (`none`, `antithetic`, `control`)
- `--verbose`

### Output

- **MC Price**: Monte Carlo price for the selected option
- **BS Price**: Black-Scholes price (for European vanilla, with discrete dividend adjustment if needed)
- **Difference**: Absolute difference between MC and BS prices
- **CSV Export**: Results and sample paths are saved to `data/results.csv` and `data/sample_paths.csv`

### Python/Flask Web UI

- Interactive form for all parameters
- Real-time log output and results
- Visualization of simulated paths and Black-Scholes reference
- Supports all option types, dividends, and variance reduction

### Backtesting Features

- **Historical Data**: Uses Yahoo Finance for real market data
- **Rolling Analysis**: Tests pricing accuracy across different market conditions
- **Performance Metrics**: Compares Monte Carlo vs Black-Scholes vs intrinsic values
- **Visualization**: Generates comprehensive plots of pricing performance over time
- **Export**: Saves backtest results and statistics to CSV files

---

## Dependencies

### Python Dependencies (requirements.txt)
- `pandas` - Data manipulation and analysis
- `matplotlib` - Plotting and visualization
- `Flask` - Web interface framework
- `numpy` - Numerical computations
- `yfinance` - Yahoo Finance data access (for backtesting)

---

## Customization

- Add new option types by extending `include/options/` and `src/options/`
- Add new pricing methods in `include/pricing/` and `src/pricing/`
- Extend the web UI in `python/app.py` and `python/templates/index.html`
- Modify backtesting parameters in `python/backtest/backtest.py`

---

## License

MIT License



