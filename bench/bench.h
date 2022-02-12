#ifndef RANGE_FILTERING_BENCH_H
#define RANGE_FILTERING_BENCH_H

#include <iostream>
#include <fstream>
#include <random>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <random>

#include "Trie.hpp"
#include "CompactTrie.hpp"
#include "surf.hpp"
#include "BlindTrieWithBloomFilter.h"
#include "PrefixBloomFilter.h"
#include <chrono>
#include <ctime>

namespace bench {

    static const uint64_t kNumRecords = 100000000;

    void loadKeysFromFile(const std::string& file_name, std::vector<std::string> &keys) {
        std::string line;
        std::ifstream infile(file_name);
        if (infile.is_open()) {
            while (getline(infile, line)) {
                keys.push_back(line);
            }
            infile.close();
        } else {
            std::cout << "Can't open file" << std::endl;
        }
    }

    // 0 < percent <= 100
    void selectKeysToInsert(const unsigned percent,
                            std::vector<std::string> &insert_keys,
                            std::vector<std::string> &keys) {
        std::shuffle(keys.begin(), keys.end(), std::mt19937(std::random_device()()));
        uint64_t num_insert_keys = keys.size() * percent / 100;
        for (uint64_t i = 0; i < num_insert_keys; i++)
            insert_keys.push_back(keys[i]);

        keys.clear();
        std::unordered_set<std::string> s(insert_keys.begin(), insert_keys.end());
        insert_keys.assign(s.begin(), s.end());
        sort(insert_keys.begin(), insert_keys.end());
    }

    uint generateRandomWithLinearDistribution(uint16_t max) {
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> dist(1, max);
        return std::max(dist(rng), dist(rng));
    }

    char generateRandomCharacter() {
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> dist(0, 9);
        return '0' + dist(rng);
    }

    void generatePrefixesToQuery(std::vector<std::string> &keys,
                                 std::unordered_set<std::string> &prefixes) {
        // Generate 10 prefixes out of every key:
        // 1. choose randomly with linear distribution cut-off index
        // 2. replace last character with randomly selected one ([0-9])
        for (const auto& key : keys) {
            uint counter = 0;
            while (counter < 10) {
                auto prefix = key.substr(0, generateRandomWithLinearDistribution(key.size()));
                if (counter > 4) prefix[prefix.length() - 1] = generateRandomCharacter();
                counter++;
                prefixes.insert(prefix);
            }
        }
    }

    bool lookupPrefixInSuRF(surf::SuRF *surf, std::string prefix) {
        char last_char = prefix[prefix.length() - 1];
        std::string right_key = prefix;
        right_key[prefix.length() - 1] = last_char + 1;
        return surf->lookupRange(prefix, true, right_key, false);
    }

    double calculateFPR(range_filtering::PrefixFilter *filter,
                        range_filtering::Trie &trie,
                        std::unordered_set<std::string> &prefixes) {
        uint32_t negatives = 0;
        uint32_t false_positives = 0;
        uint32_t true_negatives = 0;
        for (const auto& prefix : prefixes) {
            bool foundInTrie = trie.lookupPrefix(prefix);
            bool foundInFilter = filter->lookupPrefix(prefix);

            negatives += (int) !foundInFilter;
            false_positives += (int) (!foundInTrie && foundInFilter);
            true_negatives += (int) (!foundInFilter && !foundInTrie);
        }

        assert(negatives == true_negatives);
        double fp_rate = false_positives / (negatives + false_positives + 0.0);
        return fp_rate;
    }

    std::tuple<double, double> calculateFPR(range_filtering::PrefixFilter *filter,
                        range_filtering::Trie &trie,
                        std::vector<std::string> &prefixes) {
        uint32_t negatives = 0;
        uint32_t false_positives = 0;
        uint32_t true_negatives = 0;
        double total_query_time = 0.;
        for (const auto& prefix : prefixes) {
            bool foundInTrie = trie.lookupPrefix(prefix);
            auto start = std::chrono::system_clock::now();
            bool foundInFilter = filter->lookupPrefix(prefix);
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end-start;

            negatives += (int) !foundInFilter;
            false_positives += (int) (!foundInTrie && foundInFilter);
            true_negatives += (int) (!foundInFilter && !foundInTrie);
            total_query_time += elapsed_seconds.count();
        }

        assert(negatives == true_negatives);
        double fp_rate = false_positives / (negatives + false_positives + 0.0);
        double average_query_time = total_query_time / prefixes.size();
        return std::make_tuple(fp_rate, average_query_time);
    }

    double calculateFPR(surf::SuRF *surf,
                        range_filtering::Trie &trie,
                        std::unordered_set<std::string> &prefixes) {
        uint32_t negatives = 0;
        uint32_t false_positives = 0;
        uint32_t true_negatives = 0;
        for (const auto& prefix : prefixes) {
            bool foundInTrie = trie.lookupPrefix(prefix);
            bool foundInFilter = lookupPrefixInSuRF(surf, prefix);

            negatives += (int) !foundInFilter;
            false_positives += (int) (!foundInTrie && foundInFilter);
            true_negatives += (int) (!foundInFilter && !foundInTrie);
        }

        assert(negatives == true_negatives);
        double fp_rate = false_positives / (negatives + false_positives + 0.0);
        return fp_rate;
    }
}


#endif //RANGE_FILTERING_BENCH_H
