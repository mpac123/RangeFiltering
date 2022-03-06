#include "prefixBF_bench.h"

int main(int argc, char *argv[]) {
    std::string data_type = "uniform";
    std::string query_type = "similar";
    std::string input_dir = "/home/mapac/Coding/RangeFiltering/bench/workload-gen/workloads/100k_new/";
    range_filtering::RestraintType restraintType = range_filtering::RestraintType::none;

    bool bloomed = false;
    uint64_t bf_min_size = 50000;
    uint64_t bf_max_size = 500000;
    uint64_t bf_interval = 50000;

    bool splashy = false;
    double splashiness_min = 0.8;
    double splashiness_max = 1.0;
    double splashiness_interval = 0.05;

    uint64_t absolute_restraint_value_min = 0;
    uint64_t absolute_restraint_value_max = 0;
    double relative_restraint_value_min = 0.0;
    double relative_restraint_value_max = 0.0;

    std::tuple<uint32_t, uint32_t> surf_params = {0, 0};

    if (argc > 3) {
        data_type = argv[1];
        query_type = argv[2];
        input_dir = argv[3];
    }

    if (argc > 5) {
        surf_params = {std::stoi(argv[4]), std::stoi(argv[5])};
    }

    if (argc > 8) {
        if (strcmp(argv[6], "relative") == 0) {
            restraintType = range_filtering::RestraintType::relative;
            relative_restraint_value_min = std::stod(argv[7]);
            relative_restraint_value_max = std::stod(argv[8]);
        } else if (strcmp(argv[6], "absolute") == 0) {
            restraintType = range_filtering::RestraintType::absolute;
            absolute_restraint_value_min = std::stoi(argv[7]);
            absolute_restraint_value_max = std::stoi(argv[8]);
        } else if (strcmp(argv[6], "bloomed") == 0) {
            bloomed = true;
            if (argc > 9) {
                bf_min_size == std::stoi(argv[7]);
                bf_max_size = std::stoi(argv[8]);
                bf_interval = std::stoi(argv[9]);
            }
        } else if (strcmp(argv[6], "splashy") == 0) {
            splashy = true;
            if (argc > 9) {
                splashiness_min = std::stod(argv[7]);
                splashiness_max = std::stod(argv[8]);
                splashiness_interval = std::stod(argv[9]);
            }
        } else if (strcmp(argv[6], "restrained_splashy") == 0) {
            splashy = true;
            if (argc > 12) {
                splashiness_min = std::stod(argv[7]);
                splashiness_max = std::stod(argv[8]);
                splashiness_interval = std::stod(argv[9]);
                if (strcmp(argv[10], "relative") == 0) {
                    restraintType = range_filtering::RestraintType::relative;
                    relative_restraint_value_min = std::stod(argv[11]);
                    relative_restraint_value_max = std::stod(argv[12]);
                } else if (strcmp(argv[10], "absolute") == 0) {
                    restraintType = range_filtering::RestraintType::absolute;
                    absolute_restraint_value_min = std::stoi(argv[11]);
                    absolute_restraint_value_max = std::stoi(argv[12]);
                } else {
                    std::cout << "Unknown restraint type" << std::endl;
                    return -1;
                }
            }
        }
        else {
            restraintType = range_filtering::RestraintType::none;
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

    if (bloomed) {
        prefixBF_bench::runTestsBloomedSurfingTrie(std::get<0>(surf_params), std::get<1>(surf_params),
                bf_min_size, bf_max_size, bf_interval, keys, prefixes);
        return 0;
    }

    if (splashy) {
        if (restraintType == range_filtering::RestraintType::none) {
            prefixBF_bench::runTestsSplashyTrie(std::get<0>(surf_params), std::get<1>(surf_params),
                                                splashiness_min, splashiness_max, splashiness_interval, keys, prefixes);
        } else if (restraintType == range_filtering::RestraintType::absolute) {
            prefixBF_bench::runTestsAbsoluteRestrainedSplashyTrie(std::get<0>(surf_params), std::get<1>(surf_params),
                                                                 absolute_restraint_value_min, absolute_restraint_value_max,
                                                                 splashiness_min, splashiness_max, splashiness_interval,
                                                                 keys, prefixes);
        } else {
            prefixBF_bench::runTestsRelativeRestrainedSplashyTrie(std::get<0>(surf_params), std::get<1>(surf_params),
                                                                 relative_restraint_value_min, relative_restraint_value_max, 0.05,
                                                                 splashiness_min, splashiness_max, splashiness_interval,
                                                                 keys, prefixes);
        }
        return 0;
    }

    if (restraintType == range_filtering::RestraintType::none) {
        prefixBF_bench::runTestsSurfingTrie(std::get<0>(surf_params), std::get<1>(surf_params), keys, prefixes);
    } else if (restraintType == range_filtering::RestraintType::absolute) {
        prefixBF_bench::runTestsAbsoluteRestraintSurfingTrie(std::get<0>(surf_params), std::get<1>(surf_params),
                                                             absolute_restraint_value_min, absolute_restraint_value_max,
                                                             keys, prefixes);
    }  else {
        prefixBF_bench::runTestsRelativeRestraintSurfingTrie(std::get<0>(surf_params), std::get<1>(surf_params),
                                                             relative_restraint_value_min, relative_restraint_value_max, 0.05,
                                                             keys, prefixes);
    }
}