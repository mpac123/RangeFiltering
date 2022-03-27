#include "bench.h"
#include "range_filtering_bench.hpp"

int main(int argc, char *argv[]) {
    std::string data_type = "powerlaw";
    std::string query_type = "similar";
    std::string input_dir = "/home/mapac/Coding/RangeFiltering/bench/workload-gen/range_queries_workloads/100k/";

    std::string data_structure = "fst";

    std::tuple<uint32_t, uint32_t> surf_params = {0, 0};

    uint64_t rosetta_size_min = 50000000;
    uint64_t rosetta_size_max = 100000000;
    uint64_t rosetta_size_step = 10000000;

    double splash_cutoff = 0.75;
    double splash_restraint_val_min = 1.0;
    double splash_restraint_val_max = 1.0;
    double splash_restraint_val_interval = 0.0;

    if (argc > 4) {
        data_type = argv[1];
        query_type = argv[2];
        input_dir = argv[3];
        data_structure = argv[4];
    }

    if (data_structure == "surf" && argc > 6) {
        surf_params = {std::stoi(argv[5]), std::stoi(argv[6])};
    }

    if (data_structure == "rosetta" && argc > 7) {
        rosetta_size_min = std::stoi(argv[5]);
        rosetta_size_max = std::stoi(argv[6]);
        rosetta_size_step = std::stoi(argv[7]);
    }

    if (data_structure == "splash" && argc > 8) {
        splash_cutoff = std::stod(argv[5]);
        splash_restraint_val_min = std::stod(argv[6]);
        splash_restraint_val_max = std::stod(argv[7]);
        splash_restraint_val_interval = std::stod(argv[8]);
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
    } else if (data_structure == "rosetta") {
        range_filtering_bench::runTestsRosetta(rosetta_size_min, rosetta_size_max, rosetta_size_step, keys, ranges);
    } else if (data_structure == "fst") {
        range_filtering_bench::runTestsFST(keys, ranges);
    } else if (data_structure == "splash") {
        range_filtering_bench::runTestsSplash(splash_cutoff, splash_restraint_val_min,
                                              splash_restraint_val_max, splash_restraint_val_interval,
                                              keys, ranges);
    } else {
        std::cout << "Unknown data structure " << data_structure << std::endl;
    }
}