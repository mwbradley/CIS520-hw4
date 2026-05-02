import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("results_openmp.csv")

df_avg = df.groupby(["threads", "mem"], as_index=False)["time"].mean()

mem_levels = ["1G", "3G", "1500M", "512M", "128M", "64M"]

for mem in mem_levels:
    df_m = df_avg[df_avg["mem"] == mem]

    if df_m.empty:
        print(f"No data for {mem}")
        continue
 
    df_m = df_m.sort_values("threads")

    threads = df_m["threads"]
    time = df_m["time"]

    baseline = time.iloc[0]
    speedup = baseline / time

    plt.plot(threads, speedup, marker='o')
    plt.xlabel("Threads")
    plt.ylabel("Speedup")
    plt.title(f"OpenMP Speedup ({mem} memory)")

    plt.savefig(f"speedup_openmp_{mem}.png")
    plt.clf()