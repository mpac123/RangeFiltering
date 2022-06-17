import subprocess
import pandas as pd
import os

def run_SuRFBase_bench(dist, workload_dir, u, n):
    cmd = ["../build/bench/integer_range_filtering_bench", str(dist), str(workload_dir), "surf", str(u), str(n)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

def run_SuRFReal_bench(dist, workload_dir, u, n):
    cmd = ["../build/bench/integer_range_filtering_bench", str(dist), str(workload_dir), "surfreal", str(u), str(n)]
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

def run_rangeBF_bench(dist, workload_dir, u, n, RBF_size_min, RBF_size_max, RBF_size_step):
    cmd = ["../build/bench/integer_range_filtering_bench", str(dist), str(workload_dir), "rangeBF", str(u), str(n), str(RBF_size_min), str(RBF_size_max), str(RBF_size_step)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

def run_chareq_bench(dist, workload_dir, u, n, CHRQ_frac_min, CHRQ_frac_max, CHRQ_frac_step, CHRQ_top_layer_height):
    cmd = ["../build/bench/integer_range_filtering_bench", str(dist), str(workload_dir), "chareq", str(u), str(n), str(CHRQ_frac_min), str(CHRQ_frac_max), str(CHRQ_frac_step)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

def run_splash(dist, workload_dir, u, n, cut_off_min, cut_off_max, cut_off_interval, constraint_type, constraint_min, constraint_max, constraint_interval):
    cmd = ["../build/bench/integer_range_filtering_bench", str(dist), str(workload_dir), "splash", str(u), str(n), str(cut_off_min), str(cut_off_max), str(cut_off_interval), str(constraint_type), 
    str(constraint_min), str(constraint_max), str(constraint_interval)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

def run_FST_bench(dist, workload_dir, u, n):
    cmd = ["../build/bench/integer_range_filtering_bench", str(dist), str(workload_dir), "fst", str(u), str(n)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

distributions=["uniform", "normal"]
ns = ["50", "200", "500"]
dir="1e5"
results_dir = "integer-range-filtering-results/1e5"
workload_dir = "workload-gen/integer_workloads/%s/" % dir

if not os.path.exists(results_dir):
    os.makedirs(results_dir)

RST_size_min=2000
RST_size_max=10000
RST_size_step=2000
u=1000000000
RBF_size_min=1000
RBF_size_max=10000
RBF_size_step=1000
CHRQ_frac_min=0.5
CHRQ_frac_max=0.89
CHRQ_frac_step=0.05
SPL_restraint_min = 0.0
SPL_restraint_max = 1.0
SPL_restraint_interval = 0.1
SPL_abs_restraint_min = 0
SPL_abs_restraint_max = 15
SPL_abs_restraint_interval = 1

print("hola")
for n in ns:
    for dist in distributions:
        print("Running bench for distribution = %s" % (dist))
        df_surf = run_SuRFBase_bench(dist, workload_dir, u, n)
        print(df_surf)
        df_surf_real = run_SuRFReal_bench(dist, workload_dir, u, n)
        print(df_surf_real)
        df_range_bf = run_rangeBF_bench(dist, workload_dir, u, n, RBF_size_min, RBF_size_max, RBF_size_step)
        print(df_range_bf)
        df_rosetta = run_Rosetta_bench(dist, workload_dir, RST_size_min, RST_size_max, RST_size_step, u, n)
        print(df_rosetta)
        df_chareq = run_chareq_bench(dist, workload_dir, u, n, CHRQ_frac_min, CHRQ_frac_max, CHRQ_frac_step, 1)
        print(df_chareq)
        df_empty_ranges = run_EmptyRanges_bench(dist, workload_dir, u, n)
        print(df_empty_ranges)
        df_sorted_arr = pd.DataFrame(data={'Memory usage': [int(n)*4], 'Sorted array': [0.0]})
        print(df_sorted_arr)
        df_splash_rel = run_splash(dist, workload_dir, u, n,  0.8, 0.8, 0.05, "relative", SPL_restraint_min, SPL_restraint_max, SPL_restraint_interval)
        print(df_splash_rel)
        df_splash_abs = run_splash(dist, workload_dir, u, n, 0.8, 0.8, 0.05, "absolute", SPL_abs_restraint_min, SPL_abs_restraint_max, SPL_abs_restraint_interval)
        print(df_splash_abs)
        df_fst = run_FST_bench(dist, workload_dir, u, n)
        print(df_fst)

        df_surf.rename(columns={'FPR': 'SuRF Base'}, inplace=True)
        df_surf_real.rename(columns={'FPR': 'SuRF Real'}, inplace=True)
        df_range_bf.rename(columns={'FPR': 'RangeBF'}, inplace=True)
        df_rosetta.rename(columns={'FPR': 'Rosetta'}, inplace=True)
        df_chareq.rename(columns={'FPR': 'CHaREQ'}, inplace=True)
        df_empty_ranges.rename(columns={'FPR': 'Empty Ranges'}, inplace=True)
        df_splash_rel.rename(columns={'FPR': 'Splash Rel. cut-off=0.8'}, inplace=True)
        df_splash_abs.rename(columns={'FPR': 'Splash Abs. cut-off=0.8'}, inplace=True)
        df_fst.rename(columns={'FPR': 'FST'}, inplace=True)
        
        print("Creating DF with stats")
        dfs = [df_range_bf[['Memory usage', 'RangeBF']],
                df_rosetta[['Memory usage', 'Rosetta']],
                df_empty_ranges[['Memory usage', 'Empty Ranges']],
                df_sorted_arr[['Memory usage', 'Sorted array']],
                df_chareq[['Memory usage', 'CHaREQ']],
                df_splash_rel[['Memory usage', 'Splash Rel. cut-off=0.8']],
                df_splash_abs[['Memory usage', 'Splash Abs. cut-off=0.8']],
                df_surf_real[['Memory usage', 'SuRF Real']],
                df_surf[['Memory usage', 'SuRF Base']],
                df_fst[['Memory usage', 'FST']]]

        df = pd.concat(dfs)

        with open(results_dir + "/" + dist + "_" + n + ".txt", "w") as f:
            df.to_csv(f)

