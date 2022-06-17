#include "bench.h"
#include "integer_range_filtering_bench.h"

int main(int argc, char* argv[]) {
    std::string data_type = "normal";
    std::string input_dir = "/home/mapac/Coding/RangeFiltering/bench/workload-gen/integer_workloads/1e4/";
    std::string data_structure = "chareq";

    uint64_t universe_size = 1000;
    std::string n = "50";

    float empty_ranges_gamma_min = 1.0;
    float empty_ranges_gamma_max = 2.0;
    float empty_ranges_gamma_step = 0.25;

    uint64_t rosetta_size_min = 2000;
    uint64_t rosetta_size_max = 10000;
    uint64_t rosetta_size_step = 2000;

    uint64_t rangeBF_size_min = 100000;
    uint64_t rangeBF_size_max = 8000000;
    uint64_t rangeBF_size_step = 1000000;

    float chareq_filled_in_fraction_min = 0.2;
    float chareq_filled_in_fraction_max = 0.9;
    float chareq_filled_in_fraction_step = 0.1;

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

    if (argc > 5) {
        data_type = argv[1];
        input_dir = argv[2];
        data_structure = argv[3];
        universe_size = std::stoi(argv[4]);
        n = argv[5];
    }

    if (data_structure == "rosetta" && argc > 8) {
        rosetta_size_min = std::stoi(argv[6]);
        rosetta_size_max = std::stoi(argv[7]);
        rosetta_size_step = std::stoi(argv[8]);
    }

    if (data_structure == "rangeBF" && argc > 8) {
        rangeBF_size_min = std::stoi(argv[6]);
        rangeBF_size_max = std::stoi(argv[7]);
        rangeBF_size_step = std::stoi(argv[8]);
    }

    if (data_structure == "chareq" && argc > 8) {
        chareq_filled_in_fraction_min = std::stod(argv[6]);
        chareq_filled_in_fraction_max = std::stod(argv[7]);
        chareq_filled_in_fraction_step = std::stod(argv[8]);
    }

    if (data_structure == "splash" && argc > 12) {
        cutoff_min = std::stod(argv[6]);
        cutoff_max = std::stod(argv[7]);
        cutoff_interval = std::stod(argv[8]);
        if (strcmp(argv[9], "relative") == 0) {
            restraintType = range_filtering_splash::SplashRestraintType::relative;
            relative_restraint_value_min = std::stod(argv[10]);
            relative_restraint_value_max = std::stod(argv[11]);
            relative_restraint_interval = std::stod(argv[12]);
        } else if (strcmp(argv[9], "absolute") == 0) {
            restraintType = range_filtering_splash::SplashRestraintType::absolute;
            absolute_restraint_value_min = std::stoi(argv[10]);
            absolute_restraint_value_max = std::stoi(argv[11]);
            absolute_restraint_interval = std::stoi(argv[12]);
        }
    }

    std::string input_filename = input_dir + data_type + "_" + n + ".txt";
    std::vector<uint32_t> keys;
    bench::loadKeysFromFile(input_filename, keys);

    std::cout << "Memory usage\tFPR\tSuffix size\tCreation time\tQuery time\tTrie memory usage" << std::endl;

    if (data_structure == "surf") {
        range_filtering_bench::runTestsIntegerSuRFBase(keys, universe_size, 2, 10);
    } else if (data_structure == "surfreal") {
      range_filtering_bench::runTestsIntegerSuRFReal(keys, universe_size, 2, 10, 0, 8);
    } else if (data_structure == "fst") {
        range_filtering_bench::runTestsIntegerFST(keys, universe_size, 2, 10);
    } else if (data_structure == "rosetta") {
        range_filtering_bench::runTestsIntegerRosetta(keys, universe_size, 2, 10,
                                                      rosetta_size_min, rosetta_size_max, rosetta_size_step);
    } else if (data_structure == "emptyranges") {
        range_filtering_bench::runTestsEmptyRanges(keys, universe_size, 2, 10, empty_ranges_gamma_min,
                                                   empty_ranges_gamma_max, empty_ranges_gamma_step);
    } else if (data_structure == "rangeBF") {
        range_filtering_bench::runTestsRangeBF(keys, universe_size, 2, 10,
                                               rangeBF_size_min, rangeBF_size_max, rangeBF_size_step);
    } else if (data_structure == "chareq") {
        range_filtering_bench::runTestsIntegerCHaREQ(keys, universe_size, 2, 10,
                                                     chareq_filled_in_fraction_min, chareq_filled_in_fraction_max,
                                                     chareq_filled_in_fraction_step);
    } else if (data_structure == "splash") {
        if (restraintType == range_filtering_splash::SplashRestraintType::relative) {
            range_filtering_bench::runTestsIntegerSplashRelative(keys, universe_size, 2, 10,
                                                     cutoff_min, cutoff_max, cutoff_interval,
                                                     relative_restraint_value_min, relative_restraint_value_max,
                                                     relative_restraint_interval);
        } else {
            range_filtering_bench::runTestsIntegerSplashAbsolute(keys, universe_size, 2, 10,
                                                                 cutoff_min, cutoff_max, cutoff_interval,
                                                                 absolute_restraint_value_min,
                                                                 absolute_restraint_value_max,
                                                                 absolute_restraint_interval);
        }
    } else {
        std::cout << "Unknown data structure " << data_structure << std::endl;
    }

}