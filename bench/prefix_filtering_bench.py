import subprocess
import pandas as pd
import os
from functools import reduce

def run_prefixBF_bench(dist, qt, workload_dir, BF_min, BF_max, BF_interval, BF_max_doubting_level, multi = False, memory_allocation_type = "proportional", max_mem_allocation_diff = 0.0):
    cmd = ["../build/bench/prefixBF_bench", str(dist), str(qt), str(workload_dir), str(BF_min), str(BF_max), str(BF_interval), str(BF_max_doubting_level)]
    if multi:
        cmd = ["../build/bench/prefixBF_bench", str(dist), str(qt), str(workload_dir), str(BF_min), str(BF_max), str(BF_interval), str(BF_max_doubting_level), "multi", memory_allocation_type, str(max_mem_allocation_diff)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    return pd.read_csv(p.stdout, delimiter="\t")

def run_prefixQF_bench(dist, qt, workload_dir, QF_start_q, QF_end_q, QF_start_r, QF_end_r, QF_max_doubting_level, multi = False):
    cmd = ["../build/bench/prefixQF_bench", str(dist), str(qt), str(workload_dir), str(QF_start_r), str(QF_end_r), str(QF_max_doubting_level)]
    if multi:
        cmd = ["../build/bench/prefixQF_bench", str(dist), str(qt), str(workload_dir), str(QF_start_r), str(QF_end_r), str(QF_max_doubting_level), "multi"]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

def run_SuRFReal_bench(dist, qt, workload_dir, SR_suffix_size_min, SR_suffix_size_max):
    cmd = ["../build/bench/SuRFReal_bench", str(dist), str(qt), str(workload_dir), str(SR_suffix_size_min), str(SR_suffix_size_max)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

distributions=["last_letter_different", "uniform", "normal", "powerlaw", "increasing_degree"]
#distributions=["uniform"]
querytypes=["similar", "random", "common_prefix", "last_letter"]
#querytypes=["similar"]
dir="100k_26_15_3"
#test_id="prefix_filtering_surf"
test_id="prefix_filtering_no_surf"
#test_id="multi_level"
#test_id = "qf"

# Params for 100k and 1st experiment
BF_min=1000000
BF_max=25000000
BF_interval=1000000
QF_start_q=21
QF_end_q=21
QF_start_r=1
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

if dir=="100k_new" and test_id=="multi_level":
    BF_min=1000000
    BF_max=15000000
    BF_interval=500000
    SR_suffix_size_min=0
    SR_suffix_size_max=8

def run_prefix_fitlering_test_without_surf():
    df_prefixBF = run_prefixBF_bench(dist, qt, workload_dir, BF_min, BF_max, BF_interval, 0)
    df_prefixBF_doubting_3 = run_prefixBF_bench(dist, qt, workload_dir, BF_min, BF_max, BF_interval, 3)
    df_prefixBF_doubting_6 = run_prefixBF_bench(dist, qt, workload_dir, BF_min, BF_max, BF_interval, 6)
    df_prefixQF = run_prefixQF_bench(dist, qt, workload_dir, QF_start_q, QF_end_q, QF_start_r, QF_end_r, 0)
    df_prefixQF_doubting_3 = run_prefixQF_bench(dist, qt, workload_dir, QF_start_q, QF_end_q, QF_start_r, QF_end_r, 3)
    df_prefixQF_doubting_6 = run_prefixQF_bench(dist, qt, workload_dir, QF_start_q, QF_end_q, QF_start_r, QF_end_r, 6)


    df_prefixBF.rename(columns={'FPR Prefix-BF': 'Prefix-BF',
                                'Creation time': 'Prefix-BF Creation time',
                                'Query time': 'Prefix-BF Query time'}, inplace=True)
    df_prefixBF_doubting_3.rename(columns={'FPR Prefix-BF': 'Prefix-BF+Doubting d=3',
                                        'Creation time': 'Prefix-BF+Doubting Creation time',
                                        'Query time': 'Prefix-BF+Doubting Query time'}, inplace=True)
    df_prefixBF_doubting_6.rename(columns={'FPR Prefix-BF': 'Prefix-BF+Doubting d=6',
                                        'Creation time': 'Prefix-BF+Doubting Creation time',
                                        'Query time': 'Prefix-BF+Doubting Query time'}, inplace=True)
    df_prefixQF.rename(columns={'FPR Prefix-QF': 'Prefix-QF',
                                'Creation time': 'Prefix-QF Creation time',
                                'Query time': 'Prefix-QF Query time'}, inplace=True)
    df_prefixQF_doubting_3.rename(columns={'FPR Prefix-QF': 'Prefix-QF+Doubting d=3',
                                        'Creation time': 'Prefix-QF+Doubting Creation time',
                                        'Query time': 'Prefix-QF+Doubting Query time'}, inplace=True)
    df_prefixQF_doubting_6.rename(columns={'FPR Prefix-QF': 'Prefix-QF+Doubting d=6',
                                        'Creation time': 'Prefix-QF+Doubting Creation time',
                                        'Query time': 'Prefix-QF+Doubting Query time'}, inplace=True)

    print("Creating DF with memory/FPR stats")
    dataframes = [df_prefixBF[['Memory usage', 'Prefix-BF']], 
                    df_prefixBF_doubting_3[['Memory usage', 'Prefix-BF+Doubting d=3']], 
                    df_prefixBF_doubting_6[['Memory usage', 'Prefix-BF+Doubting d=6']], 
                    df_prefixQF[['Memory usage', 'Prefix-QF']],
                    df_prefixQF_doubting_3[['Memory usage', 'Prefix-QF+Doubting d=3']],
                    df_prefixQF_doubting_6[['Memory usage', 'Prefix-QF+Doubting d=6']]]
    df = reduce(lambda left,right: pd.merge(left, right, how='outer', on='Memory usage'), dataframes)

    with open(results_dir + "/" + dist + "_" + qt + ".txt", "w") as f:
        df.to_csv(f)

    # print("Creating DF with memory/time stats")
    # dataframes = [df_prefixBF[['Memory usage', 'Prefix-BF Creation time']], 
    #                 df_prefixBF_doubting[['Memory usage', 'Prefix-BF+Doubting Creation time']], 
    #                 df_prefixQF[['Memory usage', 'Prefix-QF Creation time']], 
    #                 df_prefixQF_doubting[['Memory usage', 'Prefix-QF+Doubting Creation time']]]
    # df = reduce(lambda left,right: pd.merge(left, right, how='outer', on='Memory usage'), dataframes)

    # with open(results_dir + "/" + dist + "_" + qt + "_creation.txt", "w") as f:
    #     df.to_csv(f)

    # dataframes = [df_prefixBF[['Memory usage', 'Prefix-BF Query time']], 
    #                 df_prefixBF_doubting[['Memory usage', 'Prefix-BF+Doubting Query time']], 
    #                 df_prefixQF[['Memory usage', 'Prefix-QF Query time']], 
    #                 df_prefixQF_doubting[['Memory usage', 'Prefix-QF+Doubting Query time']]]
    # df = reduce(lambda left,right: pd.merge(left, right, how='outer', on='Memory usage'), dataframes)

    # with open(results_dir + "/" + dist + "_" + qt + "_query.txt", "w") as f:
    #     df.to_csv(f)

def run_prefix_fitlering_test_with_surf():
    df_prefixBF = run_prefixBF_bench(dist, qt, workload_dir, BF_min, BF_max, BF_interval, 0)
    df_prefixBF_doubting = run_prefixBF_bench(dist, qt, workload_dir, BF_min, BF_max, BF_interval, 3)
    df_prefixQF = run_prefixQF_bench(dist, qt, workload_dir, QF_start_q, QF_end_q, QF_start_r, QF_end_r, 0)
    df_prefixQF_doubting = run_prefixQF_bench(dist, qt, workload_dir, QF_start_q, QF_end_q, QF_start_r, QF_end_r, 3)
    df_SuRFReal = run_SuRFReal_bench(dist, qt, workload_dir, SR_suffix_size_min, SR_suffix_size_max)
    df_SuRFReal.rename(columns={'FPR': 'FPR SuRFReal'}, inplace=True)
    df_prefixBF_doubting.rename(columns={'FPR Prefix-BF': 'FPR Prefix-BF+Doubting'}, inplace=True)
    df_prefixQF_doubting.rename(columns={'FPR Prefix-QF': 'FPR Prefix-QF+Doubting'}, inplace=True)

    print("Creating DF with stats")
    dataframes = [df_prefixBF[['Memory usage', 'FPR Prefix-BF']], 
                  df_prefixBF_doubting[['Memory usage', 'FPR Prefix-BF+Doubting']], 
                  df_prefixQF[['Memory usage', 'FPR Prefix-QF']], 
                  df_prefixQF_doubting[['Memory usage', 'FPR Prefix-QF+Doubting']],
                  df_SuRFReal[['Memory usage', 'FPR SuRFReal']]]
    df = reduce(lambda left,right: pd.merge(left, right, how='outer', on='Memory usage'), dataframes)

    with open(results_dir + "/" + dist + "_" + qt + ".txt", "w") as f:
        df.to_csv(f)

def run_prefix_filtering_test_multilevel():
    df_prefixBF = run_prefixBF_bench(dist, qt, workload_dir, BF_min, BF_max, BF_interval, 3)
    df_multiPBF_proportional_0 = run_prefixBF_bench(dist, qt, workload_dir, BF_min, BF_max, BF_interval, 3, True, "proportional", 0.0)
    df_multiPBF_proportional_0_3 = run_prefixBF_bench(dist, qt, workload_dir, BF_min, BF_max, BF_interval, 3, True, "proportional", 0.3)
    df_multiPBF_proportional_0_5 = run_prefixBF_bench(dist, qt, workload_dir, BF_min, BF_max, BF_interval, 3, True, "proportional", 0.5)
    df_multiPBF_proportional_0_7 = run_prefixBF_bench(dist, qt, workload_dir, BF_min, BF_max, BF_interval, 3, True, "proportional", 0.7)
    #df_multiPBF_equal_0 = run_prefixBF_bench(dist, qt, workload_dir, BF_min, BF_max, BF_interval, 3, True, "equal", 0.0)
    #df_multiPBF_equal_0_1 = run_prefixBF_bench(dist, qt, workload_dir, BF_min, BF_max, BF_interval, 3, True, "equal", 0.1)
    # df_SuRFReal = run_SuRFReal_bench(dist, qt, workload_dir, SR_suffix_size_min, SR_suffix_size_max)

    df_multiPBF_proportional_0.rename(columns={'FPR Prefix-BF': 'Multi-Prefix-BF (p=0.0)'}, inplace=True)
    df_multiPBF_proportional_0_3.rename(columns={'FPR Prefix-BF': 'Multi-Prefix-BF (p=0.3)'}, inplace=True)
    df_multiPBF_proportional_0_5.rename(columns={'FPR Prefix-BF': 'Multi-Prefix-BF (p=0.5)'}, inplace=True)
    df_multiPBF_proportional_0_7.rename(columns={'FPR Prefix-BF': 'Multi-Prefix-BF (p=0.7)'}, inplace=True)
    #df_multiPBF_equal_0.rename(columns={'FPR Prefix-BF': 'FPR Multi-Prefix-BF Equal'}, inplace=True)
    #df_multiPBF_equal_0_1.rename(columns={'FPR Prefix-BF': 'FPR Multi-Prefix-BF Equal With Decreasing'}, inplace=True)
    # df_SuRFReal.rename(columns={'FPR': 'FPR SuRFReal'}, inplace=True)

    # print(df_multiPBF_proportional_0_7)

    print("Creating DF with stats")
    dataframes = [df_prefixBF[['Memory usage', 'FPR Prefix-BF']],
                    df_multiPBF_proportional_0[['Memory usage', 'Multi-Prefix-BF (p=0.0)']],
                    df_multiPBF_proportional_0_3[['Memory usage', 'Multi-Prefix-BF (p=0.3)']],
                    df_multiPBF_proportional_0_5[['Memory usage', 'Multi-Prefix-BF (p=0.5)']],
                    df_multiPBF_proportional_0_7[['Memory usage', 'Multi-Prefix-BF (p=0.7)']]]
                    # df_SuRFReal[['Memory usage', 'FPR SuRFReal']]]
                    #df_prefixQF[['Memory usage', 'FPR Prefix-QF', 'FP Prob QF']],
                    #df_multiPQF[['Memory usage', 'FPR Multi-Prefix-QF']]]
    df = reduce(lambda left,right: pd.merge(left, right, how='outer', on='Memory usage'), dataframes)

    with open(results_dir + "/" + dist + "_" + qt + ".txt", "w") as f:
        df.to_csv(f)

def run_prefix_filtering_test_multilevel_no_penalty():
    df_prefixBF = run_prefixBF_bench(dist, qt, workload_dir, BF_min, BF_max, BF_interval, 3)
    df_multiPBF_proportional_0 = run_prefixBF_bench(dist, qt, workload_dir, BF_min, BF_max, BF_interval, 3, True, "proportional", 0.0)
    # df_multiPBF_proportional_0_3 = run_prefixBF_bench(dist, qt, workload_dir, BF_min, BF_max, BF_interval, 3, True, "proportional", 0.3)
    # df_multiPBF_proportional_0_5 = run_prefixBF_bench(dist, qt, workload_dir, BF_min, BF_max, BF_interval, 3, True, "proportional", 0.5)
    # df_multiPBF_proportional_0_7 = run_prefixBF_bench(dist, qt, workload_dir, BF_min, BF_max, BF_interval, 3, True, "proportional", 0.7)
    #df_multiPBF_equal_0 = run_prefixBF_bench(dist, qt, workload_dir, BF_min, BF_max, BF_interval, 3, True, "equal", 0.0)
    #df_multiPBF_equal_0_1 = run_prefixBF_bench(dist, qt, workload_dir, BF_min, BF_max, BF_interval, 3, True, "equal", 0.1)
    # df_SuRFReal = run_SuRFReal_bench(dist, qt, workload_dir, SR_suffix_size_min, SR_suffix_size_max)

    df_prefixBF.rename(columns={'FPR Prefix-BF': 'Prefix-BF'}, inplace=True)
    df_multiPBF_proportional_0.rename(columns={'FPR Prefix-BF': 'Multi-Prefix-BF'}, inplace=True)
    # df_multiPBF_proportional_0_3.rename(columns={'FPR Prefix-BF': 'FPR Multi-Prefix-BF (0.3)'}, inplace=True)
    # df_multiPBF_proportional_0_5.rename(columns={'FPR Prefix-BF': 'FPR Multi-Prefix-BF (0.5)'}, inplace=True)
    # df_multiPBF_proportional_0_7.rename(columns={'FPR Prefix-BF': 'FPR Multi-Prefix-BF (0.7)'}, inplace=True)
    #df_multiPBF_equal_0.rename(columns={'FPR Prefix-BF': 'FPR Multi-Prefix-BF Equal'}, inplace=True)
    #df_multiPBF_equal_0_1.rename(columns={'FPR Prefix-BF': 'FPR Multi-Prefix-BF Equal With Decreasing'}, inplace=True)
    #df_SuRFReal.rename(columns={'FPR': 'FPR SuRFReal'}, inplace=True)

    # print(df_multiPBF_proportional_0_7)

    print("Creating DF with stats")
    dataframes = [df_prefixBF[['Memory usage', 'Prefix-BF']],
                    df_multiPBF_proportional_0[['Memory usage', 'Multi-Prefix-BF']]]
                    # df_multiPBF_proportional_0_3[['Memory usage', 'FPR Multi-Prefix-BF (0.3)']],
                    # df_multiPBF_proportional_0_5[['Memory usage', 'FPR Multi-Prefix-BF (0.5)']],
                    # df_multiPBF_proportional_0_7[['Memory usage', 'FPR Multi-Prefix-BF (0.7)']],
                    # df_SuRFReal[['Memory usage', 'FPR SuRFReal']]]
                    #df_prefixQF[['Memory usage', 'FPR Prefix-QF', 'FP Prob QF']],
                    #df_multiPQF[['Memory usage', 'FPR Multi-Prefix-QF']]]
    df = reduce(lambda left,right: pd.merge(left, right, how='outer', on='Memory usage'), dataframes)

    with open(results_dir + "/" + dist + "_" + qt + ".txt", "w") as f:
        df.to_csv(f)

#results_dir = "results/" + dir
results_dir = "results/100k_multilevelbf_penalty"
#results_dir = "results/100k_time"
#results_dir = "results/100k_smaller_qf"
workload_dir = "workload-gen/workloads/%s/" % dir
if not os.path.exists(results_dir):
    os.makedirs(results_dir)

# for dist in distributions:
#     for qt in querytypes:
#         print("Running bench for distribution = %s and query type = %s" % (dist, qt))
#         if (test_id == "prefix_filtering_surf"):
#             run_prefix_fitlering_test_with_surf()
#         elif (test_id == "prefix_filtering_no_surf"):
#             run_prefix_fitlering_test_without_surf()
#         elif (test_id == "multi_level"):
#             run_prefix_filtering_test_multilevel()
#         else:
#             print("No test found")


for dist in distributions:
    for qt in querytypes:
        print("Running bench for distribution = %s and query type = %s" % (dist, qt))
        run_prefix_filtering_test_multilevel()