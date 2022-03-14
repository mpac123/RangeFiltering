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

def run_splash(dist, qt, workload_dir, cut_off_min, cut_off_max, cut_off_interval, constraint_type, constraint_min, constraint_max, constraint_interval):
    cmd = ["../build/bench/prefix_filtering_bench", str(dist), str(qt), str(workload_dir), "splash", str(cut_off_min), str(cut_off_max), str(cut_off_interval), str(constraint_type), 
    str(constraint_min), str(constraint_max), str(constraint_interval)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

def run_fst(dist, qt, workload_dir):
    cmd = ["../build/bench/prefix_filtering_bench", str(dist), str(qt), str(workload_dir), "fst"]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

def run_surf(dist, qt, workload_dir):
    cmd = ["../build/bench/prefix_filtering_bench", str(dist), str(qt), str(workload_dir), "surf", str(0), str(0)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

def run_prefixBF_bench(dist, qt, workload_dir, BF_min, BF_max, BF_interval, BF_max_doubting_level, multi = False, memory_allocation_type = "proportional", max_mem_allocation_diff = 0.0):
    cmd = ["../build/bench/prefixBF_bench", str(dist), str(qt), str(workload_dir), str(BF_min), str(BF_max), str(BF_interval), str(BF_max_doubting_level)]
    if multi:
        cmd = ["../build/bench/prefixBF_bench", str(dist), str(qt), str(workload_dir), str(BF_min), str(BF_max), str(BF_interval), str(BF_max_doubting_level), "multi", memory_allocation_type, str(max_mem_allocation_diff)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    return pd.read_csv(p.stdout, delimiter="\t")

distributions=["last_letter_different", "uniform", "normal", "powerlaw"]
querytypes=["similar", "random", "last_letter"]
dir="100k_new"
results_dir = "results/splash"
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
BF_min = 500000
BF_max = 5000000
BF_interval = 500000
ST_splashiness_min = 0.8
ST_splashiness_max = 1.0
ST_splashiness_interval = 0.05
SPL_restraint_min = 0.0
SPL_restraint_max = 1.0
SPL_restraint_interval = 0.1
splash = True

for dist in distributions:
    for qt in querytypes:
        print("Running bench for distribution = %s and query type = %s" % (dist, qt))
        if splash:
            df_splash_0_7 = run_splash(dist, qt, workload_dir, 0.7, 0.7, 0.05, "relative", SPL_restraint_min, SPL_restraint_max, SPL_restraint_interval)
            df_splash_0_8 = run_splash(dist, qt, workload_dir, 0.8, 0.8, 0.05, "relative", SPL_restraint_min, SPL_restraint_max, SPL_restraint_interval)
            df_splash_0_9 = run_splash(dist, qt, workload_dir, 0.9, 0.9, 0.05, "relative", SPL_restraint_min, SPL_restraint_max, SPL_restraint_interval)
            df_splash_1_0 = run_splash(dist, qt, workload_dir, 1.0, 1.0, 0.05, "relative", SPL_restraint_min, SPL_restraint_max, SPL_restraint_interval)
            df_surf_real = run_surf(dist, qt, workload_dir)
            df_bf = run_prefixBF_bench(dist, qt, workload_dir, BF_min, BF_max, BF_interval, 3, True, "proportional", 0.5)
            df_fst = run_fst(dist, qt, workload_dir)

            print(df_splash_0_7)
            #print(df_surf_real.head())
            #print(df_bf)
            #print(df_fst)

            df_splash_0_7.rename(columns={'FPR': 'Splash y=0.7'}, inplace=True)
            df_splash_0_8.rename(columns={'FPR': 'Splash y=0.8'}, inplace=True)
            df_splash_0_9.rename(columns={'FPR': 'Splash y=0.9'}, inplace=True)
            df_splash_1_0.rename(columns={'FPR': 'Splash y=1.0'}, inplace=True)
            df_surf_real.rename(columns={'FPR': 'SuRF Real'}, inplace=True)
            df_bf.rename(columns={'FPR Prefix-BF': 'Bloom Filter z=0.5'}, inplace=True)
            df_fst.rename(columns={'FPR': 'FST'}, inplace=True)
            
            print("Creating DF with stats")
            dfs = [df_splash_0_7[['Memory usage', 'Splash y=0.7']],
                df_splash_0_8[['Memory usage', 'Splash y=0.8']],
                df_splash_0_9[['Memory usage', 'Splash y=0.9']],
                df_splash_1_0[['Memory usage', 'Splash y=1.0']],
                df_surf_real[['Memory usage', 'SuRF Real']],
                df_bf[['Memory usage', 'Bloom Filter z=0.5']], 
                df_fst[['Memory usage', 'FST']]]

        else:
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