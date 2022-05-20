#include "prefixBF_bench.h"

int main(int argc, char *argv[]) {
    std::string data_type = "uniform";
    std::string query_type = "similar";
    std::string input_dir = "/home/mapac/Coding/RangeFiltering/bench/workload-gen/workloads/";

    std::string data_structure = "fst";

    std::tuple<uint32_t, uint32_t> surf_params = {0, 8};

    if (argc > 3) {
        data_type = argv[1];
        query_type = argv[2];
        input_dir = argv[3];
    }

    if (argc > 5) {
        surf_params = {std::stoi(argv[4]), std::stoi(argv[5])};
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

    std::cout << "Memory usage\tFPR\tSuffix size\tCreation time\tQuery time" << std::endl;
    if (data_structure == "surfreal") {
        prefixBF_bench::runTestsSuRFReal(std::get<0>(surf_params), std::get<1>(surf_params), keys, prefixes);
    } else if (data_structure == "surfhash") {
        prefixBF_bench::runTestsSuRFReal(std::get<0>(surf_params), std::get<1>(surf_params), keys, prefixes);
    }
    prefixBF_bench::runTestsSuRFReal(std::get<0>(surf_params), std::get<1>(surf_params), keys, prefixes);
}