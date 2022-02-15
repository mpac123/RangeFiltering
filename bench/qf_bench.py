import subprocess
import pandas as pd
import os

def qf_bench(q, min_r, max_r, min_n, max_n, interval_n):
    cmd = ["../build/bench/queryfilter", str(q), str(min_r), str(max_r), str(min_n), str(max_n), str(interval_n)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    print(p.stderr)
    return pd.read_csv(p.stdout, delimiter="\t")

def qf_n_vs_m(min_r, max_r, min_n, max_n, interval_n):
    cmd = ["../build/bench/qf_n_vs_m",str(min_r), str(max_r), str(min_n), str(max_n), str(interval_n)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    print(p.stderr)
    return pd.read_csv(p.stdout, delimiter="\t")

distributions=["uniform"]
querytypes=["similar"]
results_dir = "results/qf"
if not os.path.exists(results_dir):
    os.makedirs(results_dir)

q = 20
min_r=3
max_r=3
min_n=26214
max_n=996148
interval_n=32768

def run_qf_time_vs_size():
    df = qf_bench(q, min_r, max_r, min_n, max_n, interval_n)
    df = df[['alpha', 'n', 'FPR', 'Query time']]
    with open(results_dir + "/time_vs_size_r2.txt", "w") as f:
        df.to_csv(f)

def run_qf_n_vs_m():
    df = qf_n_vs_m(min_r, max_r, min_n, max_n, interval_n)
    df = df[['n', 'q', 'QF size', 'BF size', 'QF FPR', 'BF FPR']]
    with open(results_dir + "/qf_n_vs_m_2.txt", "w") as f:
        df.to_csv(f)

for dist in distributions:
    for qt in querytypes:
        print("Running bench for distribution = %s and query type = %s" % (dist, qt))
        #run_qf_time_vs_size()
        run_qf_n_vs_m()