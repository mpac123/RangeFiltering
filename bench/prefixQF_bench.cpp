#include "prefixBF_bench.h"

int main(int argc, char *argv[]) {
    std::string data_type = "uniform";
    std::string query_type = "similar";
    std::string input_dir = "/home/mapac/Coding/RangeFiltering/bench/workload-gen/workloads/100k/";
    uint64_t max_doubting_level = 0;
    bool multilevel = false;

    std::tuple<uint32_t, uint32_t> prefixQF_params = {1, 8};

    if (argc > 3) {
        data_type = argv[1];
        query_type = argv[2];
        input_dir = argv[3];
    }

    if (argc > 5) {
        prefixQF_params = {std::stoi(argv[4]), std::stoi(argv[5])};
    }

    if (argc > 6) {
        max_doubting_level = std::stoi(argv[6]);
    }

    if (argc > 7) {
        std::string type = argv[7];
        multilevel = type == "multi";
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

    std::cout << "Memory usage\tFPR Prefix-QF\tFP Prob QF\tq\tr\tCreation time\tQuery time" << std::endl;
    if (multilevel) {
        prefixBF_bench::runTestsMultiPQF(std::get<0>(prefixQF_params), std::get<1>(prefixQF_params), keys, prefixes,
                                         max_doubting_level);
    } else {
        prefixBF_bench::runTestsPQF(std::get<0>(prefixQF_params), std::get<1>(prefixQF_params), keys, prefixes,
                                    max_doubting_level);
    }
}