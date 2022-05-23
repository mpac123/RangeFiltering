#include "bench.h"
#include "range_filtering_bench.hpp"

int main(int argc, char *argv[]) {
    std::string data_type = "normal";
    std::string query_type = "random";
    std::string input_dir = "/home/mapac/Coding/RangeFiltering/bench/workload-gen/range_queries_workloads/100k_26_15_3__2_100/";

    std::string data_structure = "chareq";

    std::tuple<uint32_t, uint32_t> surf_params = {0, 0};
    uint32_t surfdense_suffix_len = 0;

    uint64_t rosetta_size_min = 50000000;
    uint64_t rosetta_size_max = 100000000;
    uint64_t rosetta_size_step = 10000000;

    uint64_t rangeBF_size_min = 500000;
    uint64_t rangeBF_size_max = 1000000;
    uint64_t rangeBF_size_step = 100000;

    uint64_t fst_height = 5;

    double splash_cutoff = 0.75;
    double splash_restraint_val_min = 1.0;
    double splash_restraint_val_max = 1.0;
    double splash_restraint_val_interval = 0.0;

    uint32_t chareq_top_layer_height = 1;
    float chareq_filled_in_fraction_min = 0.2;
    float chareq_filled_in_fraction_max = 0.9;
    float chareq_filled_in_fraction_step = 0.1;
    uint32_t rechareq_bits_per_char = 4;

    range_filtering_splash::SplashRestraintType restraintType = range_filtering_splash::SplashRestraintType::relative;

    double cutoff_min = 1.0;
    double cutoff_max = 1.0;
    double cutoff_interval = 0.05;
    uint64_t absolute_restraint_value_min = 0;
    uint64_t absolute_restraint_value_max = 0;
    uint64_t absolute_restraint_interval = 1.0;
    double relative_restraint_value_min = 0.0;
    double relative_restraint_value_max = 1.0;
    double relative_restraint_interval = 0.1;

    if (argc > 4) {
        data_type = argv[1];
        query_type = argv[2];
        input_dir = argv[3];
        data_structure = argv[4];
    }

    if ((data_structure == "surf" || data_structure == "surfreal"
            || data_structure == "surfhash" || data_structure == "surfmixed")
        && argc > 6) {
        surf_params = {std::stoi(argv[5]), std::stoi(argv[6])};
    }

    if (data_structure == "surfdense" and argc > 5) {
        surfdense_suffix_len = std::stoi(argv[5]);
    }

    if ((data_structure == "rosetta" || data_structure == "lilrosetta") && argc > 7) {
        rosetta_size_min = std::stoi(argv[5]);
        rosetta_size_max = std::stoi(argv[6]);
        rosetta_size_step = std::stoi(argv[7]);
    }

//    if (data_structure == "splash" && argc > 8) {
//        splash_cutoff = std::stod(argv[5]);
//        splash_restraint_val_min = std::stod(argv[6]);
//        splash_restraint_val_max = std::stod(argv[7]);
//        splash_restraint_val_interval = std::stod(argv[8]);
//    }

    if ((data_structure == "rangeBF" || data_structure == "rangeKRBF") && argc > 7) {
        rangeBF_size_min = std::stoi(argv[5]);
        rangeBF_size_max = std::stoi(argv[6]);
        rangeBF_size_step = std::stoi(argv[7]);
    }

    if (data_structure == "bloomedsplash" && argc > 8) {
        rangeBF_size_min = std::stoi(argv[5]);
        rangeBF_size_max = std::stoi(argv[6]);
        rangeBF_size_step = std::stoi(argv[7]);
        fst_height = std::stoi(argv[8]);
    }

    if (data_structure == "chareq" && argc > 8) {
        chareq_top_layer_height = std::stoi(argv[5]);
        chareq_filled_in_fraction_min = std::stod(argv[6]);
        chareq_filled_in_fraction_max = std::stod(argv[7]);
        chareq_filled_in_fraction_step = std::stod(argv[8]);
    }

    if (data_structure == "rechareq" && argc > 9) {
        chareq_top_layer_height = std::stoi(argv[5]);
        chareq_filled_in_fraction_min = std::stod(argv[6]);
        chareq_filled_in_fraction_max = std::stod(argv[7]);
        chareq_filled_in_fraction_step = std::stod(argv[8]);
        rechareq_bits_per_char = std::stoi(argv[9]);
    }

    if (data_structure == "splash" && argc > 11) {
        cutoff_min = std::stod(argv[5]);
        cutoff_max = std::stod(argv[6]);
        cutoff_interval = std::stod(argv[7]);
        if (strcmp(argv[8], "relative") == 0) {
            restraintType = range_filtering_splash::SplashRestraintType::relative;
            relative_restraint_value_min = std::stod(argv[9]);
            relative_restraint_value_max = std::stod(argv[10]);
            relative_restraint_interval = std::stod(argv[11]);
        } else if (strcmp(argv[8], "absolute") == 0) {
            restraintType = range_filtering_splash::SplashRestraintType::absolute;
            absolute_restraint_value_min = std::stoi(argv[9]);
            absolute_restraint_value_max = std::stoi(argv[10]);
            absolute_restraint_interval = std::stoi(argv[11]);
        }
    }

    std::string input_filename = input_dir + data_type + "_input.txt";
    std::string query_filename = input_dir + data_type + "_queries_" + query_type + ".txt";

    std::vector<std::string> keys;
    bench::loadKeysFromFile(input_filename, keys);

    std::vector<std::pair<std::string, std::string>> ranges;
    bench::loadRangeQueriesFromFile(query_filename, ranges);

    if (keys.empty() || ranges.empty()) {
        std::cout << "Didn't read files: " << input_filename << " and " << query_filename << std::endl;
        return -1;
    }

    std::cout << "Memory usage\tFPR\tSuffix size\tCreation time\tQuery time\tTrie memory usage" << std::endl;

    if (data_structure == "surf") {
        range_filtering_bench::runTestsSuRFReal(0, 0, keys, ranges);
    } else if (data_structure == "surfreal") {
        range_filtering_bench::runTestsSuRFReal(std::get<0>(surf_params), std::get<1>(surf_params), keys, ranges);
    } else if (data_structure == "surfhash") {
        range_filtering_bench::runTestsSuRFHash(std::get<0>(surf_params), std::get<1>(surf_params), keys, ranges);
    } else if (data_structure == "surfmixed") {
        range_filtering_bench::runTestsSuRFMixed(std::get<0>(surf_params), std::get<1>(surf_params), keys, ranges);
    } else if (data_structure == "rosetta") {
        range_filtering_bench::runTestsRosetta(rosetta_size_min, rosetta_size_max, rosetta_size_step, keys, ranges);
    } else if (data_structure == "surfdense") {
        range_filtering_bench::runTestsSuRFRealDense(2, 16, keys, ranges, surfdense_suffix_len);
    } else if (data_structure == "lilrosetta") {
        range_filtering_bench::runTestsLilRosetta(rosetta_size_min, rosetta_size_max, rosetta_size_step, keys, ranges);
    }else if (data_structure == "fst") {
        range_filtering_bench::runTestsFST(keys, ranges);
//    } else if (data_structure == "splash") {
//        range_filtering_bench::runTestsSplash(splash_cutoff, splash_restraint_val_min,
//                                              splash_restraint_val_max, splash_restraint_val_interval,
//                                              keys, ranges);
    } else if (data_structure == "rangeBF") {
        range_filtering_bench::runTestsRangeBF(rangeBF_size_min, rangeBF_size_max, rangeBF_size_step, keys, ranges);
    } else if (data_structure == "rangeKRBF") {
        range_filtering_bench::runTestsRangeKRBF(rangeBF_size_min, rangeBF_size_max, rangeBF_size_step, keys, ranges);
    } else if (data_structure == "bloomedsplash") {
        range_filtering_bench::runTestsBloomedRangeSplash(rangeBF_size_min, rangeBF_size_max, rangeBF_size_step,
                                                          fst_height, keys, ranges);
    } else if (data_structure == "chareq") {
        range_filtering_bench::runTestsChareq(chareq_filled_in_fraction_min, chareq_filled_in_fraction_max, chareq_filled_in_fraction_step,
                                              chareq_top_layer_height, keys, ranges);
    } else if (data_structure == "rechareq") {
        range_filtering_bench::runTestsReChareq(chareq_filled_in_fraction_min, chareq_filled_in_fraction_max, chareq_filled_in_fraction_step,
                                              chareq_top_layer_height, rechareq_bits_per_char, keys, ranges);
    } else if (data_structure == "splash") {
        try {
            if (restraintType == range_filtering_splash::SplashRestraintType::relative) {
                range_filtering_bench::runSplashRelative(keys, ranges,
                                                         cutoff_min, cutoff_max, cutoff_interval,
                                                         relative_restraint_value_min, relative_restraint_value_max,
                                                         relative_restraint_interval);
            } else if (restraintType == range_filtering_splash::SplashRestraintType::absolute) {
                range_filtering_bench::runSplashAbsolute(keys, ranges,
                                                         cutoff_min, cutoff_max, cutoff_interval,
                                                         absolute_restraint_value_min, absolute_restraint_value_max,
                                                         absolute_restraint_interval);
            } else {
                std::cout << "Unknown restraint type " << restraintType << std::endl;
            }
        } catch (...) {}
    } else {
        std::cout << "Unknown data structure " << data_structure << std::endl;
    }
}