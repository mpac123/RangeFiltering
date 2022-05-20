import subprocess
import pandas as pd
import os

def run_SuRFBase_bench(dist, qt, workload_dir, SR_suffix_size_min, SR_suffix_size_max):
    cmd = ["../build/bench/range_filtering_bench", str(dist), str(qt), str(workload_dir), "surf", str(SR_suffix_size_min), str(SR_suffix_size_max)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

def run_SuRFReal_bench(dist, qt, workload_dir, SR_suffix_size_min, SR_suffix_size_max):
    cmd = ["../build/bench/range_filtering_bench", str(dist), str(qt), str(workload_dir), "surfreal", str(SR_suffix_size_min), str(SR_suffix_size_max)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

def run_SuRFHash_bench(dist, qt, workload_dir, SR_suffix_size_min, SR_suffix_size_max):
    cmd = ["../build/bench/range_filtering_bench", str(dist), str(qt), str(workload_dir), "surfhash", str(SR_suffix_size_min), str(SR_suffix_size_max)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

def run_SuRFMixed_bench(dist, qt, workload_dir, SR_suffix_size_min, SR_suffix_size_max):
    cmd = ["../build/bench/range_filtering_bench", str(dist), str(qt), str(workload_dir), "surfmixed", str(SR_suffix_size_min), str(SR_suffix_size_max)]
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

def run_splash(dist, qt, workload_dir, cut_off_min, cut_off_max, cut_off_interval, constraint_type, constraint_min, constraint_max, constraint_interval):
    cmd = ["../build/bench/range_filtering_bench", str(dist), str(qt), str(workload_dir), "splash", str(cut_off_min), str(cut_off_max), str(cut_off_interval), str(constraint_type), 
    str(constraint_min), str(constraint_max), str(constraint_interval)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

def run_rangeBF_bench(dist, qt, workload_dir, RBF_size_min, RBF_size_max, RBF_size_step):
    cmd = ["../build/bench/range_filtering_bench", str(dist), str(qt), str(workload_dir), "rangeBF", str(RBF_size_min), str(RBF_size_max), str(RBF_size_step)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

def run_rangeKRBF_bench(dist, qt, workload_dir, RBF_size_min, RBF_size_max, RBF_size_step):
    cmd = ["../build/bench/range_filtering_bench", str(dist), str(qt), str(workload_dir), "rangeKRBF", str(RBF_size_min), str(RBF_size_max), str(RBF_size_step)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

def run_bloomedRangeSplash_bench(dist, qt, workload_dir, RBF_size_min, RBF_size_max, RBF_size_step, FST_height):
    cmd = ["../build/bench/range_filtering_bench", str(dist), str(qt), str(workload_dir), "bloomedsplash", str(RBF_size_min), str(RBF_size_max), str(RBF_size_step), str(FST_height)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

def run_chareq_bench(dist, qt, workload_dir, CHRQ_frac_min, CHRQ_frac_max, CHRQ_frac_step, CHRQ_top_layer_height):
    cmd = ["../build/bench/range_filtering_bench", str(dist), str(qt), str(workload_dir), "chareq", str(CHRQ_top_layer_height), str(CHRQ_frac_min), str(CHRQ_frac_max), str(CHRQ_frac_step)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

distributions=["last_letter_different", "uniform", "normal", "powerlaw"]
querytypes=["similar", "random", "last_letter", "common_prefix"]
dir="100k_26_15_3__2_100"
results_dir = "range-filtering-results/100k_26_15_3__2_100__ch_5_all"
workload_dir = "workload-gen/range_queries_workloads/%s/" % dir

if not os.path.exists(results_dir):
    os.makedirs(results_dir)

# Parameters
ST_cutoff_min = 0.8
ST_cutoff_max = 1.0
ST_cutoff_interval = 0.05
SPL_restraint_min = 0.0
SPL_restraint_max = 1.0
SPL_restraint_interval = 0.1
SPL_abs_restraint_min = 0
SPL_abs_restraint_max = 15
SPL_abs_restraint_interval = 1

bf_size_min = 500000
bf_size_max = 1000000
bf_interval = 100000
bf_penalty = 0.5

RBF_size_min=100000
RBF_size_max=4000000
RBF_size_step=500000

test_type="all"

for dist in distributions:
    for qt in querytypes:
        print("Running bench for distribution = %s and query type = %s" % (dist, qt))

        if test_type == "cut_off_rel":
            df_splash_0_7 = run_splash(dist, qt, workload_dir, 0.7, 0.7, 0.05, "relative", SPL_restraint_min, SPL_restraint_max, SPL_restraint_interval)
            df_splash_0_8 = run_splash(dist, qt, workload_dir, 0.8, 0.8, 0.05, "relative", SPL_restraint_min, SPL_restraint_max, SPL_restraint_interval)
            df_splash_0_9 = run_splash(dist, qt, workload_dir, 0.9, 0.9, 0.05, "relative", SPL_restraint_min, SPL_restraint_max, SPL_restraint_interval)
            df_splash_1_0 = run_splash(dist, qt, workload_dir, 1.0, 1.0, 0.05, "relative", SPL_restraint_min, SPL_restraint_max, SPL_restraint_interval)
            df_surf_real = run_SuRFReal_bench(dist, qt, workload_dir, 0, 0)
            df_fst = run_FST_bench(dist, qt, workload_dir)

            print(df_splash_0_7)
            print(df_splash_0_8)
            print(df_splash_0_9)
            print(df_splash_1_0)

            df_splash_0_7.rename(columns={'FPR': 'Splash y=0.7'}, inplace=True)
            df_splash_0_8.rename(columns={'FPR': 'Splash y=0.8'}, inplace=True)
            df_splash_0_9.rename(columns={'FPR': 'Splash y=0.9'}, inplace=True)
            df_splash_1_0.rename(columns={'FPR': 'Splash y=1.0'}, inplace=True)
            df_surf_real.rename(columns={'FPR': 'SuRF Real'}, inplace=True)
            df_fst.rename(columns={'FPR': 'FST'}, inplace=True)
            
            print("Creating DF with stats")
            dfs = [df_splash_0_7[['Memory usage', 'Splash y=0.7']],
                df_splash_0_8[['Memory usage', 'Splash y=0.8']],
                df_splash_0_9[['Memory usage', 'Splash y=0.9']],
                df_splash_1_0[['Memory usage', 'Splash y=1.0']],
                df_surf_real[['Memory usage', 'SuRF Real']],
                df_fst[['Memory usage', 'FST']]]

        elif test_type == "cut_off_abs":
            df_splash_0_7 = run_splash(dist, qt, workload_dir, 0.7, 0.7, 0.05, "absolute", SPL_abs_restraint_min, SPL_abs_restraint_max, SPL_abs_restraint_interval)
            df_splash_0_8 = run_splash(dist, qt, workload_dir, 0.8, 0.8, 0.05, "absolute", SPL_abs_restraint_min, SPL_abs_restraint_max, SPL_abs_restraint_interval)
            df_splash_0_9 = run_splash(dist, qt, workload_dir, 0.9, 0.9, 0.05, "absolute", SPL_abs_restraint_min, SPL_abs_restraint_max, SPL_abs_restraint_interval)
            df_splash_1_0 = run_splash(dist, qt, workload_dir, 1.0, 1.0, 0.05, "absolute", SPL_abs_restraint_min, SPL_abs_restraint_max, SPL_abs_restraint_interval)
            df_surf_real = run_SuRFReal_bench(dist, qt, workload_dir, 0, 0)
            df_fst = run_FST_bench(dist, qt, workload_dir)

            print(df_splash_0_7)

            df_splash_0_7.rename(columns={'FPR': 'Splash y=0.7'}, inplace=True)
            df_splash_0_8.rename(columns={'FPR': 'Splash y=0.8'}, inplace=True)
            df_splash_0_9.rename(columns={'FPR': 'Splash y=0.9'}, inplace=True)
            df_splash_1_0.rename(columns={'FPR': 'Splash y=1.0'}, inplace=True)
            df_surf_real.rename(columns={'FPR': 'SuRF'}, inplace=True)
            df_fst.rename(columns={'FPR': 'FST'}, inplace=True)
            
            print("Creating DF with stats")
            dfs = [df_splash_0_7[['Memory usage', 'Splash y=0.7']],
                df_splash_0_8[['Memory usage', 'Splash y=0.8']],
                df_splash_0_9[['Memory usage', 'Splash y=0.9']],
                df_splash_1_0[['Memory usage', 'Splash y=1.0']],
                df_surf_real[['Memory usage', 'SuRF']],
                df_fst[['Memory usage', 'FST']]]
        
        elif test_type == "bloomed_splash":
            df_bf = run_rangeBF_bench(dist, qt, workload_dir, RBF_size_min, RBF_size_max, RBF_size_step)
            df_splash_5 = run_bloomedRangeSplash_bench(dist, qt, workload_dir, bf_size_min, bf_size_max, bf_interval, 5)
            df_splash_6 = run_bloomedRangeSplash_bench(dist, qt, workload_dir, bf_size_min, bf_size_max, bf_interval, 6)
            df_splash_7 = run_bloomedRangeSplash_bench(dist, qt, workload_dir, bf_size_min, bf_size_max, bf_interval, 7)
            df_splash_8 = run_bloomedRangeSplash_bench(dist, qt, workload_dir, bf_size_min, bf_size_max, bf_interval, 8)
            df_surf_real = run_SuRFReal_bench(dist, qt, workload_dir, 0, 0)
            df_fst = run_FST_bench(dist, qt, workload_dir)

            df_bf.rename(columns={'FPR': 'Range Bloom Filter'}, inplace=True)
            df_splash_5.rename(columns={'FPR': 'Bloomed Splash height=5'}, inplace=True)
            df_splash_6.rename(columns={'FPR': 'Bloomed Splash height=6'}, inplace=True)
            df_splash_7.rename(columns={'FPR': 'Bloomed Splash height=7'}, inplace=True)
            df_splash_8.rename(columns={'FPR': 'Bloomed Splash height=8'}, inplace=True)
            df_surf_real.rename(columns={'FPR': 'SuRF'}, inplace=True)
            df_fst.rename(columns={'FPR': 'FST'}, inplace=True)
            
            print(df_splash_5)
            print(df_splash_6)
            print(df_splash_7)

            print("Creating DF with stats")
            dfs = [df_bf[['Memory usage', 'Range Bloom Filter']], 
                df_splash_5[['Memory usage', 'Bloomed Splash height=5']],
                df_splash_6[['Memory usage', 'Bloomed Splash height=6']],
                df_splash_7[['Memory usage', 'Bloomed Splash height=7']],
                df_splash_8[['Memory usage', 'Bloomed Splash height=8']],
                df_surf_real[['Memory usage', 'SuRF']],
                df_fst[['Memory usage', 'FST']]]
        else:
            df_splash_rel = run_splash(dist, qt, workload_dir, 0.8, 0.8, 0.05, "relative", SPL_restraint_min, SPL_restraint_max, SPL_restraint_interval)
            df_splash_abs = run_splash(dist, qt, workload_dir, 0.8, 0.8, 0.05, "absolute", SPL_abs_restraint_min, SPL_abs_restraint_max, SPL_abs_restraint_interval)
            df_splash_7 = run_bloomedRangeSplash_bench(dist, qt, workload_dir, bf_size_min, bf_size_max, bf_interval, 7)
            df_surf_base = run_SuRFReal_bench(dist, qt, workload_dir, 0, 0)
            df_surf_real = run_SuRFReal_bench(dist, qt, workload_dir, 0, 8)
            df_bf = run_rangeBF_bench(dist, qt, workload_dir, RBF_size_min, RBF_size_max, RBF_size_step)
            df_fst = run_FST_bench(dist, qt, workload_dir)

            df_splash_rel.rename(columns={'FPR': 'Splash Rel. cut-off=0.8'}, inplace=True)
            df_splash_abs.rename(columns={'FPR': 'Splash Abs. cut-off=0.8'}, inplace=True)
            df_splash_7.rename(columns={'FPR': 'Bloomed FST height=7'}, inplace=True)
            df_surf_base.rename(columns={'FPR': 'SuRF Base'}, inplace=True)
            df_surf_real.rename(columns={'FPR': 'SuRF Real'}, inplace=True)
            df_bf.rename(columns={'FPR': 'Range Bloom Filter'}, inplace=True)
            df_fst.rename(columns={'FPR': 'FST'}, inplace=True)
            
            print("Creating DF with stats")
            dfs = [df_splash_rel[['Memory usage', 'Splash Rel. cut-off=0.8']],
                df_splash_abs[['Memory usage', 'Splash Abs. cut-off=0.8']],
                df_splash_7[['Memory usage', 'Bloomed FST height=7']],
                df_bf[['Memory usage', 'Range Bloom Filter']], 
                df_surf_real[['Memory usage', 'SuRF Real']],
                df_surf_base[['Memory usage', 'SuRF Base']],
                df_fst[['Memory usage', 'FST']]]

        df = pd.concat(dfs)

        with open(results_dir + "/" + dist + "_" + qt + ".txt", "w") as f:
            df.to_csv(f)