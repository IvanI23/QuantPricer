import os
import subprocess
import pandas as pd
from flask import Flask, render_template, request, send_from_directory, url_for, Response, stream_with_context, jsonify
import matplotlib
import json
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy as np

app = Flask(__name__, template_folder='templates', static_folder='static')

BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DATA_DIR = os.path.join(BASE_DIR, 'data')
if os.name == 'nt':
    PRICER_EXECUTABLE = os.path.join(BASE_DIR, 'build', 'bin', 'pricer.exe')
else:
    PRICER_EXECUTABLE = os.path.join(BASE_DIR, 'build_linux', 'bin', 'pricer')
RESULTS_CSV = os.path.join(DATA_DIR, 'results.csv')
PATHS_CSV = os.path.join(DATA_DIR, 'sample_paths.csv')
PLOT_PY = os.path.join(BASE_DIR, 'plot_paths.py')
STATIC_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'static')

os.makedirs(DATA_DIR, exist_ok=True)

@app.route('/health')
def health_check():
    """Health check endpoint for Render"""
    return jsonify({"status": "healthy", "message": "QuantPricer is running"})

@app.route('/', methods=['GET'])
def index():
    inputs = {
        'S0': '100',
        'K': '100',
        'T': '1.0',
        'r': '0.05',
        'sigma': '0.2',
        'paths': '100000',
        'steps': '365',
        'option-type': 'european',
        'side': 'call',
        'complexity': 'vanilla',
        'barrier-type': '',
        'barrier': '0',
        'payoff': 'vanilla',
        'payout': '1.0',
        'dividends': '',
        'variates': ''
    }
    return render_template('index.html', inputs=inputs)

@app.route('/run', methods=['POST'])
def run_simulation():
    if not os.path.exists(PRICER_EXECUTABLE):
        def error_generator():
            yield f"ERROR:::Pricer executable not found at {PRICER_EXECUTABLE}\n"
            yield "Please ensure the C++ application is built before running the web interface.\n"
        return Response(stream_with_context(error_generator()), mimetype='text/plain')

    inputs = {k: request.form.get(k, '') for k in [
        'S0', 'K', 'T', 'r', 'sigma', 'paths', 'steps',
        'option-type', 'side', 'complexity', 'barrier-type',
        'barrier', 'payoff', 'payout', 'dividends', 'variates']}

    if os.path.exists(RESULTS_CSV):
        os.remove(RESULTS_CSV)
    if os.path.exists(PATHS_CSV):
        os.remove(PATHS_CSV)

    cmd = [
        PRICER_EXECUTABLE,
        '--S0', inputs['S0'],
        '--K', inputs['K'],
        '--T', inputs['T'],
        '--r', inputs['r'],
        '--sigma', inputs['sigma'],
        '--paths', inputs['paths'],
        '--steps', inputs['steps'],
        '--option-type', inputs['option-type'],
        '--side', inputs['side'],
        '--complexity', inputs['complexity'],
        '--verbose'
    ]

    if inputs['complexity'] == 'barrier':
        cmd.extend([
            '--barrier-type', inputs['barrier-type'],
            '--barrier', inputs['barrier']
        ])
    if inputs['complexity'] == 'binary':
        cmd.extend([
            '--payout', inputs['payout']
        ])
    if inputs['dividends']:
        cmd.extend(['--dividends', inputs['dividends']])
    if inputs['variates'] == 'antithetic':
        cmd.extend(['--variates', 'antithetic'])
    elif inputs['variates'] == 'control':
        cmd.extend(['--variates', 'control'])

    def generate():
        try:
            process = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, bufsize=1, universal_newlines=True)
            
            for line in process.stdout:
                yield line

            process.wait()
            
            if process.returncode != 0:
                yield f"Error: Failed to run simulation. Return code: {process.returncode}"
                return

            if not os.path.exists(RESULTS_CSV):
                yield "Error: Results file was not created by the simulation."
                return

            col_names = ['S0', 'K', 'T', 'r', 'sigma', 'paths', 'MonteCarlo_Price', 'BlackScholes_Price', 'Difference']
            results_df = pd.read_csv(RESULTS_CSV, names=col_names, header=None)
            latest_result = results_df.iloc[-1]
            results = {
                'mc_price': f"{latest_result['MonteCarlo_Price']:.4f}",
                'bs_price': f"{latest_result['BlackScholes_Price']:.4f}",
                'diff': f"{latest_result['Difference']:.4f}"
            }

            plot_url = generate_plot(S0=float(inputs['S0']), r=float(inputs['r']), sigma=float(inputs['sigma']), T=float(inputs['T']), steps=int(inputs['steps']))
            
            result_payload = json.dumps({'results': results, 'plot_url': plot_url})
            yield f"RESULTS:::{result_payload}"

        except FileNotFoundError:
             yield f"ERROR:::Pricer executable not found at {PRICER_EXECUTABLE}"
        except Exception as e:
            yield f"ERROR:::{str(e)}"

    return Response(stream_with_context(generate()), mimetype='text/plain')


def generate_plot(S0=None, r=None, sigma=None, T=None, steps=None):
    try:
        if not os.path.exists(PATHS_CSV):
            print("Paths CSV file not found, skipping plot generation")
            return None
            
        paths_df = pd.read_csv(PATHS_CSV)
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
        plot_path = os.path.join(STATIC_DIR, 'plot.png')
        plt.savefig(plot_path)
        plt.close()
        return url_for('static', filename='plot.png', t=os.path.getmtime(plot_path))
    except Exception as e:
        print(f"Error generating plot: {e}")
        return None

if __name__ == '__main__':
    port = int(os.environ.get("PORT", 5000))  
    app.run(host="0.0.0.0", port=port, debug=False) 