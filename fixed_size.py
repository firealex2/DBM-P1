import pandas as pd
import matplotlib.pyplot as plt
import glob

# load all CSVs
df_all = pd.concat(
    [pd.read_csv(f) for f in glob.glob("results_*.csv")],
    ignore_index=True
)

# pick n=512,p=0.2
sub = df_all[(df_all.n==1024)&(df_all.p==0.5)]

sub = sub.sort_values("epsilon")

for metric in ["time_ms", "phases", "doublings"]:
    plt.figure()
    plt.plot(sub["epsilon"], sub[metric], marker='o')
    plt.xlabel("ε")
    plt.ylabel(metric.replace("_"," "))
    plt.title(f"{metric.replace('_',' ').title()} vs ε (n=1024, p=0.5)")
    plt.tight_layout()
    plt.savefig(f"{metric}_vs_epsilon_n1024_p50.png")