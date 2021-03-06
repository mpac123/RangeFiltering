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

distributions=["last_letter_different", "uniform", "normal", "powerlaw", "increasing_degree"]
querytypes=["similar", "random", "last_letter", "common_prefix"]
dir="100k_26_15_3__2_100"
results_dir = "range-filtering-results/100k_26_15_3__surf_prefix_workloads"
workload_dir = "workload-gen/range_queries_workloads/%s/" % dir

if not os.path.exists(results_dir):
    os.makedirs(results_dir)

# Parameters
SR_suffix_size_min=0
SR_suffix_size_max=8
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
RBF_size_min=100000
RBF_size_max=4000000
RBF_size_step=500000
CHRQ_frac_min=0.5
CHRQ_frac_max=0.96
CHRQ_frac_step=0.05
CHRQ_top_layer_height=1

for dist in distributions:
    for qt in querytypes:
        print("Running bench for distribution = %s and query type = %s" % (dist, qt))
        #df_rosetta = run_Rosetta_bench(dist, qt, workload_dir, RST_size_min, RST_size_max, RST_size_step)
        #df_lilrosetta = run_LilRosetta_bench(dist, qt, workload_dir, LRST_size_min, LRST_size_max, LRST_size_step)
        df_rangeBF = run_rangeBF_bench(dist, qt, workload_dir, RBF_size_min, RBF_size_max, RBF_size_step)
        #df_rangeKRBF = run_rangeKRBF_bench(dist, qt, workload_dir, RBF_size_min, RBF_size_max, RBF_size_step)
        # df_bloomedSplash3 = run_bloomedRangeSplash_bench(dist, qt, workload_dir, RBF_size_min, RBF_size_max, RBF_size_step, 3)
        # df_bloomedSplash5 = run_bloomedRangeSplash_bench(dist, qt, workload_dir, RBF_size_min, RBF_size_max, RBF_size_step, 5)
        # df_bloomedSplash7 = run_bloomedRangeSplash_bench(dist, qt, workload_dir, RBF_size_min, RBF_size_max, RBF_size_step, 7)
        df_surf = run_SuRFBase_bench(dist, qt, workload_dir, SR_suffix_size_min, SR_suffix_size_max)
        df_surf_real = run_SuRFReal_bench(dist, qt, workload_dir, SR_suffix_size_min, SR_suffix_size_max)
        df_surf_hash = run_SuRFHash_bench(dist, qt, workload_dir, SR_suffix_size_min, SR_suffix_size_max)
        df_surf_mixed = run_SuRFMixed_bench(dist, qt, workload_dir, SR_suffix_size_min, SR_suffix_size_max)
        #df_splash = run_Splash_bench(dist, qt, workload_dir, SPL_cutoff, SPL_restraint_min, SPL_restraint_max, SPL_restraint_interval)
        #df_chareq = run_chareq_bench(dist, qt, workload_dir, CHRQ_frac_min, CHRQ_frac_max, CHRQ_frac_step, CHRQ_top_layer_height)
        df_fst = run_FST_bench(dist, qt, workload_dir)

        #print(df_rosetta)
        print(df_rangeBF)
        # print(df_bloomedSplash3)
        # print(df_bloomedSplash5)
        # print(df_bloomedSplash7)
        print(df_surf)
        print(df_surf_real)
        print(df_surf_hash)
        print(df_surf_mixed)
        #print(df_splash)
        #print(df_chareq)
        print(df_fst)

        #df_rosetta.rename(columns={'FPR': 'Rosetta'}, inplace=True)
        #df_lilrosetta.rename(columns={'FPR': 'LilRosetta'}, inplace=True)
        df_rangeBF.rename(columns={'FPR': 'RangeBF'}, inplace=True)
        # df_bloomedSplash3.rename(columns={'FPR': 'BloomedSplash h=3'}, inplace=True)
        # df_bloomedSplash5.rename(columns={'FPR': 'BloomedSplash h=5'}, inplace=True)
        # df_bloomedSplash7.rename(columns={'FPR': 'BloomedSplash h=7'}, inplace=True)
        df_surf.rename(columns={'FPR': 'SuRF Base'}, inplace=True)
        df_surf_real.rename(columns={'FPR': 'SuRF Real'}, inplace=True)
        df_surf_hash.rename(columns={'FPR': 'SuRF Hash'}, inplace=True)
        df_surf_mixed.rename(columns={'FPR': 'SuRF Mixed'}, inplace=True)
        df_fst.rename(columns={'FPR': 'FST'}, inplace=True)

        print("Creating DF with stats")
        dfs = [df_rangeBF[['Memory usage', 'RangeBF']],
            # df_bloomedSplash3[['Memory usage', 'BloomedSplash h=3']],
            # df_bloomedSplash5[['Memory usage', 'BloomedSplash h=5']],
            # df_bloomedSplash7[['Memory usage', 'BloomedSplash h=7']],
            #df_lilrosetta[['Memory usage', 'LilRosetta']],
            df_surf_real[['Memory usage', 'SuRF Real']],
            df_surf_hash[['Memory usage', 'SuRF Hash']],
            df_surf_mixed[['Memory usage', 'SuRF Mixed']],
            df_surf[['Memory usage', 'SuRF Base']],
            df_fst[['Memory usage', 'FST']]]

        df = pd.concat(dfs)

        with open(results_dir + "/" + dist + "_" + qt + ".txt", "w") as f:
            df.to_csv(f)