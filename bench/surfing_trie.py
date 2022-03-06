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

distributions=["last_letter_different", "uniform", "normal", "powerlaw"]
querytypes=["similar", "random", "last_letter"]
dir="100k_new"
results_dir = "results/surfing_trie"

# Parameters
SR_suffix_size_min=0
SR_suffix_size_max=8

workload_dir = "workload-gen/workloads/%s/" % dir
if not os.path.exists(results_dir):
    os.makedirs(results_dir)

for dist in distributions:
    for qt in querytypes:
        print("Running bench for distribution = %s and query type = %s" % (dist, qt))
        df_surfing_trie = run_surfingTrie_bench(dist, qt, workload_dir, SR_suffix_size_min, SR_suffix_size_max)
        df_surf = run_SuRFReal_bench(dist, qt, workload_dir, SR_suffix_size_min, SR_suffix_size_max)

        df_surfing_trie.rename(columns={'FPR': 'FPR Surfing Trie'}, inplace=True)
        df_surf.rename(columns={'FPR': 'FPR SuRFReal'}, inplace=True)

        print("Creating DF with stats")
        dfs = [df_surfing_trie[['FPR Surfing Trie']],
               df_surf[['FPR SuRFReal']]]

        print(df_surf.head())
        print(df_surfing_trie.head())

        #df = reduce(lambda left,right: pd.merge(left, right, how='outer', on='Memory usage'), dfs)
        df = pd.concat(dfs, axis=1)
        with open(results_dir + "/" + dist + "_" + qt + ".txt", "w") as f:
            df.to_csv(f)