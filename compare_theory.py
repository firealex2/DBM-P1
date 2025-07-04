import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import glob
import sys

#Find all CSVs matching our pattern
csv_files = glob.glob("results_*_e*.csv")
if not csv_files:
    print("Error: No CSV files matching 'results_*_e*.csv' found.", file=sys.stderr)
    sys.exit(1)

#Load and concatenate
df = pd.concat((pd.read_csv(f) for f in csv_files), ignore_index=True)

#Compute m ≈ p * n^2
df['m'] = df['p'] * (df['n'] ** 2)

#Theoretical bounds
df['theo_phases'] = np.log(df['n']) / (df['epsilon'] ** 2)
df['theo_time']   = df['m'] * (np.log(df['n']) ** 2) / (df['epsilon'] ** 3)

#For each epsilon, make its own zoomed-in plots
for eps in sorted(df['epsilon'].unique()):
    sub = df[df['epsilon'] == eps]

    #phases vs theory
    plt.figure(figsize=(6,4))
    plt.scatter(sub['theo_phases'], sub['phases'], alpha=0.7, label=f"ε={eps}")
    mn = 0
    mx = max(sub['theo_phases'].max(), sub['phases'].max()) * 1.1
    plt.plot([mn, mx], [mn, mx], 'r--', label='y = x')
    plt.xlim(0, mx)
    plt.ylim(0, mx)
    plt.xlabel("Theoretical #Phases ($\ln n / \epsilon^2$)")
    plt.ylabel("Observed #Phases")
    plt.title(f"Phases vs. Theory (ε={eps})")
    plt.legend()
    plt.tight_layout()
    plt.savefig(f"phases_vs_theoretical_e{eps}.png")
    plt.close()

    #runtime vs theory
    plt.figure(figsize=(6,4))
    plt.scatter(sub['theo_time'], sub['time_ms'], alpha=0.7, label=f"ε={eps}")
    mn = 0
    mx = max(sub['theo_time'].max(), sub['time_ms'].max()) * 1.1
    plt.plot([mn, mx], [mn, mx], 'r--', label='y = x')
    plt.xscale('log')
    plt.yscale('log')
    plt.xlim(1e-1, mx) 
    plt.ylim(1e-1, mx)
    plt.xlabel("Theoretical Runtime ($m\,\ln^2 n / \epsilon^3$)")
    plt.ylabel("Observed Runtime (ms)")
    plt.title(f"Runtime vs. Theory (ε={eps})")
    plt.legend()
    plt.tight_layout()
    plt.savefig(f"runtime_vs_theoretical_e{eps}.png")
    plt.close()

print("Generated per-ε zoomed plots:")
print("  - phases_vs_theoretical_e{ε}.png")
print("  - runtime_vs_theoretical_e{ε}.png")  