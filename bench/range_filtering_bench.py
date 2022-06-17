import subprocess
import pandas as pd
import os

def run_SuRFReal_bench(dist, qt, workload_dir, SR_suffix_size_min, SR_suffix_size_max):
    cmd = ["../build/bench/range_filtering_bench", str(dist), str(qt), str(workload_dir), "surfreal", str(SR_suffix_size_min), str(SR_suffix_size_max)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

def run_Rosetta_bench(dist, qt, workload_dir, RST_size_min, RST_size_max, RST_size_step):
    cmd = ["../build/bench/range_filtering_bench", str(dist), str(qt), str(workload_dir), "rosetta", str(RST_size_min), str(RST_size_max), str(RST_size_step)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

def run_LilRosetta_bench(dist, qt, workload_dir, RST_size_min, RST_size_max, RST_size_step, RST_penalty):
    cmd = ["../build/bench/range_filtering_bench", str(dist), str(qt), str(workload_dir), "lilrosetta", str(RST_size_min), str(RST_size_max), str(RST_size_step), str(RST_penalty)]
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

def run_bloomedchareq_bench(dist, qt, workload_dir, CHRQ_sat, RBF_size_min, RBF_size_max, RBF_size_step):
    cmd = ["../build/bench/range_filtering_bench", str(dist), str(qt), str(workload_dir), "bloomedchareq", str(CHRQ_sat), str(RBF_size_min), str(RBF_size_max), str(RBF_size_step)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

def run_oldchareq_bench(dist, qt, workload_dir, CHRQ_frac_min, CHRQ_frac_max, CHRQ_frac_step, CHRQ_top_layer_height):
    cmd = ["../build/bench/range_filtering_bench", str(dist), str(qt), str(workload_dir), "old_chareq", str(CHRQ_top_layer_height), str(CHRQ_frac_min), str(CHRQ_frac_max), str(CHRQ_frac_step)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

def run_layeredchareq_bench(dist, qt, workload_dir, LCHRQ_first_lay_height, LCHRQ_first_lay_sat, LCHRQ_second_lay_sat_min, LCHRQ_second_lay_sat_max, LCHRQ_second_lay_sat_step, LCHRQ_first_lay_culled_bits, LCHRQ_second_lay_culled_bits):
    cmd = ["../build/bench/range_filtering_bench", str(dist), str(qt), str(workload_dir), "layeredchareq", str(LCHRQ_first_lay_height), str(LCHRQ_first_lay_sat), str(LCHRQ_second_lay_sat_min), str(LCHRQ_second_lay_sat_max), str(LCHRQ_second_lay_sat_step), str(LCHRQ_first_lay_culled_bits), str(LCHRQ_second_lay_culled_bits)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

def run_rechareq_bench(dist, qt, workload_dir, CHRQ_frac_min, CHRQ_frac_max, CHRQ_frac_step, CHRQ_top_layer_height, CHRQ_bits_per_char):
    cmd = ["../build/bench/range_filtering_bench", str(dist), str(qt), str(workload_dir), "rechareq", str(CHRQ_top_layer_height), str(CHRQ_frac_min), str(CHRQ_frac_max), str(CHRQ_frac_step), str(CHRQ_bits_per_char)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

def run_splash(dist, qt, workload_dir, cut_off_min, cut_off_max, cut_off_interval, constraint_type, constraint_min, constraint_max, constraint_interval):
    cmd = ["../build/bench/range_filtering_bench", str(dist), str(qt), str(workload_dir), "splash", str(cut_off_min), str(cut_off_max), str(cut_off_interval), str(constraint_type), 
    str(constraint_min), str(constraint_max), str(constraint_interval)]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return pd.read_csv(p.stdout, delimiter="\t")

distributions=["last_letter_different", "uniform", "normal", "powerlaw"]

querytypes=["similar", "random", "last_letter", "common_prefix"]
dir="10k_90_15_0__8_16"
results_dir = "range-filtering-results/10k_90_15_0__8_16_chareq_equal_fixed"
workload_dir = "workload-gen/range_queries_workloads/%s/" % dir

if not os.path.exists(results_dir):
    os.makedirs(results_dir)

# Parameters
SR_suffix_size_min=0
SR_suffix_size_max=0
RST_size_min=30000000
RST_size_max=100000000
RST_size_step=10000000
LRST_size_min=2000000
LRST_size_max=7000000
LRST_size_step=1000000
SPL_cutoff=0.75
SPL_restraint_min=0.0
SPL_restraint_max=1.0
SPL_restraint_interval=0.05
RBF_size_min=100000
RBF_size_max=800000
RBF_size_step=100000
CHRQ_frac_min=0.3
CHRQ_frac_max=0.81
CHRQ_frac_step=0.1
CHRQ_top_layer_height=1
SPL_restraint_min = 0.0
SPL_restraint_max = 1.0
SPL_restraint_interval = 0.1
SPL_abs_restraint_min = 0
SPL_abs_restraint_max = 15
SPL_abs_restraint_interval = 1

equal_lengths = False

if equal_lengths:
    distributions = ["normal", "equal"]

for dist in distributions:
    for qt in querytypes:
        print("Running bench for distribution = %s and query type = %s" % (dist, qt))
        #df_rosetta = run_Rosetta_bench(dist, qt, workload_dir, RST_size_min, RST_size_max, RST_size_step)
        # df_lilrosetta_0_2 = run_LilRosetta_bench(dist, qt, workload_dir, LRST_size_min, LRST_size_max, LRST_size_step, 0.2)
        # df_lilrosetta_0_7 = run_LilRosetta_bench(dist, qt, workload_dir, LRST_size_min, LRST_size_max, LRST_size_step, 0.7)
        df_lilrosetta_0_8 = run_LilRosetta_bench(dist, qt, workload_dir, LRST_size_min, LRST_size_max, LRST_size_step, 0.8)
        # df_lilrosetta_0_9 = run_LilRosetta_bench(dist, qt, workload_dir, LRST_size_min, LRST_size_max, LRST_size_step, 0.9)
        df_rangeBF = run_rangeBF_bench(dist, qt, workload_dir, RBF_size_min, RBF_size_max, RBF_size_step)
        #df_rangeKRBF = run_rangeKRBF_bench(dist, qt, workload_dir, RBF_size_min, RBF_size_max, RBF_size_step)
        # df_bloomedSplash3 = run_bloomedRangeSplash_bench(dist, qt, workload_dir, RBF_size_min, RBF_size_max, RBF_size_step, 3)
        # df_bloomedSplash5 = run_bloomedRangeSplash_bench(dist, qt, workload_dir, RBF_size_min, RBF_size_max, RBF_size_step, 5)
        # df_bloomedSplash7 = run_bloomedRangeSplash_bench(dist, qt, workload_dir, RBF_size_min, RBF_size_max, RBF_size_step, 7)
        # df_surf = run_SuRFReal_bench(dist, qt, workload_dir, SR_suffix_size_min, SR_suffix_size_max)
        #df_splash = run_Splash_bench(dist, qt, workload_dir, SPL_cutoff, SPL_restraint_min, SPL_restraint_max, SPL_restraint_interval)
        df_splash_rel = run_splash(dist, qt, workload_dir, 0.8, 0.8, 0.05, "relative", SPL_restraint_min, SPL_restraint_max, SPL_restraint_interval)
        df_splash_abs = run_splash(dist, qt, workload_dir, 0.8, 0.8, 0.05, "absolute", SPL_abs_restraint_min, SPL_abs_restraint_max, SPL_abs_restraint_interval)
        #df_chareq = run_chareq_bench(dist, qt, workload_dir, CHRQ_frac_min, CHRQ_frac_max, CHRQ_frac_step, CHRQ_top_layer_height)
        #df_bloomed_chareq_0_5 = run_bloomedchareq_bench(dist, qt, workload_dir, 0.5, RBF_size_min, RBF_size_max, RBF_size_step)
        #df_bloomed_chareq_0_7 = run_bloomedchareq_bench(dist, qt, workload_dir, 0.7, RBF_size_min, RBF_size_max, RBF_size_step)
        # df_layered_chareq_14_0_8_1_0 = run_layeredchareq_bench(dist, qt, workload_dir, 14, 0.8, CHRQ_frac_min, CHRQ_frac_max, CHRQ_frac_step, 1, 0)
        df_layered_chareq_14_0_9_3_0 = run_layeredchareq_bench(dist, qt, workload_dir, 14, 0.9, CHRQ_frac_min, CHRQ_frac_max, CHRQ_frac_step, 3, 0)
        df_layered_chareq_14_0_9_4_0 = run_layeredchareq_bench(dist, qt, workload_dir, 14, 0.9, CHRQ_frac_min, CHRQ_frac_max, CHRQ_frac_step, 4, 0)
        df_layered_chareq_14_0_9_5_0 = run_layeredchareq_bench(dist, qt, workload_dir, 14, 0.9, CHRQ_frac_min, CHRQ_frac_max, CHRQ_frac_step, 5, 0)
        df_layered_chareq_14_0_9_6_0 = run_layeredchareq_bench(dist, qt, workload_dir, 14, 0.9, CHRQ_frac_min, CHRQ_frac_max, CHRQ_frac_step, 6, 0)
        # df_layered_chareq_4_1 = run_layeredchareq_bench(dist, qt, workload_dir, 4, 0.2, CHRQ_frac_min, CHRQ_frac_max, CHRQ_frac_step, 1)
        # df_layered_chareq_4_2 = run_layeredchareq_bench(dist, qt, workload_dir, 4, 0.2, CHRQ_frac_min, CHRQ_frac_max, CHRQ_frac_step, 2)
        # df_layered_chareq_6_0 = run_layeredchareq_bench(dist, qt, workload_dir, 6, 0.2, CHRQ_frac_min, CHRQ_frac_max, CHRQ_frac_step, 0)
        # df_layered_chareq_6_1 = run_layeredchareq_bench(dist, qt, workload_dir, 6, 0.2, CHRQ_frac_min, CHRQ_frac_max, CHRQ_frac_step, 1)
        # df_layered_chareq_6_2 = run_layeredchareq_bench(dist, qt, workload_dir, 6, 0.2, CHRQ_frac_min, CHRQ_frac_max, CHRQ_frac_step, 2)
        # df_oldchareq = run_oldchareq_bench(dist, qt, workload_dir, CHRQ_frac_min, CHRQ_frac_max, CHRQ_frac_step, CHRQ_top_layer_height)
        # df_rechareq_3 = run_rechareq_bench(dist, qt, workload_dir, CHRQ_frac_min, CHRQ_frac_max, CHRQ_frac_step, CHRQ_top_layer_height, 3)
        # df_rechareq_4 = run_rechareq_bench(dist, qt, workload_dir, CHRQ_frac_min, CHRQ_frac_max, CHRQ_frac_step, CHRQ_top_layer_height, 4)
        # df_rechareq_5 = run_rechareq_bench(dist, qt, workload_dir, CHRQ_frac_min, CHRQ_frac_max, CHRQ_frac_step, CHRQ_top_layer_height, 5)
        df_fst = run_FST_bench(dist, qt, workload_dir)
        df_surf_base = run_SuRFReal_bench(dist, qt, workload_dir, 0, 0)
        df_surf_real = run_SuRFReal_bench(dist, qt, workload_dir, 0, 8)

        #print(df_rosetta)
        print(df_rangeBF)
        # print(df_chareq)
        # print(df_bloomedSplash3)
        # print(df_bloomedSplash5)
        # print(df_bloomedSplash7)
        #print(df_surf)
        #print(df_splash_rel)
        #print(df_splash_abs)
        #print(df_chareq)
        # print(df_oldchareq)
        
        #print(df_bloomed_chareq_0_5)
        #print(df_bloomed_chareq_0_7)
        #print(df_layered_chareq_14_0_8_1_0)
        # print(df_layered_chareq_14_0_9_3_0)
        # print(df_layered_chareq_4_2)
        # print(df_layered_chareq_6_0)
        # print(df_layered_chareq_6_1)
        # print(df_layered_chareq_6_2)
        print(df_fst)

        # df_rosetta.rename(columns={'FPR': 'Rosetta'}, inplace=True)
        # df_lilrosetta_0_2.rename(columns={'FPR': 'LilRosetta p=0.2'}, inplace=True)
        # df_lilrosetta_0_7.rename(columns={'FPR': 'LilRosetta p=0.7'}, inplace=True)
        df_lilrosetta_0_8.rename(columns={'FPR': 'LilRosetta p=0.8'}, inplace=True)
        # df_lilrosetta_0_9.rename(columns={'FPR': 'LilRosetta p=0.9'}, inplace=True)
        df_rangeBF.rename(columns={'FPR': 'RangeBF'}, inplace=True)
        #df_rangeKRBF.rename(columns={'FPR': 'Range KR-BF'}, inplace=True)
        # df_bloomedSplash3.rename(columns={'FPR': 'BloomedSplash h=3'}, inplace=True)
        # df_bloomedSplash5.rename(columns={'FPR': 'BloomedSplash h=5'}, inplace=True)
        # df_bloomedSplash7.rename(columns={'FPR': 'BloomedSplash h=7'}, inplace=True)
        #df_surf.rename(columns={'FPR': 'SuRF Real'}, inplace=True)
        df_splash_rel.rename(columns={'FPR': 'Splash Rel. cut-off=0.8'}, inplace=True)
        df_splash_abs.rename(columns={'FPR': 'Splash Abs. cut-off=0.8'}, inplace=True)
        # df_chareq.rename(columns={'FPR': 'CHaREQ'}, inplace=True)
        #df_bloomed_chareq_0_5.rename(columns={'FPR': 'Bloomed CHaREQ sat.=0.5'}, inplace=True)
        #df_bloomed_chareq_0_7.rename(columns={'FPR': 'Bloomed CHaREQ sat.=0.7'}, inplace=True)
        df_layered_chareq_14_0_9_3_0.rename(columns={'FPR': 'Layered CHaREQ h=14 b=3'}, inplace=True)
        df_layered_chareq_14_0_9_4_0.rename(columns={'FPR': 'Layered CHaREQ h=14 b=4'}, inplace=True)
        df_layered_chareq_14_0_9_5_0.rename(columns={'FPR': 'Layered CHaREQ h=14 b=5'}, inplace=True)
        df_layered_chareq_14_0_9_6_0.rename(columns={'FPR': 'Layered CHaREQ h=14 b=6'}, inplace=True)
        # df_layered_chareq_4_2.rename(columns={'FPR': 'Layered CHaREQ h=4 b=2'}, inplace=True)
        # df_layered_chareq_6_0.rename(columns={'FPR': 'Layered CHaREQ h=6 b=0'}, inplace=True)
        # df_layered_chareq_6_1.rename(columns={'FPR': 'Layered CHaREQ h=6 b=1'}, inplace=True)
        # df_layered_chareq_6_2.rename(columns={'FPR': 'Layered CHaREQ h=6 b=2'}, inplace=True)
        # df_oldchareq.rename(columns={'FPR': 'Old CHaREQ'}, inplace=True)
        # df_rechareq_3.rename(columns={'FPR': 'ReCHaREQ b=3'}, inplace=True)
        # df_rechareq_4.rename(columns={'FPR': 'ReCHaREQ b=4'}, inplace=True)
        # df_rechareq_5.rename(columns={'FPR': 'ReCHaREQ b=5'}, inplace=True)
        df_fst.rename(columns={'FPR': 'FST'}, inplace=True)
        df_surf_base.rename(columns={'FPR': 'SuRF Base'}, inplace=True)
        df_surf_real.rename(columns={'FPR': 'SuRF Real'}, inplace=True)

        print("Creating DF with stats")
        dfs = [df_rangeBF[['Memory usage', 'RangeBF']],
               #df_rangeKRBF[['Memory usage', 'Range KR-BF']]
            #df_rosetta[['Memory usage', 'Rosetta']],
            # df_lilrosetta_0_2[['Memory usage', 'LilRosetta p=0.2']],
            # df_lilrosetta_0_7[['Memory usage', 'LilRosetta p=0.7']],
            df_lilrosetta_0_8[['Memory usage', 'LilRosetta p=0.8']],
            # df_lilrosetta_0_9[['Memory usage', 'LilRosetta p=0.9']],
            # df_bloomedSplash3[['Memory usage', 'BloomedSplash h=3']],
            # df_bloomedSplash5[['Memory usage', 'BloomedSplash h=5']],
            # df_bloomedSplash7[['Memory usage', 'BloomedSplash h=7']],
            #df_lilrosetta[['Memory usage', 'LilRosetta']],
            df_splash_rel[['Memory usage', 'Splash Rel. cut-off=0.8']],
            df_splash_abs[['Memory usage', 'Splash Abs. cut-off=0.8']],
            # df_surf[['Memory usage', 'SuRF Real']],
            #df_chareq[['Memory usage', 'CHaREQ']],
            #df_bloomed_chareq_0_5[['Memory usage', 'Bloomed CHaREQ sat.=0.5']],
            #df_bloomed_chareq_0_7[['Memory usage', 'Bloomed CHaREQ sat.=0.7']],
            df_layered_chareq_14_0_9_3_0[['Memory usage', 'Layered CHaREQ h=14 b=3']],
            df_layered_chareq_14_0_9_4_0[['Memory usage', 'Layered CHaREQ h=14 b=4']],
            df_layered_chareq_14_0_9_5_0[['Memory usage', 'Layered CHaREQ h=14 b=5']],
            df_layered_chareq_14_0_9_6_0[['Memory usage', 'Layered CHaREQ h=14 b=6']],
            # df_layered_chareq_4_2[['Memory usage', 'Layered CHaREQ h=4 b=2']],
            # df_layered_chareq_6_0[['Memory usage', 'Layered CHaREQ h=6 b=0']],
            # df_layered_chareq_6_1[['Memory usage', 'Layered CHaREQ h=6 b=1']],
            # df_layered_chareq_6_2[['Memory usage', 'Layered CHaREQ h=6 b=2']],
            # df_oldchareq[['Memory usage', 'Old CHaREQ']],
            # df_rechareq_3[['Memory usage', 'ReCHaREQ b=3']],
            # df_rechareq_4[['Memory usage', 'ReCHaREQ b=4']],
            # df_rechareq_5[['Memory usage', 'ReCHaREQ b=5']],
            df_surf_real[['Memory usage', 'SuRF Real']],
            df_surf_base[['Memory usage', 'SuRF Base']],
            df_fst[['Memory usage', 'FST']]]

        df = pd.concat(dfs)

        with open(results_dir + "/" + dist + "_" + qt + ".txt", "w") as f:
            df.to_csv(f)