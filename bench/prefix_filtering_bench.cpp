#include "bench.h"
#include "prefixBF_bench.h"

int main(int argc, char *argv[]) {
    std::string data_type = "last_letter_different";
    std::string query_type = "last_letter";
    std::string input_dir = "/home/mapac/Coding/RangeFiltering/bench/workload-gen/workloads/100k_new/";

    std::string data_structure = "splash";
    range_filtering_splash::SplashRestraintType restraintType = range_filtering_splash::SplashRestraintType::relative;

    double cutoff_min = 1.0;
    double cutoff_max = 1.0;
    double cutoff_interval = 0.05;
    uint64_t absolute_restraint_value_min = 0;
    uint64_t absolute_restraint_value_max = 0;
    uint64_t absolute_restraint_interval = 1.0;
    double relative_restraint_value_min = 1.0;
    double relative_restraint_value_max = 1.0;
    double relative_restraint_interval = 0.05;

    std::tuple<uint32_t, uint32_t> surf_params = {0, 0};

    if (argc > 4) {
        data_type = argv[1];
        query_type = argv[2];
        input_dir = argv[3];
        data_structure = argv[4];
    }

    if (data_structure == "surf" && argc > 6) {
        surf_params = {std::stoi(argv[5]), std::stoi(argv[6])};
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

    std::vector<std::string> prefixes;
    bench::loadKeysFromFile(query_filename, prefixes);

    if (keys.empty() || prefixes.empty()) {
        std::cout << "Didn't read files: " << input_filename << " and " << query_filename << std::endl;
        return -1;
    }

    std::cout << "Memory usage\tFPR\tSuffix size\tCreation time\tQuery time\tTrie memory usage" << std::endl;
    if (data_structure == "surf") {
        prefixBF_bench::runTestsSuRFReal(0, 0, keys, prefixes);
    } else if (data_structure == "splash") {
        if (restraintType == range_filtering_splash::SplashRestraintType::relative) {
            prefixBF_bench::runSplashRelative(keys, prefixes,
                                              cutoff_min, cutoff_max, cutoff_interval,
                                              relative_restraint_value_min, relative_restraint_value_max,
                                              relative_restraint_interval);
        } else if (restraintType == range_filtering_splash::SplashRestraintType::absolute) {
            prefixBF_bench::runSplashAbsolute(keys, prefixes,
                                              cutoff_min, cutoff_max, cutoff_interval,
                                              absolute_restraint_value_min, absolute_restraint_value_max,
                                              absolute_restraint_interval);
        } else {
            std::cout << "Unknown restraint type " << restraintType << std::endl;
        }
    } else if (data_structure == "fst") {
        prefixBF_bench::runTestsFST(keys, prefixes);
    } else {
        std::cout << "Unknown data structure " << data_structure << std::endl;
    }
}