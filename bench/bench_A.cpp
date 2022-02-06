#include "bench.h"
#include "SuRFFacade.h"

int main(int argc, char *argv[]) {
    // Load keys from file
    std::string filename = "/home/mapac/Coding/RangeFiltering/bench/workloads/top-10000-english-words";
    std::vector<std::string> keys;
    bench::loadKeysFromFile(filename, keys);

    // Generate insert keys
    std::vector<std::string> insert_keys;
    bench::selectKeysToInsert(100, insert_keys, keys);

    // Generate prefixes to query
    std::unordered_set<std::string> prefixes;
    bench::generatePrefixesToQuery(insert_keys, prefixes);

    // Build tries
    auto trie = new range_filtering::Trie(insert_keys);
    auto filters = std::vector<range_filtering::PrefixFilter*>();
    filters.push_back(trie);
    filters.push_back(new range_filtering::CompactTrie(insert_keys));
    filters.push_back(new range_filtering::PrefixBloomFilter(insert_keys, 134144));
    filters.push_back(new range_filtering::PrefixBloomFilter(insert_keys, 282504));
    filters.push_back(new range_filtering::SuRFFacade(insert_keys, false, 8));
    filters.push_back(new range_filtering::SuRFFacade(insert_keys, true, 0));

    // Print stats
    std::cout << "Structure\tMemory\tFPR" << std::endl;
    for (auto filter : filters) {
        std::cout << filter->getName() << "\t" << filter->getMemoryUsage() << "\t"
                  << bench::calculateFPR(filter, *trie, prefixes) << std::endl;
    }
}