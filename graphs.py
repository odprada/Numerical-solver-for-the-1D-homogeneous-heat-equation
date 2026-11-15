import pandas as pd
from pathlib import Path

RESULTS = Path(__file__).resolve().parent / "results.csv"
all_data = pd.read_csv(RESULTS)

naive_comm = all_data.loc[all_data['method']=='naive', ['N', 'p', 'time']]
opt_comm = all_data.loc[all_data['method']=='odd_even', ['N', 'p', 'time']]

print(naive_comm)