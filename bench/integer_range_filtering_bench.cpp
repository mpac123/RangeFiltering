#include "bench.h"
#include "integer_range_filtering_bench.h"

int main(int argc, char* argv[]) {
    std::string data_type = "normal";
    std::string input_dir = "/home/mapac/Coding/RangeFiltering/bench/workload-gen/integer_workloads/1e4/";
    std::string data_structure = "emptyranges";

    uint64_t universe_size = 1000;
    std::string n = "50";

    float empty_ranges_gamma_min = 1.0;
    float empty_ranges_gamma_max = 2.0;
    float empty_ranges_gamma_step = 0.25;

    uint64_t rosetta_size_min = 2000;
    uint64_t rosetta_size_max = 10000;
    uint64_t rosetta_size_step = 2000;

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

    std::string input_filename = input_dir + data_type + "_" + n + ".txt";
    std::vector<uint32_t> keys;
    bench::loadKeysFromFile(input_filename, keys);

    std::cout << "Memory usage\tFPR\tSuffix size\tCreation time\tQuery time\tTrie memory usage" << std::endl;

    if (data_structure == "surf") {
        range_filtering_bench::runTestsIntegerSuRFReal(keys, universe_size, 2, 10);
    } else if (data_structure == "rosetta") {
        range_filtering_bench::runTestsIntegerRosetta(keys, universe_size, 2, 10,
                                                      rosetta_size_min, rosetta_size_max, rosetta_size_step);
    } else if (data_structure == "emptyranges") {
        range_filtering_bench::runTestsEmptyRanges(keys, universe_size, 2, 10, empty_ranges_gamma_min,
                                                   empty_ranges_gamma_max, empty_ranges_gamma_step);
    } else {
        std::cout << "Unknown data structure " << data_structure << std::endl;
    }

}