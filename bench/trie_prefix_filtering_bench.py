import subprocess
import pandas as pd
import os
from functools import reduce

def run_SuRFReal_bench(dist, qt, workload_dir, SR_suffix_size_min, SR_suffix_size_max):
    cmd = ["../build/bench/SuRFReal_bench", str(dist), str(qt), str(workload_dir), str(SR_suffix_size_min), str(SR_suffix_size_max)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

def run_surfingTrie_bench(dist, qt, workload_dir, SR_suffix_size_min, SR_suffix_size_max):
    cmd = ["../build/bench/surfingTrie_bench", str(dist), str(qt), str(workload_dir), str(SR_suffix_size_min), str(SR_suffix_size_max)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

def run_absoluteRestrainedSurfingTrie_bench(dist, qt, workload_dir, SR_suffix_size_min, SR_suffix_size_max, RST_abs_min, RST_abs_max):
    cmd = ["../build/bench/surfingTrie_bench", str(dist), str(qt), str(workload_dir), str(SR_suffix_size_min), str(SR_suffix_size_max), "absolute", str(RST_abs_min), str(RST_abs_max)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

def run_relativeRestrainedSurfingTrie_bench(dist, qt, workload_dir, SR_suffix_size_min, SR_suffix_size_max, RST_rel_min, RST_rel_max):
    cmd = ["../build/bench/surfingTrie_bench", str(dist), str(qt), str(workload_dir), str(SR_suffix_size_min), str(SR_suffix_size_max), "relative", str(RST_rel_min), str(RST_rel_max)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

def run_bloomedSurfingTrie_bench(dist, qt, workload_dir, SR_suffix_size_min, SR_suffix_size_max, BF_min, BF_max, BF_interval):
    cmd = ["../build/bench/surfingTrie_bench", str(dist), str(qt), str(workload_dir), str(SR_suffix_size_min), str(SR_suffix_size_max), "bloomed", str(BF_min), str(BF_max), str(BF_interval)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

def run_SplashyTrie_bench(dist, qt, workload_dir, SR_suffix_size_min, SR_suffix_size_max, ST_splashiness_min, ST_splashiness_max, ST_splashiness_interval):
    cmd = ["../build/bench/surfingTrie_bench", str(dist), str(qt), str(workload_dir), str(SR_suffix_size_min), str(SR_suffix_size_max), 
            "splashy", str(ST_splashiness_min), str(ST_splashiness_max), str(ST_splashiness_interval)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

def run_relativeRestrainedSplashyTrie_bench(dist, qt, workload_dir, SR_suffix_size_min, SR_suffix_size_max, ST_splashiness_min, ST_splashiness_max, ST_splashiness_interval, RST_rel_min, RST_rel_max):
    cmd = ["../build/bench/surfingTrie_bench", str(dist), str(qt), str(workload_dir), str(SR_suffix_size_min), str(SR_suffix_size_max), 
            "restrained_splashy", str(ST_splashiness_min), str(ST_splashiness_max), str(ST_splashiness_interval), "relative", str(RST_rel_min), str(RST_rel_max)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

def run_absoluteRestrainedSplashyTrie_bench(dist, qt, workload_dir, SR_suffix_size_min, SR_suffix_size_max, ST_splashiness_min, ST_splashiness_max, ST_splashiness_interval, RST_abs_min, RST_abs_max):
    cmd = ["../build/bench/surfingTrie_bench", str(dist), str(qt), str(workload_dir), str(SR_suffix_size_min), str(SR_suffix_size_max), 
            "restrained_splashy", str(ST_splashiness_min), str(ST_splashiness_max), str(ST_splashiness_interval), "absolute", str(RST_abs_min), str(RST_abs_max)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

distributions=["last_letter_different", "uniform", "normal", "powerlaw"]
querytypes=["similar", "random", "last_letter"]
dir="100k_new"
results_dir = "results/trie_prefix_01"
workload_dir = "workload-gen/workloads/%s/" % dir

if not os.path.exists(results_dir):
    os.makedirs(results_dir)

# Parameters
SR_suffix_size_min=0
SR_suffix_size_max=0
RST_abs_min=0
RST_abs_max = 15
RST_rel_min = 0.0
RST_rel_max = 1.0
BF_min = 10000000
BF_max = 300000000
BF_interval = 50000000
ST_splashiness_min = 0.8
ST_splashiness_max = 1.0
ST_splashiness_interval = 0.05

for dist in distributions:
    for qt in querytypes:
        print("Running bench for distribution = %s and query type = %s" % (dist, qt))
        df_surfing_trie = run_surfingTrie_bench(dist, qt, workload_dir, SR_suffix_size_min, SR_suffix_size_max)
        df_absolute_st = run_absoluteRestrainedSurfingTrie_bench(dist, qt, workload_dir, SR_suffix_size_min, SR_suffix_size_max, RST_abs_min, RST_abs_max)
        df_relative_st = run_relativeRestrainedSurfingTrie_bench(dist, qt, workload_dir, SR_suffix_size_min, SR_suffix_size_max, RST_rel_min, RST_rel_max)
        df_bloomed_sf = run_bloomedSurfingTrie_bench(dist, qt, workload_dir, SR_suffix_size_min, SR_suffix_size_max, BF_min, BF_max, BF_interval)
        df_splashy_trie = run_SplashyTrie_bench(dist, qt, workload_dir, SR_suffix_size_min, SR_suffix_size_max, ST_splashiness_min, ST_splashiness_max, ST_splashiness_interval)
        df_abs_splashy = run_absoluteRestrainedSplashyTrie_bench(dist, qt, workload_dir, SR_suffix_size_min, SR_suffix_size_max, 0.8, 0.9, 0.2, RST_abs_min, RST_abs_max)
        df_rel_splashy = run_relativeRestrainedSplashyTrie_bench(dist, qt, workload_dir, SR_suffix_size_min, SR_suffix_size_max, 0.8, 0.9, 0.2, RST_rel_min, RST_rel_max)
        
        df_surfing_trie.rename(columns={'FPR': 'Surfing Trie'}, inplace=True)
        df_absolute_st.rename(columns={'FPR': 'Absolutely Restrained Surfing Trie'}, inplace=True)
        df_relative_st.rename(columns={'FPR': 'Relatively Restrained Surfing Trie'}, inplace=True)
        df_bloomed_sf.rename(columns={'FPR': 'Bloomed Surfing Trie'}, inplace=True)
        df_splashy_trie.rename(columns={'FPR': 'Splashy Trie'}, inplace=True)
        df_abs_splashy.rename(columns={'FPR': 'Absolutely Restrained Splashy Trie'}, inplace=True)
        df_rel_splashy.rename(columns={'FPR': 'Relatively Restrained Splashy Trie'}, inplace=True)

        print(df_splashy_trie.head())
        print(df_abs_splashy.head())
        print(df_rel_splashy.head())

        df_trie = pd.DataFrame(data={'Memory usage': [df_surfing_trie['Trie memory usage'].values[0]], 'Trie': 0.0})

        print("Creating DF with stats")
        dfs = [df_absolute_st[['Memory usage', 'Absolutely Restrained Surfing Trie']],
               df_relative_st[['Memory usage', 'Relatively Restrained Surfing Trie']],
               df_abs_splashy[['Memory usage', 'Absolutely Restrained Splashy Trie']],
               df_rel_splashy[['Memory usage', 'Relatively Restrained Splashy Trie']],
               df_bloomed_sf[['Memory usage', 'Bloomed Surfing Trie']],
               df_splashy_trie[['Memory usage', 'Splashy Trie']],
               df_surfing_trie[['Memory usage', 'Surfing Trie']],
               df_trie[['Memory usage', 'Trie']]]

        # df = reduce(lambda left,right: pd.merge(left, right, how='outer', on='Memory usage'), dfs)
        df = pd.concat(dfs)

        with open(results_dir + "/" + dist + "_" + qt + ".txt", "w") as f:
            df.to_csv(f)