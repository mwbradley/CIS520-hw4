import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("results.csv")

mem_levels = ["1G", "3G", "1500M", "512M", "128M", "64M"]

for mem in mem_levels:
    df_m = df[df["mem"] == mem]

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
    plt.title(f"Pthread Speedup ({mem} memory)")

    plt.savefig(f"speedup_{mem}.png")
    plt.clf()