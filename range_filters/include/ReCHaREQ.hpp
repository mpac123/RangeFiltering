#ifndef RANGE_FILTERING_RECHAREQ_HPP
#define RANGE_FILTERING_RECHAREQ_HPP

#include <vector>
#include <string>
#include <MurmurHash3.h>
#include "RangeFilter.h"
#include <cstdint>
#include <cassert>
#include "Trie.hpp"

#define LOW_MASK(n) ((1ULL << (n)) - 1ULL)

namespace range_filters {
class ReCHaREQ : public range_filtering::RangeFilter {
public:
    ReCHaREQ(std::vector<std::string> &keys, uint32_t topLaterMaxLength, float filledInFraction, uint32_t bitsPerChar);
    bool lookupRange(const std::string& leftKey, const std::string& rightKey) override;
    uint64_t getMemoryUsage() const override;

private:
    uint64_t *table_;
    uint64_t slots_cnt_ = 0;
    uint64_t max_hash_value = 0;
    uint8_t elem_bits_ = 0;
    uint64_t elem_mask_ = 0;
    uint32_t r_ = 0;
    uint64_t r_mask_ = 0;

    uint32_t top_layer_max_length_;

    std::vector<std::string> prefix_list_;

    void countKeys(range_filtering::Trie& trie, uint64_t& prefix_cnt);
    void traverseNode(range_filtering::Trie::TrieNode* node, std::string& prefix, uint64_t& prefix_cnt);
    void populateFilter(range_filtering::Trie::TrieNode* node, std::string& prefix);
    void insertElement(uint64_t idx, char letter);

    uint64_t getElement(uint64_t idx);
    void setElement(uint64_t idx, uint64_t elt);

    static inline uint64_t isOccupied(uint64_t elt) { return elt & 1; }
    static inline uint64_t setOccupied(uint64_t elt) { return elt | 1; }
    static inline uint64_t clrOccupied(uint64_t elt) { return elt & ~1; }

    static inline uint64_t isContinuation(uint64_t elt) { return elt & 2; }
    static inline uint64_t setContinuation(uint64_t elt) { return elt | 2; }
    static inline uint64_t clrContinuation(uint64_t elt) { return elt & ~2; }

    static inline uint64_t isShifted(uint64_t elt) { return elt & 4; }
    static inline uint64_t setShifted(uint64_t elt) { return elt | 4; }
    static inline uint64_t clrShifted(uint64_t elt) { return elt & ~4; }

    static inline uint64_t getValue(uint64_t elt) { return elt >> 3; }

    static inline bool isEmptyElement(uint64_t elt) { return (elt & 7) == 0; }
    static inline bool isClusterStart(uint64_t elt) { return isOccupied(elt) && !isContinuation(elt) && !isShifted(elt); }
    static inline bool isRunStart(uint64_t elt) { return !isContinuation(elt) && (isOccupied(elt) || isShifted(elt)); }

    inline uint64_t encodeChar(char ch) { return (ch >> (7 - r_)) & r_mask_; }

    uint64_t findRunIndex(uint64_t idx);
    void insertAndShift(uint64_t s, uint64_t elt);
    uint64_t hashString(std::string& s);

    std::pair<bool, std::string> queryTopLayer(const std::string& leftKey, const std::string& rightKey);
    int64_t findPosPrefixInTopLayer(const std::string& key);
    bool lookupLetter(uint64_t idx, char letter);
    bool lookupRangeBetweenLetters(uint64_t idx, char letterLeft, char letterRight);
    bool lookupRangeRecursively(const std::string &leftKey, const std::string &rightKey);
};

    range_filters::ReCHaREQ::ReCHaREQ(std::vector<std::string> &keys, uint32_t topLayerMaxLength,
                                      float filledInFraction, uint32_t bitsPerChar) {
        assert(keys.size() > 0);
        top_layer_max_length_ = topLayerMaxLength;
        prefix_list_ = std::vector<std::string>();

        // generate keys
        range_filtering::Trie trie = range_filtering::Trie(keys);
        uint64_t prefix_cnt = 0;
        countKeys(trie, prefix_cnt);

        // calculate number of codes and r_ = number of bits per character
        r_ = bitsPerChar;
        elem_bits_ = r_ + 3;
        r_mask_ = LOW_MASK(r_);
        elem_mask_ = LOW_MASK(elem_bits_);

        // Initialize table
        slots_cnt_ = prefix_cnt / filledInFraction;
        max_hash_value = 0.98 * slots_cnt_;
        uint64_t table_size_bits_ = slots_cnt_ * elem_bits_; // in bits

        size_t table_size_bytes = table_size_bits_ / 8;
        size_t table_size = (table_size_bits_ % 8) ? (table_size_bytes + 1) : table_size_bytes;
        table_ = (uint64_t *) calloc(table_size, 1);

        // Populate table with prefixes
        std::string prefix = "";
        populateFilter(trie.root, prefix);
    }

    void ReCHaREQ::countKeys(range_filtering::Trie &trie, uint64_t& prefix_cnt) {
        std::string new_string = "";
        traverseNode(trie.root, new_string, prefix_cnt);
    }

    void ReCHaREQ::traverseNode(range_filtering::Trie::TrieNode* node,
                                    std::string& prefix, uint64_t& prefix_cnt) {
        if (prefix.length() == top_layer_max_length_) {
            prefix_list_.push_back(prefix);
        }
        if (node->children.empty()) {
            if (prefix.length() < top_layer_max_length_) {
                prefix_list_.push_back(prefix);
            }
        }
        for (auto child : node->children) {
            auto new_prefix = prefix + child.first;
            prefix_cnt++;
            traverseNode(child.second, new_prefix, prefix_cnt);
        }
    }

    void ReCHaREQ::populateFilter(range_filtering::Trie::TrieNode* node, std::string& prefix) {
        if (prefix.length() < top_layer_max_length_) {
            for (auto child : node->children) {
                prefix += child.first;
                populateFilter(child.second, prefix);
                prefix.pop_back();
            }
        } else {
            uint32_t hashed_value = hashString(prefix);
            for (auto child : node->children) {
                insertElement(hashed_value, child.first);
                prefix += child.first;
                populateFilter(child.second, prefix);
                prefix.pop_back();
            }
        }
    }

    void ReCHaREQ::insertElement(uint64_t idx, char letter) {
        uint64_t value = encodeChar(letter) & r_mask_;
        uint64_t old_element = getElement(idx);
        uint64_t entry = (value << 3) & ~7;

        // Slot is empty, we can insert the key
        if (isEmptyElement(old_element)) {
            setElement(idx, setOccupied(entry));
            return;
        }

        if (!isOccupied(old_element)) {
            setElement(idx, setOccupied(old_element));
        }

        uint64_t start = findRunIndex(idx);
        uint64_t s = start;

        if (isOccupied(old_element)) {
            // Move cursor to the insert position in the run
            do {
                uint64_t currentValue = getValue(getElement(s));
                if (currentValue == value) {
                    // Value is already in the filter
                    return;
                } else if (currentValue > value) {
                    break;
                }
                s++;
            } while (isContinuation(getElement(s)));

            if (s == start) {
                // The old start of the run becomes a continuation, new element becomes new start
                uint64_t old_head = getElement(start);
                setElement(start, setContinuation(old_head));
            } else {
                // New element becomes a continuation
                entry = setContinuation(entry);
            }
        }

        // Set the shifted bit if we cannot use the canonical slot
        if (s != idx) {
            entry = setShifted(entry);
        }

        insertAndShift(s, entry);
    }

    uint64_t ReCHaREQ::getElement(uint64_t idx) {
        size_t bitpos = elem_bits_ * idx;
        size_t tabpos = bitpos / 64;
        size_t slotpos = bitpos % 64;
        int spillbits = (slotpos + elem_bits_) - 64;
        uint64_t elt = (table_[tabpos] >> slotpos) & elem_mask_;

        if (spillbits > 0) {
            ++tabpos;
            uint64_t x = table_[tabpos] & LOW_MASK(spillbits);
            elt |= x << (elem_bits_ - spillbits);
        }
        return elt;
    }

    void ReCHaREQ::setElement(uint64_t idx, uint64_t elt) {
        size_t bitpos = elem_bits_ * idx;
        size_t tabpos = bitpos / 64;
        size_t slotpos = bitpos % 64;
        int spillbits = (slotpos + elem_bits_) - 64;
        elt &= elem_mask_;
        table_[tabpos] &= ~(elem_mask_ << slotpos);
        table_[tabpos] |= elt << slotpos;
        if (spillbits > 0) {
            ++tabpos;
            table_[tabpos] &= ~LOW_MASK(spillbits);
            table_[tabpos] |= elt >> (elem_bits_ - spillbits);
        }
    }

    uint64_t ReCHaREQ::findRunIndex(uint64_t idx) {
        // Find the start of the cluster
        uint64_t b = idx;
        while (isShifted(getElement(b))) {
            b--;
        }

        // Find the start of the run for the index
        uint64_t s = b;
        while (b != idx) {
            do {
                s++;
            } while (isContinuation(getElement(s)));

            do {
                b++;
            } while (!isOccupied(getElement(b)));
        }
        return s;
    }

    void ReCHaREQ::insertAndShift(uint64_t s, uint64_t elt) {
        uint64_t prev;
        uint64_t curr = elt;
        bool empty;

        do {
            prev = getElement(s);
            empty = isEmptyElement(prev);
            if (!empty) {
                // Fix up is_shifted and is_occupied bits
                prev = setShifted(prev);
                if (isOccupied(prev)) {
                    curr = setOccupied(curr);
                    prev = clrOccupied(prev);
                }
            }
            setElement(s, curr);
            curr = prev;
            s++;
        } while (!empty);
    }

    bool ReCHaREQ::lookupRange(const std::string &leftKey, const std::string &rightKey) {
        if (leftKey > rightKey) return false;

        bool isInTopLayer;
        std::string prefixInTopLayer;
        std::tie(isInTopLayer, prefixInTopLayer) = queryTopLayer(leftKey, rightKey);
        if (!isInTopLayer) return false;
        if (isInTopLayer && prefixInTopLayer.empty()) return true;

        std::string common_prefix;
        for (size_t i = 0; i < std::min(leftKey.length(), rightKey.length()); i++) {
            if (leftKey[i] != rightKey[i]) break;
            common_prefix += leftKey[i];
        }

        // If common prefix is shorter than prefix in top layer, we can tighten the range
        std::string newRightKey = rightKey;
        if (common_prefix.length() < prefixInTopLayer.length()) {
            newRightKey = prefixInTopLayer + char(127);
            common_prefix = prefixInTopLayer;
        }

        // All the prefixes in the common_prefix should exist in BF
        for (size_t i = prefixInTopLayer.length(); i < common_prefix.length(); i++) {
            uint64_t hash = hashString(prefixInTopLayer);
            if (!lookupLetter(hash, common_prefix[i])) return false;
            prefixInTopLayer = common_prefix.substr(0, i + 1);
        }

        return lookupRangeRecursively(leftKey, newRightKey);
    }

    bool ReCHaREQ::lookupRangeRecursively(const std::string &leftKey, const std::string &rightKey) {
        std::string common_prefix;
        for (size_t i = 0; i < std::min(leftKey.length(), rightKey.length()); i++) {
            if (leftKey[i] != rightKey[i]) break;
            common_prefix += leftKey[i];
        }

        if (leftKey.length() == rightKey.length() &&
            (common_prefix.length() == leftKey.length() - 1 || common_prefix.length() == leftKey.length())) {
            std::string prefix = leftKey.substr(0, leftKey.length() - 1);
            uint64_t hash = hashString(prefix);
            return lookupRangeBetweenLetters(hash, leftKey.at(leftKey.length() - 1), rightKey.at(rightKey.length() - 1));
        }

        std::string prefix = leftKey.substr(0, leftKey.length() - 1);
        uint64_t hash = hashString(prefix);
        if (leftKey.length() - common_prefix.length() > 1) {
            if (lookupRangeBetweenLetters(hash, leftKey.at(leftKey.length() - 1), char(127))) return true;

            char lastLetter = prefix.at(prefix.length() - 1) + 1;
            std::string newLeftKey = prefix.substr(0, prefix.length() - 1) + lastLetter;
            return lookupRangeRecursively(newLeftKey, rightKey);
        }

        if (leftKey.length() - common_prefix.length() == 0) {
            return true; // We already checked that the common prefix is in the filter
        }

        if (leftKey.length() - common_prefix.length() <= 1) {
            if (lookupRangeBetweenLetters(hash, leftKey.at(leftKey.length() - 1), rightKey.at(common_prefix.length()))) return true;
            std::string newLeftKey = prefix + rightKey.at(common_prefix.length()) + char(0);
            return lookupRangeRecursively(newLeftKey, rightKey);
        }
        return true;
    }

    uint64_t ReCHaREQ::hashString(std::string &s) {
        uint32_t hashed_value;
        MurmurHash3_x86_32(s.c_str(), s.size(), 0, (void*) &hashed_value);
        hashed_value %= max_hash_value;
        return hashed_value;
    }

    // pair.first: found range
    // pair.second: if possible false positive, return prefix to start searching in QF with
    std::pair<bool, std::string> ReCHaREQ::queryTopLayer(const std::string &leftKey, const std::string &rightKey) {
        int64_t x = findPosPrefixInTopLayer(leftKey);
        int64_t y = findPosPrefixInTopLayer(rightKey);

        if (y == -1) return std::make_pair(false, std::string());
        if (x < y) return std::make_pair(true, std::string());

        if (leftKey == prefix_list_[x]) return std::make_pair(true, std::string());
        if (leftKey.substr(0, prefix_list_[x].length()) == prefix_list_[x]) return std::make_pair(true, prefix_list_[x]);
        return std::make_pair(false, std::string());
    }

    int64_t ReCHaREQ::findPosPrefixInTopLayer(const std::string &key) {
        int a = 0, b = prefix_list_.size() - 1;
        while (a <= b) {
            int k = (a + b) / 2;
            if (prefix_list_[k] <= key && (k + 1 >= prefix_list_.size() || prefix_list_[k + 1] > key)) {
                return k;
            }
            if (prefix_list_[k] > key) b = k - 1;
            else a = k + 1;
        }
        return -1;
    }

    bool ReCHaREQ::lookupLetter(uint64_t idx, char letter) {
        uint64_t encodedLetter = encodeChar(letter);

        uint64_t element = getElement(idx);

        // If this quotient has no run, range is definitely empty
        if (!isOccupied(element)) return false;

        // Scan the sorted run for the range
        uint64_t s = findRunIndex(idx);
        do {
            uint64_t value = getValue(getElement(s));
            if (value == encodedLetter) {
                return true;
            } else if (value > encodedLetter) {
                return false;
            }
            s++;
        } while (isContinuation(getElement(s)));
        return false;
    }

    bool ReCHaREQ::lookupRangeBetweenLetters(uint64_t idx, char letterLeft, char letterRight) {
        uint64_t leftValue = encodeChar(letterLeft);
        uint64_t rightValue = encodeChar(letterRight);

        uint64_t element = getElement(idx);

        // If this quotient has no run, range is definitely empty
        if (!isOccupied(element)) return false;

        // Scan the sorted run for the range
        uint64_t s = findRunIndex(idx);
        do {
            uint64_t value = getValue(getElement(s));
            if (value >= leftValue && value <= rightValue) {
                return true;
            } else if (value > rightValue) {
                return false;
            }
            s++;
        } while (isContinuation(getElement(s)));
        return false;
    }

    uint64_t ReCHaREQ::getMemoryUsage() const {
        uint64_t total_bits = 64 + 8 + 64 + 32 + 64 + 32 // masks, counters etc
                              + slots_cnt_ * elem_bits_;
        uint64_t prefix_vec_size = 0;
        for (const auto& el : prefix_list_) {
            prefix_vec_size += el.length();
        }
        return total_bits / 8 + prefix_vec_size;
    }

}

#endif //RANGE_FILTERING_RECHAREQ_HPP
