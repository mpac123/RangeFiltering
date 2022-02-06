#include "bench.h"
#include "prefixBF_bench.h"

int main(int argc, char *argv[]) {
    std::string data_type = "uniform";
    std::string query_type = "similar";
    if (argc > 2) {
        data_type = argv[1];
        query_type = argv[2];
    }
    std::string input_filename = "/home/mapac/Coding/RangeFiltering/bench/workload-gen/workloads/" + data_type + "_input.txt";
    std::string query_filename = "/home/mapac/Coding/RangeFiltering/bench/workload-gen/workloads/" + data_type + "_queries_" + query_type + ".txt";

    std::vector<std::string> keys;
    bench::loadKeysFromFile(input_filename, keys);

    std::vector<std::string> prefixes;
    bench::loadKeysFromFile(query_filename, prefixes);

    std::cout << "memoryUsage\tFPR Prefix-BF\tPF Prob BF\tFPR Prefix-QF\tFP Prob QF\tSuRF Real" << std::endl;

    // Run PrefixBF
    std::tuple<uint32_t, uint32_t, uint32_t> prefixBF_params = {100000, 20000000, 500000};
    prefixBF_bench::runTestsPBF(std::get<0>(prefixBF_params), std::get<1>(prefixBF_params), std::get<2>(prefixBF_params), keys, prefixes);

    // Run PrefixQF
    std::tuple<uint32_t, uint32_t, uint32_t, uint32_t> prefixQF_params = {21, 21, 1, 8};
    prefixBF_bench::runTestsPQF(std::get<0>(prefixQF_params), std::get<1>(prefixQF_params),
            std::get<2>(prefixQF_params), std::get<3>(prefixQF_params), keys, prefixes);

    // Run SuRF Real
    prefixBF_bench::runTestsSuRFReal(0, 8, keys, prefixes);
}