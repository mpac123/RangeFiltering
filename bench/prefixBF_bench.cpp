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

    std::cout << "memoryUsage\tFPR Prefix-BF\tPF Prob BF\tFPR Prefix-QF\tFP Prob QF\tSuRF Base\tSuRF Real" << std::endl;
    //prefixBF_bench::runTests(1024, 7500000, 102400, insert_keys, prefixes);
    prefixBF_bench::runTestsPBF(1024, 780148, 10240, insert_keys, prefixes);
    prefixBF_bench::runTestsPQF(16, 16, 1, 12, insert_keys, prefixes);

    auto trie = range_filtering::Trie(insert_keys);
    auto surf_base = new surf::SuRF(insert_keys, surf::kNone, 0, 0);
    std::cout << surf_base->getMemoryUsage() << "\t\t\t\t\t" << bench::calculateFPR(surf_base, trie, prefixes) << std::endl;

    for (size_t i = 1; i <= 8; i++) {
        auto surf_real = new surf::SuRF(insert_keys, surf::kReal, 0, i);
        std::cout << surf_real->getMemoryUsage() << "\t\t\t\t\t\t" << bench::calculateFPR(surf_real, trie, prefixes) << std::endl;
    }
}