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
}


#endif //RANGE_FILTERING_BENCH_H
