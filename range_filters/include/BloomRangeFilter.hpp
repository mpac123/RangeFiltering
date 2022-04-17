#ifndef RANGE_FILTERING_BLOOMRANGEFILTER_HPP
#define RANGE_FILTERING_BLOOMRANGEFILTER_HPP

#include <iostream>
#include <vector>
#include "PrefixFilter.h"
#include "RangeFilter.h"
#include "PrefixBloomFilter.h"
#include <bitset>
#include <assert.h>

namespace range_filtering {
    class BloomRangeFilter : public PrefixFilter, public RangeFilter {
    public:
        BloomRangeFilter(std::vector<std::string>& keys, uint32_t m);
        bool lookupPrefix(const std::string& key) override;
        bool lookupRange(const std::string& leftKey, const std::string& rightKey) override;
        uint64_t getMemoryUsage() const;
    private:
        PrefixBloomFilter bf_;
        std::bitset<256> charUsageMask_;

        bool lookupRangeOneLevel(const std::string &leftKey, const std::string &rightKey);
        bool lookupRangeRecursively(const std::string &leftKey, const std::string& rightKey, char firstCharInAlphabet, char lastLetterInAlphabet);
        std::string getNextUp(std::string word, char lastLetterInAlphabet);
        std::string getNextDown(std::string word, char firstLetterInAlphabet);
    };

    BloomRangeFilter::BloomRangeFilter(std::vector<std::string> &keys, uint32_t m) : bf_(keys, m, 0) {
        for (const auto& key : keys) {
            for (const auto& letter : key) {
                charUsageMask_[int(letter)] = true;
            }
        }
    }

    bool BloomRangeFilter::lookupPrefix(const std::string &key) {
        if (key.empty()) {
            return true;
        }
        return bf_.lookupPrefix(key);
    }

    bool BloomRangeFilter::lookupRange(const std::string &leftKey, const std::string &rightKey) {
        if (leftKey > rightKey) return false;
        std::string common_prefix;
        for (size_t i = 0; i < std::min(leftKey.length(), rightKey.length()); i++) {
            if (leftKey[i] != rightKey[i]) break;
            common_prefix += leftKey[i];
        }

        // All the prefixes in the common_prefix should exist in BF
        for (size_t i = 0; i < common_prefix.length(); i++) {
            auto prefix = common_prefix.substr(0, i + 1);
            if (!lookupPrefix(prefix)) return false;
        }

        // Split query into multiple ranges
        uint8_t first_letter_in_alphabet = 255;
        uint8_t last_letter_in_alphabet = 0;
        for (size_t i = 0; i < 256; i++) {
            if (charUsageMask_[i]) {
                first_letter_in_alphabet = i;
                break;
            }
        }
        for (size_t i = 255; i >= 0; i--) {
            if (charUsageMask_[i]) {
                last_letter_in_alphabet = i;
                break;
            }
        }

        return lookupRangeRecursively(leftKey, rightKey, first_letter_in_alphabet, last_letter_in_alphabet);
    }

    bool BloomRangeFilter::lookupRangeRecursively(const std::string &leftKey, const std::string& rightKey,
                                                  char firstCharInAlphabet, char lastLetterInAlphabet) {
        std::string common_prefix = "";
        for (size_t i = 0; i < std::min(leftKey.length(), rightKey.length()); i++) {
            if (leftKey[i] != rightKey[i]) break;
            common_prefix += leftKey[i];
        }

        if (leftKey.length() == rightKey.length() &&
        (common_prefix.length() == leftKey.length() - 1 || common_prefix.length() == leftKey.length())) {
            return lookupRangeOneLevel(leftKey, rightKey);
        }

        if (leftKey.length() - common_prefix.length() > 1) {
            std::string newRightKey = leftKey.substr(0, leftKey.length() - 1);
            newRightKey += lastLetterInAlphabet;
            if (lookupRangeOneLevel(leftKey, newRightKey)) return true;
            std::string newLeftKey = getNextUp(newRightKey, lastLetterInAlphabet);
            return lookupRangeRecursively(newLeftKey, rightKey, firstCharInAlphabet, lastLetterInAlphabet);
        }

        if (leftKey.length() - common_prefix.length() <= 1) {
            std::string newRightKey = leftKey.substr(0, leftKey.length() - 1);
            newRightKey += rightKey.at(common_prefix.length());
            if (lookupRangeOneLevel(leftKey, newRightKey)) return true;
            std::string newLeftKey = getNextDown(newRightKey, firstCharInAlphabet);
            return lookupRangeRecursively(newLeftKey, rightKey, firstCharInAlphabet, lastLetterInAlphabet);
        }
    }

    std::string BloomRangeFilter::getNextUp(std::string word, char lastLetterInAlphabet) {
        while (word.at(word.length() - 1) >= lastLetterInAlphabet) {
            word = word.substr(0, word.length() - 1);
            if (word.length() == 0) return word;
        }
        auto current = word.at(word.length() - 1);
        while (!charUsageMask_[current]) {
            current++;
        }
        auto next = word.substr(0, word.length() - 1);
        next += current;
        return next;
    }

    std::string BloomRangeFilter::getNextDown(std::string word, char firstLetterInAlphabet) {
        return word + firstLetterInAlphabet;
    }

    bool BloomRangeFilter::lookupRangeOneLevel(const std::string &leftKey, const std::string &rightKey) {
        assert(leftKey.length() == rightKey.length());
        for (uint8_t letter = leftKey.at(leftKey.length() - 1); letter <= rightKey.at(rightKey.length() - 1); letter += 1) {
            if (charUsageMask_[letter] == false) continue;
            auto query = leftKey.substr(0, leftKey.length() - 1);
            query += letter;
            if (lookupPrefix(query)) return true;
        }
        return false;
    }

    uint64_t BloomRangeFilter::getMemoryUsage() const {
        return bf_.getMemoryUsage() + (uint64_t) 256 / 8;
    }
}

#endif //RANGE_FILTERING_BLOOMRANGEFILTER_HPP
