import subprocess
import pandas as pd
import os

def run_SuRFReal_bench(dist, workload_dir, u, n):
    cmd = ["../build/bench/integer_range_filtering_bench", str(dist), str(workload_dir), "surf", str(u), str(n)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

def run_Rosetta_bench(dist, workload_dir, RST_size_min, RST_size_max, RST_size_step, u, n):
    cmd = ["../build/bench/integer_range_filtering_bench", str(dist), str(workload_dir), "rosetta", str(u), str(n), str(RST_size_min), str(RST_size_max), str(RST_size_step)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

def run_EmptyRanges_bench(dist, workload_dir, u, n):
    cmd = ["../build/bench/integer_range_filtering_bench", str(dist), str(workload_dir), "emptyranges", str(u), str(n)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

distributions=["uniform", "normal", "multiple_uniform", "multiple_normal"]
ns = ["50", "200", "500"]
dir="1e4"
results_dir = "integer-range-filtering-results/1e4"
workload_dir = "workload-gen/integer_workloads/%s/" % dir

if not os.path.exists(results_dir):
    os.makedirs(results_dir)

RST_size_min=2000
RST_size_max=10000
RST_size_step=2000
u=1000
for n in ns:
    for dist in distributions:
        print("Running bench for distribution = %s" % (dist))
        df_surf = run_SuRFReal_bench(dist, workload_dir, u, n)
        df_rosetta = run_Rosetta_bench(dist, workload_dir, RST_size_min, RST_size_max, RST_size_step, u, n)
        df_empty_ranges = run_EmptyRanges_bench(dist, workload_dir, u, n)
        df_sorted_arr = pd.DataFrame(data={'Memory usage': [int(n)*4], 'Sorted array': [0.0]})

        print(df_surf)
        print(df_rosetta)
        print(df_empty_ranges)

        df_surf.rename(columns={'FPR': 'SuRF Real'}, inplace=True)
        df_rosetta.rename(columns={'FPR': 'Rosetta'}, inplace=True)
        df_empty_ranges.rename(columns={'FPR': 'Empty Ranges'}, inplace=True)

        print("Creating DF with stats")
        dfs = [df_rosetta[['Memory usage', 'Rosetta']],
                df_surf[['Memory usage', 'SuRF Real']],
                df_empty_ranges[['Memory usage', 'Empty Ranges']],
                df_sorted_arr[['Memory usage', 'Sorted array']]]

        df = pd.concat(dfs)

        with open(results_dir + "/" + dist + "_" + n + ".txt", "w") as f:
            df.to_csv(f)

