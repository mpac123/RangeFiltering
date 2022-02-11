#include "bench.h"
#include "prefixBF_bench.h"

int main(int argc, char *argv[]) {
    std::string data_type = "uniform";
    std::string query_type = "similar";
    std::string input_dir = "/home/mapac/Coding/RangeFiltering/bench/workload-gen/workloads/";

    std::tuple<uint32_t, uint32_t, uint32_t> prefixBF_params = {100000, 20000000, 500000};
    std::tuple<uint32_t, uint32_t, uint32_t, uint32_t> prefixQF_params = {21, 21, 1, 8};

    if (argc > 3) {
        data_type = argv[1];
        query_type = argv[2];
        input_dir = argv[3];
    }

    if (argc > 10) {
        prefixBF_params = {std::stoi(argv[4]), std::stoi(argv[5]), std::stoi(argv[6])};
        prefixQF_params = {std::stoi(argv[7]), std::stoi(argv[8]), std::stoi(argv[9]), std::stoi(argv[10])};
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

    std::cout << "memoryUsage\tFPR Prefix-BF\tPF Prob BF\tFPR Prefix-QF\tFP Prob QF\tSuRF Real" << std::endl;

    // Run PrefixBF
    prefixBF_bench::runTestsPBF(std::get<0>(prefixBF_params), std::get<1>(prefixBF_params), std::get<2>(prefixBF_params), keys, prefixes);

    // Run PrefixQF
    prefixBF_bench::runTestsPQF(std::get<0>(prefixQF_params), std::get<1>(prefixQF_params),
            std::get<2>(prefixQF_params), std::get<3>(prefixQF_params), keys, prefixes);

    // Run SuRF Real
    prefixBF_bench::runTestsSuRFReal(0, 8, keys, prefixes);
}