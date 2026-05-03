import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("results_mpi.csv")
df["time"] = pd.to_numeric(df["time"], errors="coerce")
df["ranks"] = pd.to_numeric(df["ranks"], errors="coerce")
df = df.dropna()

df = df[df["time"] < 0.5]

df_avg = df.groupby(["ranks", "nodes", "mem"], as_index=False)["time"].mean()

mem_levels = ["512M", "1G", "1500M", "3G"]

for mem in mem_levels:
    df_m = df_avg[df_avg["mem"] == mem].sort_values("ranks")

    if df_m.empty:
        print(f"No data for {mem}")
        continue

    for nodes, group in df_m.groupby("nodes"):
        group = group.sort_values("ranks")
        baseline = group["time"].iloc[0]
        speedup = baseline / group["time"]
        plt.plot(group["ranks"], speedup, marker='o', label=f"{nodes} node(s)")

    plt.xlabel("Ranks")
    plt.ylabel("Speedup")
    plt.title(f"MPI Speedup ({mem} memory)")
    plt.legend()
    plt.tight_layout()
    plt.savefig(f"speedup_mpi_{mem}.png")
    plt.clf()
    print(f"Saved speedup_mpi_{mem}.png")