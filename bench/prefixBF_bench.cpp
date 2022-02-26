#include "prefixBF_bench.h"

int main(int argc, char *argv[]) {
    std::string data_type = "powerlaw";
    std::string query_type = "similar";
    std::string input_dir = "/home/mapac/Coding/RangeFiltering/bench/workload-gen/workloads/100k_new/";
    uint64_t max_doubting_level = 3;
    bool multilevel = true;
    auto memory_allocation_type = range_filtering::MultiPrefixBloomFilter::MemoryAllocationType::proportionalDecreasing;
    double max_mem_allocation_diff = 0.7;

    std::tuple<uint32_t, uint32_t, uint32_t> prefixBF_params = {100000, 20000000, 500000};

    if (argc > 3) {
        data_type = argv[1];
        query_type = argv[2];
        input_dir = argv[3];
    }

    if (argc > 6) {
        prefixBF_params = {std::stoi(argv[4]), std::stoi(argv[5]), std::stoi(argv[6])};
    }

    if (argc > 7) {
        max_doubting_level = std::stoi(argv[7]);
    }

    if (argc > 8) {
        std::string type = argv[8];
        multilevel = type == "multi";
    }

    if (argc > 9) {
        if (strcmp(argv[9], "proportional") == 0) {
            if (argc > 10 && std::stod(argv[10]) == 0.0)
                memory_allocation_type = range_filtering::MultiPrefixBloomFilter::MemoryAllocationType::proportional;
            else {
                memory_allocation_type = range_filtering::MultiPrefixBloomFilter::MemoryAllocationType::proportionalDecreasing;
                max_mem_allocation_diff = std::stod(argv[10]);
            }
        } else if (strcmp(argv[9], "equal") == 0) {
            if (argc > 10 && std::stod(argv[10]) == 0.0)
                memory_allocation_type = range_filtering::MultiPrefixBloomFilter::MemoryAllocationType::equal;
            else {
                memory_allocation_type = range_filtering::MultiPrefixBloomFilter::MemoryAllocationType::equalDecreasing;
                max_mem_allocation_diff = std::stod(argv[10]);
            }
        } else {
            std::cout << "Memory allocation type " << argv[9] << " not known." << std::endl;
            return -1;
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

    std::cout << "Memory usage\tFPR Prefix-BF\tFP Prob BF\tArray size\tK\tCreation time\tQuery time" << std::endl;
    prefixBF_bench::runTestsPBF(std::get<0>(prefixBF_params), std::get<1>(prefixBF_params),
                                std::get<2>(prefixBF_params),
                                keys, prefixes, max_doubting_level, multilevel,
                                memory_allocation_type, max_mem_allocation_diff);
}