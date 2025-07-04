import pandas as pd
import matplotlib.pyplot as plt
import glob


df = pd.concat(
    [pd.read_csv(f) for f in glob.glob("results_*_e0.5.csv")],
    ignore_index=True
)

for metric in ["time_ms", "phases", "doublings"]:
    plt.figure()
    for p, sub in df.groupby("p"):
        sub = sub.sort_values("n")
        y = sub[metric]
        x = sub["n"]
        plt.plot(x, y, marker='o', label=f"p={p}")
    if metric == "time_ms":
        plt.xscale("log"); plt.yscale("log")
    else:
        plt.xscale("log")
    plt.xlabel("n = |L|")
    plt.ylabel(metric.replace("_"," "))
    plt.title(f"{metric.replace('_',' ').title()} vs n (ε=0.5)")
    plt.legend()
    plt.tight_layout()
    plt.savefig(f"{metric}_vs_n_e0.5.png")