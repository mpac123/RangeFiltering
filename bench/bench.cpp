#include "bench.h"

int main(int argc, char *argv[]) {

    // Load keys from file
    //std::string filename = "bench/workloads/top-10000-english-words";
    std::string filename = "/home/mapac/Coding/RangeFiltering/bench/workloads/words.txt";
    //std::string filename = "bench/workloads/words.txt";
    std::vector<std::string> keys;
    bench::loadKeysFromFile(filename, keys);

    // Generate insert keys
    std::vector<std::string> insert_keys;
    bench::selectKeysToInsert(100, insert_keys, keys);

    // Generate prefixes to query
    std::unordered_set<std::string> prefixes;
    bench::generatePrefixesToQuery(insert_keys, prefixes);

    std::cout << "Number of generated prefixes to be queried = " << prefixes.size() << std::endl;

    // Build tries
    auto trie = new range_filtering::Trie(insert_keys);
    auto compact_trie = new range_filtering::CompactTrie(insert_keys);
    auto bloom_blind_small = new range_filtering::BlindTrieWithBloomFilter(insert_keys, 256);
    auto bloom_blind_medium = new range_filtering::BlindTrieWithBloomFilter(insert_keys, 1024);
    auto bloom_blind_big = new range_filtering::BlindTrieWithBloomFilter(insert_keys, 8000);
    auto prefix_BF_S = new range_filtering::PrefixBloomFilter(insert_keys, 25000);
    auto prefix_BF_M = new range_filtering::PrefixBloomFilter(insert_keys, 200000);
    auto prefix_BF_L = new range_filtering::PrefixBloomFilter(insert_keys, 500000);

    auto surf_base = new surf::SuRF(insert_keys, surf::kNone, 0, 0);
    auto surf_hash = new surf::SuRF(insert_keys, surf::kHash, 8, 0);
    auto surf_real = new surf::SuRF(insert_keys, surf::kReal, 0, 8);
    auto surf_mix = new surf::SuRF(insert_keys, surf::kMixed, 4, 4);

    std::cout << "Trie memory = " << trie->getMemoryUsage() << std::endl;
    std::cout << "Compact trie memory = " << compact_trie->getMemoryUsage() << std::endl;
    std::cout << "Bloom Blind Trie (S) = " << bloom_blind_small->getMemoryUsage() << std::endl;
    std::cout << "Bloom Blind Trie (M) = " << bloom_blind_medium->getMemoryUsage() << std::endl;
    std::cout << "Bloom Blind Trie (L) = " << bloom_blind_big->getMemoryUsage() << std::endl;
    std::cout << "Prefix BF (S) = " << prefix_BF_S->getMemoryUsage() << std::endl;
    std::cout << "Prefix BF (M) = " << prefix_BF_M->getMemoryUsage() << std::endl;
    std::cout << "Prefix BF (L) = " << prefix_BF_L->getMemoryUsage() << std::endl;
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
    uint64_t false_positives_bloom_blind_trie_s = 0;
    uint64_t false_positives_bloom_blind_trie_m = 0;
    uint64_t false_positives_bloom_blind_trie_l = 0;
    uint64_t false_positives_prefix_BF_S = 0;
    uint64_t false_positives_prefix_BF_M = 0;
    uint64_t false_positives_prefix_BF_L = 0;


    for (const auto& prefix : prefixes) {
        bool foundInTrie = trie->lookupPrefix(prefix);
        bool foundInCompactTrie = compact_trie->lookupPrefix(prefix);
        bool foundInSuRF = bench::lookupPrefixInSuRF(surf_base, prefix);
        bool foundInSuRFHash = bench::lookupPrefixInSuRF(surf_hash, prefix);
        bool foundInSuRFReal = bench::lookupPrefixInSuRF(surf_real, prefix);
        bool foundInSuRFMixed = bench::lookupPrefixInSuRF(surf_mix, prefix);
        bool foundInBloomBlindTrieSmall = bloom_blind_small->lookupPrefix(prefix);
        bool foundInBloomBlindTrieMedium = bloom_blind_medium->lookupPrefix(prefix);
        bool foundInBloomBlindTrieLarge = bloom_blind_big->lookupPrefix(prefix);
        bool foundInPrefixBF_S = prefix_BF_S->lookupPrefix(prefix);
        bool foundInPrefixBF_M = prefix_BF_M->lookupPrefix(prefix);
        bool foundInPrefixBF_L = prefix_BF_L->lookupPrefix(prefix);

        if (foundInTrie && (!foundInCompactTrie || !foundInSuRF || !foundInSuRFHash ||
            !foundInSuRFReal || !foundInSuRFMixed || !foundInBloomBlindTrieLarge ||
            !foundInBloomBlindTrieMedium || !foundInBloomBlindTrieSmall || !foundInPrefixBF_S ||
            !foundInPrefixBF_M || !foundInPrefixBF_L)) {
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
        false_positives_bloom_blind_trie_s += (!foundInTrie && foundInBloomBlindTrieSmall);
        false_positives_bloom_blind_trie_m += (!foundInTrie && foundInBloomBlindTrieMedium);
        false_positives_bloom_blind_trie_l += (!foundInTrie && foundInBloomBlindTrieLarge);
        false_positives_prefix_BF_S += (!foundInTrie && foundInPrefixBF_S);
        false_positives_prefix_BF_M += (!foundInTrie && foundInPrefixBF_M);
        false_positives_prefix_BF_L += (!foundInTrie && foundInPrefixBF_L);

    }

    int64_t negatives = prefixes.size() - positives;
    double fp_rate_compact_trie = false_positives_compact_trie / (negatives + false_positives_compact_trie + 0.0);
    double fp_rate_surf_base = false_positives_surf_base / (negatives + false_positives_surf_base + 0.0);
    double fp_rate_surf_hash = false_positives_surf_hash / (negatives + false_positives_surf_hash + 0.0);
    double fp_rate_surf_real = false_positives_surf_real / (negatives + false_positives_surf_real + 0.0);
    double fp_rate_surf_mix = false_positives_surf_mixed / (negatives + false_positives_surf_mixed + 0.0);
    double fp_rate_bbt_s = false_positives_bloom_blind_trie_s / (negatives + false_positives_bloom_blind_trie_s + 0.0);
    double fp_rate_bbt_m = false_positives_bloom_blind_trie_m / (negatives + false_positives_bloom_blind_trie_m + 0.0);
    double fp_rate_bbt_l = false_positives_bloom_blind_trie_l / (negatives + false_positives_bloom_blind_trie_l + 0.0);
    double fp_rate_prefix_BF_S = false_positives_prefix_BF_S / (negatives + false_positives_prefix_BF_S + 0.0);
    double fp_rate_prefix_BF_M = false_positives_prefix_BF_M / (negatives + false_positives_prefix_BF_M + 0.0);
    double fp_rate_prefix_BF_L = false_positives_prefix_BF_L / (negatives + false_positives_prefix_BF_L + 0.0);



    std::cout << "FPR compact trie = " << fp_rate_compact_trie << std::endl;
    std::cout << "FPR base SuRF = " << fp_rate_surf_base << std::endl;
    std::cout << "FPR SuRFHash = " << fp_rate_surf_hash << std::endl;
    std::cout << "FPR SuRFReal = " << fp_rate_surf_real << std::endl;
    std::cout << "FPR SuRFMixed = " << fp_rate_surf_mix << std::endl;
    std::cout << "FPR BBT S = " << fp_rate_bbt_s << std::endl;
    std::cout << "FPR BBT M = " << fp_rate_bbt_m << std::endl;
    std::cout << "FPR BBT L = " << fp_rate_bbt_l << std::endl;
    std::cout << "FPR Prefix BF S = " << fp_rate_prefix_BF_S << std::endl;
    std::cout << "FPR Prefix BF M = " << fp_rate_prefix_BF_M << std::endl;
    std::cout << "FPR Prefix BF L = " << fp_rate_prefix_BF_L << std::endl;

}