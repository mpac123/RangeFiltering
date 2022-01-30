#include "prefixBF_bench.h"

int main(int argc, char *argv[]) {

    // Load keys from file
    //std::string filename = "bench/workloads/top-10000-english-words";
    std::string filename = "/home/mapac/Coding/RangeFiltering/bench/workloads/top-10000-english-words";
    //std::string filename = "bench/workloads/words.txt";
    std::vector<std::string> keys;
    bench::loadKeysFromFile(filename, keys);

    // Generate insert keys
    std::vector<std::string> insert_keys;
    bench::selectKeysToInsert(100, insert_keys, keys);

    // Generate prefixes to query
    std::unordered_set<std::string> prefixes;
    bench::generatePrefixesToQuery(insert_keys, prefixes);

    std::cout << "Number of generated prefixes to be queried = " << prefixes.size() << std::endl << std::endl;

    std::cout << "arr_size\tmemoryUsage\tFPR\tBF_FPR\thashesCnt" << std::endl;
    //prefixBF_bench::runTests(1024, 7500000, 102400, insert_keys, prefixes);
    //prefixBF_bench::runTestsPBF(1024, 390148, 10240, insert_keys, prefixes);

    std::cout << std::endl;
    prefixBF_bench::runTestsPQF(15, 15, 15, 16, insert_keys, prefixes);

    auto surf_real = new surf::SuRF(insert_keys, surf::kReal, 0, 8);
    auto trie = range_filtering::Trie(insert_keys);
    std::cout << std::endl << "SuRF Real memory = " << surf_real->getMemoryUsage() << std::endl;
    std::cout << "FPR SuRF Real = " << bench::calculateFPR(surf_real, trie, prefixes) << std::endl;

    auto surf_base = new surf::SuRF(insert_keys, surf::kNone, 0, 0);
    std::cout << std::endl << "SuRF Base memory = " << surf_base->getMemoryUsage() << std::endl;
    std::cout << "FPR SuRF Base = " << bench::calculateFPR(surf_base, trie, prefixes) << std::endl;
}