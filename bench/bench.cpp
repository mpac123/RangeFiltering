#include "bench.h"

int main(int argc, char *argv[]) {

    // Load keys from file
    std::string filename = "bench/workloads/top-10000-english-words";
    //std::string filename = "bench/workloads/words.txt";
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
    auto compact_trie = new range_filtering::CompactTrie(insert_keys);

    auto surf_base = new surf::SuRF(insert_keys, surf::kNone, 0, 0);
    auto surf_hash = new surf::SuRF(insert_keys, surf::kHash, 8, 0);
    auto surf_real = new surf::SuRF(insert_keys, surf::kReal, 0, 8);
    auto surf_mix = new surf::SuRF(insert_keys, surf::kMixed, 4, 4);

    std::cout << "Trie memory = " << trie->getMemoryUsage() << std::endl;
    std::cout << "Compact trie memory = " << compact_trie->getMemoryUsage() << std::endl;
    std::cout << "Base SuRF memory = " << surf_base->getMemoryUsage() << std::endl;
    std::cout << "SuRF Hash memory = " << surf_hash->getMemoryUsage() << std::endl;
    std::cout << "SuRF Real memory = " << surf_real->getMemoryUsage() << std::endl;
    std::cout << "SuRF Mixed memory = " << surf_mix->getMemoryUsage() << std::endl;

    // Make queries
    uint64_t positives = 0;
    uint64_t false_positives_compact_trie = 0;
    uint64_t false_positives_surf_base = 0;
    uint64_t false_positives_surf_hash = 0;
    uint64_t false_positives_surf_real = 0;
    uint64_t false_positives_surf_mixed = 0;


    for (const auto& prefix : prefixes) {
        bool foundInTrie = trie->lookupPrefix(prefix);
        bool foundInCompactTrie = compact_trie->lookupPrefix(prefix);
        bool foundInSuRF = bench::lookupPrefixInSuRF(surf_base, prefix);
        bool foundInSuRFHash = bench::lookupPrefixInSuRF(surf_hash, prefix);
        bool foundInSuRFReal = bench::lookupPrefixInSuRF(surf_real, prefix);
        bool foundInSuRFMixed = bench::lookupPrefixInSuRF(surf_mix, prefix);

        if (foundInTrie && (!foundInCompactTrie || !foundInSuRF || !foundInSuRFHash || !foundInSuRFReal || !foundInSuRFMixed)) {
            std::cout << "False negatives are not allowed in filters! Key: " << prefix << std::endl;
            if (!foundInSuRFReal) {
                std::cout << "Not found in SurfReal" << std::endl;
            }
            //return -1;
        }

        positives += foundInTrie;
        false_positives_compact_trie += (!foundInTrie && foundInCompactTrie);
        false_positives_surf_base += (!foundInTrie && foundInSuRF);
        false_positives_surf_hash += (!foundInTrie && foundInSuRFHash);
        false_positives_surf_real += (!foundInTrie && foundInSuRFReal);
        false_positives_surf_mixed += (!foundInTrie && foundInSuRFMixed);
    }

    int64_t negatives = prefixes.size() - positives;
    double fp_rate_compact_trie = false_positives_compact_trie / (negatives + false_positives_compact_trie + 0.0);
    double fp_rate_surf_base = false_positives_surf_base / (negatives + false_positives_surf_base + 0.0);
    double fp_rate_surf_hash = false_positives_surf_hash / (negatives + false_positives_surf_hash + 0.0);
    double fp_rate_surf_real = false_positives_surf_real / (negatives + false_positives_surf_real + 0.0);
    double fp_rate_surf_mix = false_positives_surf_mixed / (negatives + false_positives_surf_mixed + 0.0);

    
    std::cout << "FPR compact trie = " << fp_rate_compact_trie << std::endl;
    std::cout << "FPR base SuRF = " << fp_rate_surf_base << std::endl;
    std::cout << "FPR SuRFHash = " << fp_rate_surf_hash << std::endl;
    std::cout << "FPR SuRFReal = " << fp_rate_surf_real << std::endl;
    std::cout << "FPR SuRFMixed = " << fp_rate_surf_mix << std::endl;


    bench::lookupPrefixInSuRF(surf_base, "the");
}