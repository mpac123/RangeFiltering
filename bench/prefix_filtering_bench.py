import subprocess
import pandas as pd
import os
from functools import reduce

def run_prefixBF_bench(dist, qt, workload_dir, BF_min, BF_max, BF_interval, BF_max_doubting_level):
    cmd = ["../build/bench/prefixBF_bench", str(dist), str(qt), str(workload_dir), str(BF_min), str(BF_max), str(BF_interval), str(BF_max_doubting_level)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

def run_prefixQF_bench(dist, qt, workload_dir, QF_start_q, QF_end_q, QF_start_r, QF_end_r, QF_max_doubting_level):
    cmd = ["../build/bench/prefixQF_bench", str(dist), str(qt), str(workload_dir), str(QF_start_r), str(QF_end_r), str(QF_max_doubting_level)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

def run_SuRFReal_bench(dist, qt, workload_dir, SR_suffix_size_min, SR_suffix_size_max):
    cmd = ["../build/bench/SuRFReal_bench", str(dist), str(qt), str(workload_dir), str(SR_suffix_size_min), str(SR_suffix_size_max)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

distributions=["uniform", "normal", "powerlaw"]
querytypes=["similar", "random", "last_letter"]
dir="100k"
#test_id="prefix_filtering_surf"
test_id="prefix_filtering_no_surf"

# Params for 100k and 1st experiment
BF_min=1000000
BF_max=25000000
BF_interval=1000000
QF_start_q=21
QF_end_q=21
QF_start_r=2
QF_end_r=12
SR_suffix_size_min=0
SR_suffix_size_max=8

if dir=="100":
    BF_min=1000
    BF_max=20000
    BF_interval=1000
    QF_start_q=11
    QF_end_q=11
    QF_start_r=1
    QF_end_r=8

if dir=="1mln":
    BF_min=50000000
    BF_max=160000000
    BF_interval=20000000
    QF_start_q=22
    QF_end_q=22
    QF_start_r=4
    QF_end_r=4

if dir=="100k" and test_id=="prefix_filtering_no_surf":
    BF_min=5000000
    BF_max=25000000
    BF_interval=1000000
    QF_start_r=2
    QF_end_r=12

def run_prefix_fitlering_test_without_surf():
    df_prefixBF = run_prefixBF_bench(dist, qt, workload_dir, BF_min, BF_max, BF_interval, 0)
    df_prefixBF_doubting = run_prefixBF_bench(dist, qt, workload_dir, BF_min, BF_max, BF_interval, 3)
    df_prefixQF = run_prefixQF_bench(dist, qt, workload_dir, QF_start_q, QF_end_q, QF_start_r, QF_end_r, 0)
    df_prefixQF_doubting = run_prefixQF_bench(dist, qt, workload_dir, QF_start_q, QF_end_q, QF_start_r, QF_end_r, 3)

    df_prefixBF.rename(columns={'Creation time': 'Prefix-BF Creation time',
                                'Query time': 'Prefix-BF Query time'}, inplace=True)
    df_prefixBF_doubting.rename(columns={'FPR Prefix-BF': 'FPR Prefix-BF+Doubting',
                                         'Creation time': 'Prefix-BF+Doubting Creation time',
                                         'Query time': 'Prefix-BF+Doubting Query time'}, inplace=True)
    df_prefixQF.rename(columns={'Creation time': 'Prefix-QF Creation time',
                                'Query time': 'Prefix-QF Query time'}, inplace=True)
    df_prefixQF_doubting.rename(columns={'FPR Prefix-QF': 'FPR Prefix-QF+Doubting',
                                         'Creation time': 'Prefix-QF+Doubting Creation time',
                                         'Query time': 'Prefix-QF+Doubting Query time'}, inplace=True)

    print("Creating DF with memory/FPR stats")
    dataframes = [df_prefixBF[['Memory usage', 'FPR Prefix-BF']], 
                    df_prefixBF_doubting[['Memory usage', 'FPR Prefix-BF+Doubting']], 
                    df_prefixQF[['Memory usage', 'FPR Prefix-QF']], 
                    df_prefixQF_doubting[['Memory usage', 'FPR Prefix-QF+Doubting']]]
    df = reduce(lambda left,right: pd.merge(left, right, how='outer', on='Memory usage'), dataframes)

    with open(results_dir + "/" + dist + "_" + qt + ".txt", "w") as f:
        df.to_csv(f)

    print("Creating DF with memory/time stats")
    dataframes = [df_prefixBF[['Memory usage', 'Prefix-BF Creation time']], 
                    df_prefixBF_doubting[['Memory usage', 'Prefix-BF+Doubting Creation time']], 
                    df_prefixQF[['Memory usage', 'Prefix-QF Creation time']], 
                    df_prefixQF_doubting[['Memory usage', 'Prefix-QF+Doubting Creation time']]]
    df = reduce(lambda left,right: pd.merge(left, right, how='outer', on='Memory usage'), dataframes)

    with open(results_dir + "/" + dist + "_" + qt + "_creation.txt", "w") as f:
        df.to_csv(f)

    dataframes = [df_prefixBF[['Memory usage', 'Prefix-BF Query time']], 
                    df_prefixBF_doubting[['Memory usage', 'Prefix-BF+Doubting Query time']], 
                    df_prefixQF[['Memory usage', 'Prefix-QF Query time']], 
                    df_prefixQF_doubting[['Memory usage', 'Prefix-QF+Doubting Query time']]]
    df = reduce(lambda left,right: pd.merge(left, right, how='outer', on='Memory usage'), dataframes)

    with open(results_dir + "/" + dist + "_" + qt + "_query.txt", "w") as f:
        df.to_csv(f)

def run_prefix_fitlering_test_with_surf():
    df_prefixBF = run_prefixBF_bench(dist, qt, workload_dir, BF_min, BF_max, BF_interval, 0)
    df_prefixQF = run_prefixQF_bench(dist, qt, workload_dir, QF_start_q, QF_end_q, QF_start_r, QF_end_r, 0)
    df_SuRFReal = run_SuRFReal_bench(dist, qt, workload_dir, SR_suffix_size_min, SR_suffix_size_max)
    df_SuRFReal.rename(columns={'FPR': 'FPR SuRFReal'}, inplace=True)

    print("Creating DF with stats")
    dataframes = [df_prefixBF[['Memory usage', 'FPR Prefix-BF', 'FP Prob BF']], 
                    df_prefixQF[['Memory usage', 'FPR Prefix-QF', 'FP Prob QF']], 
                    df_SuRFReal[['Memory usage', 'FPR SuRFReal']]]
    df = reduce(lambda left,right: pd.merge(left, right, how='outer', on='Memory usage'), dataframes)

    with open(results_dir + "/" + dist + "_" + qt + ".txt", "w") as f:
        df.to_csv(f)

#results_dir = "results/" + dir
results_dir = "results/100k_doubting_time"
workload_dir = "workload-gen/workloads/%s/" % dir
if not os.path.exists(results_dir):
    os.makedirs(results_dir)

for dist in distributions:
    for qt in querytypes:
        print("Running bench for distribution = %s and query type = %s" % (dist, qt))
        if (test_id == "prefix_filtering_surf"):
            run_prefix_fitlering_test_with_surf()
        else:
            run_prefix_fitlering_test_without_surf()