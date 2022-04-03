import subprocess
import pandas as pd
import os

def run_SuRFReal_bench(dist, qt, workload_dir, SR_suffix_size_min, SR_suffix_size_max):
    cmd = ["../build/bench/range_filtering_bench", str(dist), str(qt), str(workload_dir), "surf", str(SR_suffix_size_min), str(SR_suffix_size_max)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

def run_Rosetta_bench(dist, qt, workload_dir, RST_size_min, RST_size_max, RST_size_step):
    cmd = ["../build/bench/range_filtering_bench", str(dist), str(qt), str(workload_dir), "rosetta", str(RST_size_min), str(RST_size_max), str(RST_size_step)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

def run_LilRosetta_bench(dist, qt, workload_dir, RST_size_min, RST_size_max, RST_size_step):
    cmd = ["../build/bench/range_filtering_bench", str(dist), str(qt), str(workload_dir), "lilrosetta", str(RST_size_min), str(RST_size_max), str(RST_size_step)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

def run_FST_bench(dist, qt, workload_dir):
    cmd = ["../build/bench/range_filtering_bench", str(dist), str(qt), str(workload_dir), "fst"]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

def run_Splash_bench(dist, qt, workload_dir, SPL_cutoff, SPL_constraint_min, SPL_constraint_max, SPL_constraint_interval):
    cmd = ["../build/bench/range_filtering_bench", str(dist), str(qt), str(workload_dir), "splash", str(SPL_cutoff), str(SPL_constraint_min), str(SPL_constraint_max), str(SPL_constraint_interval)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

distributions=["last_letter_different", "uniform", "normal", "powerlaw"]
querytypes=["similar", "random", "last_letter"]
dir="100k"
results_dir = "range-filtering-results/100k_2"
workload_dir = "workload-gen/range_queries_workloads/%s/" % dir

if not os.path.exists(results_dir):
    os.makedirs(results_dir)

# Parameters
SR_suffix_size_min=0
SR_suffix_size_max=0
RST_size_min=30000000
RST_size_max=100000000
RST_size_step=10000000
LRST_size_min=5000000
LRST_size_max=50000000
LRST_size_step=10000000
SPL_cutoff=0.75
SPL_restraint_min=0.0
SPL_restraint_max=1.0
SPL_restraint_interval=0.05

for dist in distributions:
    for qt in querytypes:
        print("Running bench for distribution = %s and query type = %s" % (dist, qt))
        df_rosetta = run_Rosetta_bench(dist, qt, workload_dir, RST_size_min, RST_size_max, RST_size_step)
        df_lilrosetta = run_LilRosetta_bench(dist, qt, workload_dir, LRST_size_min, LRST_size_max, LRST_size_step)
        df_surf = run_SuRFReal_bench(dist, qt, workload_dir, SR_suffix_size_min, SR_suffix_size_max)
        df_splash = run_Splash_bench(dist, qt, workload_dir, SPL_cutoff, SPL_restraint_min, SPL_restraint_max, SPL_restraint_interval)
        df_fst = run_FST_bench(dist, qt, workload_dir)

        print(df_rosetta)
        print(df_lilrosetta)
        print(df_surf)
        print(df_splash)
        print(df_fst)

        df_rosetta.rename(columns={'FPR': 'Rosetta'}, inplace=True)
        df_lilrosetta.rename(columns={'FPR': 'LilRosetta'}, inplace=True)
        df_surf.rename(columns={'FPR': 'SuRF Real'}, inplace=True)
        df_splash.rename(columns={'FPR': 'Splash'}, inplace=True)
        df_fst.rename(columns={'FPR': 'FST'}, inplace=True)

        print("Creating DF with stats")
        dfs = [df_rosetta[['Memory usage', 'Rosetta']],
            df_lilrosetta[['Memory usage', 'LilRosetta']],
            df_splash[['Memory usage', 'Splash']],
            df_surf[['Memory usage', 'SuRF Real']],
            df_fst[['Memory usage', 'FST']]]

        df = pd.concat(dfs)

        with open(results_dir + "/" + dist + "_" + qt + ".txt", "w") as f:
            df.to_csv(f)