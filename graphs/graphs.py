import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path

GRAPH_DIR = Path(__file__).resolve().parent
RESULTS = GRAPH_DIR.parent / "results.csv"
df = pd.read_csv(RESULTS)

df = df.sort_values(['method', 'N', 'p']).reset_index(drop=True)
df['Ts'] = df.groupby(['method', 'N'])['time'].transform('first')
df['speedup'] = df['Ts'] / df['time']
df['efficiency'] = df['speedup'] / df['p']

p_vals = sorted(df['p'].unique())

fig, axes = plt.subplots(2, 2, figsize=(12, 9))

methods = [
    ('naive', 'Naive Communication'),
    ('odd_even', 'Optimized Communication (Odd-Even)')
]

for row, (method, title_name) in enumerate(methods):
    subset = df[df['method'] == method]
    ax_speedup, ax_eff = axes[row]

    for n, group in subset.groupby('N'):
        ax_speedup.plot(group['p'], group['speedup'], marker='o', label=f"N={n}")
        ax_eff.plot(group['p'], group['efficiency'], marker='o', label=f"N={n}")

    ax_speedup.plot(p_vals, p_vals, 'k--', label='Ideal')
    ax_eff.axhline(1.0, color='k', linestyle='--', label='Ideal')

    ax_speedup.set_title(f'Speedup - {title_name}')
    ax_speedup.set_xlabel('p')
    ax_speedup.set_ylabel('Speedup')
    ax_speedup.grid(True)
    ax_speedup.legend()

    ax_eff.set_title(f'Efficiency - {title_name}')
    ax_eff.set_xlabel('p')
    ax_eff.set_ylabel('Efficiency')
    ax_eff.grid(True)
    ax_eff.legend()

plt.tight_layout()
plt.show()
