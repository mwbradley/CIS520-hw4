import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("results.csv")

df_avg = df.groupby(["input_size", "threads", "mem"], as_index=False)["time"].mean()

mem_levels = ["1G", "3G", "1500M", "512M"]
input_sizes = [1000, 10000, 100000, 1200000]

for size in input_sizes:
    for mem in mem_levels:
        df_filtered = df_avg[(df_avg["mem"] == mem) & (df_avg["input_size"] == size)]

        if df_filtered.empty:
            print(f"No data for Size: {size}, Mem: {mem}")
            continue
 
        df_filtered = df_filtered.sort_values("threads")

        threads = df_filtered["threads"]
        time = df_filtered["time"]

        baseline = time.iloc[0]
        speedup = baseline / time

        plt.plot(threads, speedup, marker='o')
        plt.xlabel("Threads")
        plt.ylabel("Speedup")
        plt.title(f"Pthread Speedup ({mem} memory, {size} lines)")

        plt.savefig(f"speedup_pthread_size{size}_{mem}.png")
        plt.clf()