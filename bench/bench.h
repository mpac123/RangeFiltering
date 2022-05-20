#ifndef RANGE_FILTERING_BENCH_H
#define RANGE_FILTERING_BENCH_H

#include <iostream>
#include <fstream>
#include <random>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <random>
#include <sstream>
#include <string>

#include "Trie.hpp"
#include "CompactTrie.hpp"
#include "surf.hpp"
#include "BlindTrieWithBloomFilter.h"
#include "PrefixBloomFilter.h"
#include "RangeFilter.h"
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

    void loadKeysFromFile(const std::string& file_name, std::vector<uint32_t> &keys) {
        std::string line;
        std::ifstream infile(file_name);
        if (infile.is_open()) {
            while (getline(infile, line)) {
                keys.push_back(std::stoi(line));
            }
            infile.close();
        } else {
            std::cout << "Can't open file" << std::endl;
        }
    }

    void loadRangeQueriesFromFile(const std::string& file_name, std::vector<std::pair<std::string, std::string>> &keys) {
        std::string line;
        std::ifstream infile(file_name);
        if (infile.is_open()) {
            while (getline(infile, line)) {
                std::string segment;
                std::vector<std::string> seglist;
                std::stringstream streamline(line);
                while (std::getline(streamline, segment, ';')) {
                    seglist.push_back(segment);
                }
                keys.emplace_back(seglist[0], seglist[1]);
            }
            infile.close();
        } else {
            std::cout << "Can't open file" << std::endl;
        }
    }

    void selectKeysToInsert(const unsigned num_insert_keys,
                            std::vector<std::string> &insert_keys,
                            std::vector<std::string> &keys) {
        std::shuffle(keys.begin(), keys.end(), std::mt19937(std::random_device()()));
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
            double total_time = 0.0;
            bool foundInFilter;
            for (size_t i = 0; i < 10; i++) {
                auto start = std::chrono::system_clock::now();
                foundInFilter = filter->lookupPrefix(prefix);
                auto end = std::chrono::system_clock::now();
                std::chrono::duration<double> elapsed_seconds = end-start;
                total_time += elapsed_seconds.count();
            }



             negatives += (int) !foundInFilter;
            false_positives += (int) (!foundInTrie && foundInFilter);
            true_negatives += (int) (!foundInFilter && !foundInTrie);
            total_query_time += total_time / 10;
        }

        assert(negatives == true_negatives);
        double fp_rate = false_positives / (negatives + false_positives + 0.0);
        double average_query_time = total_query_time / prefixes.size();
        return std::make_tuple(fp_rate, average_query_time);
    }

    std::tuple<double, double> calculateFPR(range_filtering::RangeFilter *filter,
                                            range_filtering::Trie &trie,
                                            std::vector<std::pair<std::string, std::string>> &queries) {
        uint32_t negatives = 0;
        uint32_t false_positives = 0;
        uint32_t true_negatives = 0;
        double total_query_time = 0.;
        for (const auto& query : queries) {
            bool foundInTrie = trie.lookupRange(query.first, true, query.second, true);
            double total_time = 0.0;
            bool foundInFilter;
            auto start = std::chrono::system_clock::now();
            foundInFilter = filter->lookupRange(query.first, query.second);
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end-start;
            total_time += elapsed_seconds.count();


            negatives += (int) !foundInFilter;
            false_positives += (int) (!foundInTrie && foundInFilter);
            true_negatives += (int) (!foundInFilter && !foundInTrie);
            total_query_time += total_time;
        }

        // assert(negatives == true_negatives);
        double fp_rate = false_positives / (true_negatives + false_positives + 0.0);
        double average_query_time = total_query_time / queries.size();
        return std::make_tuple(fp_rate, average_query_time);
    }

    std::tuple<double, double> calculateFPR(range_filtering::PrefixFilter *filter01,
                                            range_filtering::PrefixFilter *filter02,
                                            range_filtering::Trie &trie,
                                            std::vector<std::string> &prefixes) {
        uint32_t negatives = 0;
        uint32_t false_positives = 0;
        uint32_t true_negatives = 0;
        double total_query_time = 0.;
        for (const auto& prefix : prefixes) {
            bool foundInTrie = trie.lookupPrefix(prefix);
            double total_time = 0.0;
            bool foundInFilter01;
            bool foundInFilter02;
            for (size_t i = 0; i < 10; i++) {
                auto start = std::chrono::system_clock::now();
                foundInFilter01 = filter01->lookupPrefix(prefix);
                auto end = std::chrono::system_clock::now();
                std::chrono::duration<double> elapsed_seconds = end-start;
                total_time += elapsed_seconds.count();
            }
            foundInFilter02 = filter02->lookupPrefix(prefix);
            if (foundInFilter01 != foundInFilter02) {
                std::cout << prefix << std::endl;
            }

            negatives += (int) !foundInFilter01;
            false_positives += (int) (!foundInTrie && foundInFilter01);
            true_negatives += (int) (!foundInFilter01 && !foundInTrie);
            total_query_time += total_time / 10;
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
