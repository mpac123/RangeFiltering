#include "bench.h"
#include "prefixBF_bench.h"

int main(int argc, char *argv[]) {
    std::string data_type = "uniform";
    std::string query_type = "similar";
    std::string input_dir = "/home/mapac/Coding/RangeFiltering/bench/workload-gen/workloads/100k_new/";

    std::string data_structure = "fst";

    std::tuple<uint32_t, uint32_t, uint32_t> prefixBF_params = {100000, 20000000, 500000};
    std::tuple<uint32_t, uint32_t, uint32_t, uint32_t> prefixQF_params = {21, 21, 1, 8};

    if (argc > 4) {
        data_type = argv[1];
        query_type = argv[2];
        input_dir = argv[3];
        data_structure = argv[4];
    }

    if (data_structure == "surf" && argc > 11) {
        prefixBF_params = {std::stoi(argv[5]), std::stoi(argv[6]), std::stoi(argv[7])};
        prefixQF_params = {std::stoi(argv[8]), std::stoi(argv[9]), std::stoi(argv[10]), std::stoi(argv[11])};
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
    prefixBF_bench::runTestsFST(keys, prefixes);
}