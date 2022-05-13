#ifndef RANGE_FILTERING_QUOTIENTTRIE_HPP
#define RANGE_FILTERING_QUOTIENTTRIE_HPP

#include <vector>
#include <string>
#include "RestrainedSplashyTrie.h"
#include "bitset"
#include <MurmurHash3.h>

namespace range_filters {
    enum RestraintType {none, absolute, relative};
    class QuotientTrie {
    public:
        QuotientTrie(std::vector<std::string>& keys, uint32_t topLayerMaxLength, float filledInFraction,
                     RestraintType restraintType, uint64_t absoluteRestraintType, double relativeRestraintType,
                     double cutOffCoefficient);
        bool lookupRange(const std::string& leftKey, const std::string& rightKey);
        uint64_t getMemoryUsage();

    private:
        std::vector<bool> remainder_table_;
        std::vector<bool> is_occupied_;
        std::vector<bool> is_continuation_;
        std::vector<bool> is_shifted_;
        std::bitset<256> charUsageMask_;

        uint32_t r_ = 0;
        uint64_t arr_size_;
        uint32_t top_layer_max_length_;

        std::vector<std::string> prefix_list_;

        std::bitset<256> calculateOneToZeroMask() { return (charUsageMask_ xor (charUsageMask_ << 1)) & ~charUsageMask_; }

        void insertKey(const std::string& key);
        void countKeys(range_filtering::RestrainedSplashyTrie trie, uint64_t& prefix_cnt);
        void traverseNode(range_filtering::RestrainedSplashyTrie::TrieNode* node,
                          std::string& prefix, uint64_t& prefix_cnt);
        void populateFilter(range_filtering::RestrainedSplashyTrie::TrieNode* node, std::string& prefix, std::bitset<256> zeroAfterOneMask);
        uint8_t calculateCode(char ch, std::bitset<256> zeroAfterOneMask);
        void insertElement(uint32_t idx, char letter, std::bitset<256> zeroAfterOneMask);
        void insertToRemainderTable(uint32_t idx, uint8_t code);
        uint8_t getValueFromRemainderTable(uint32_t idx);
    };

    range_filters::QuotientTrie::QuotientTrie(std::vector<std::string> &keys, uint32_t topLayerMaxLength,
                                              float filledInFraction, RestraintType restraintType,
                                              uint64_t absoluteRestraintType, double relativeRestraintType,
                                              double cutOffCoefficient) {
        top_layer_max_length_ = topLayerMaxLength;
        prefix_list_ = std::vector<std::string>();

        // generate keys
        range_filtering::RestrainedSplashyTrie restrained_trie;
        if (restraintType == RestraintType::relative) {
            restrained_trie = range_filtering::RestrainedSplashyTrie(keys, 0, cutOffCoefficient,
                                                                          range_filtering::RestraintType::relative,
                                                                          0, relativeRestraintType);

        } else {
            restrained_trie = range_filtering::RestrainedSplashyTrie(keys, 0, cutOffCoefficient,
                                                                     range_filtering::RestraintType::absolute,
                                                                     absoluteRestraintType, 0);
        }
        uint64_t prefix_cnt = 0;
        countKeys(restrained_trie, prefix_cnt);

        auto last_bit = charUsageMask_[0];
        for (size_t i = 0; i < charUsageMask_.size(); i++) {
            if (charUsageMask_[i]) r_++;
            else r_ += last_bit;
            last_bit = charUsageMask_[i];
        }

        // initialize quotient filter
        arr_size_ = (prefix_cnt + 0.0) / filledInFraction;
        remainder_table_ = std::vector<bool>(arr_size_ * r_);
        is_occupied_ = std::vector<bool>(arr_size_);
        is_continuation_ = std::vector<bool>(arr_size_);
        is_shifted_ = std::vector<bool>(arr_size_);

        std::string prefix = "";
        auto zeroAfterOneMask = calculateOneToZeroMask();
        populateFilter(restrained_trie.root, prefix, zeroAfterOneMask);
    }

    void QuotientTrie::countKeys(range_filtering::RestrainedSplashyTrie trie, uint64_t& prefix_cnt) {
        std::string new_string = "";
        traverseNode(trie.root, new_string, prefix_cnt);
    }

    void QuotientTrie::traverseNode(range_filtering::RestrainedSplashyTrie::TrieNode* node,
                                    std::string& prefix, uint64_t& prefix_cnt) {
        if (prefix.length() == top_layer_max_length_) {
            prefix_list_.push_back(prefix);
        }
        if (node->children_.empty()) {
            if (prefix.length() < top_layer_max_length_) {
                prefix_list_.push_back(prefix);
            }
        }
        for (auto child : node->children_) {
            auto new_prefix = prefix + child.first;
            prefix_cnt++;
            charUsageMask_[int(child.first)] = true;
            traverseNode(child.second, new_prefix, prefix_cnt);
        }
    }

    void QuotientTrie::populateFilter(range_filtering::RestrainedSplashyTrie::TrieNode* node, std::string& prefix,
                                        std::bitset<256> zeroAfterOneMask) {
        if (prefix.length() < top_layer_max_length_) {
            for (auto child : node->children_) {
                prefix += child.first;
                populateFilter(child.second, prefix, zeroAfterOneMask);
                prefix.pop_back();
            }
        } else {
            uint32_t hashed_value;
            MurmurHash3_x86_32(prefix.c_str(), prefix.size(), 0, (void*) &hashed_value);
            hashed_value %= arr_size_;
            for (auto child : node->children_) {
                insertElement(hashed_value, child.first, zeroAfterOneMask);
                prefix += child.first;
                populateFilter(child.second, prefix, zeroAfterOneMask);
                prefix.pop_back();
            }
        }
    }

    void QuotientTrie::insertElement(uint32_t idx, char letter, std::bitset<256> zeroAfterOneMask) {
        uint8_t code = calculateCode(letter, zeroAfterOneMask);

        // Slot is empty, we can insert the key
        if (!is_occupied_[idx] && !is_continuation_[idx] && !is_shifted_[idx]) {
            insertToRemainderTable(idx, code);
            is_occupied_[idx] = true;
        }

        // locate the start of the cluster and count number of runs within cluster
        uint32_t pos = idx;
        uint32_t runs_count = 0;
        while (is_shifted_[pos]) {
            pos--;
            if (is_occupied_[pos]) {
                runs_count++;
            }
        }

        while (runs_count > 0) {
            pos++;
            if (!is_continuation_[pos]) {
                runs_count--;
            }
        };

        // pos now indicates the quotient's run
        do {
            // If the element has been already inserted, terminate
            if (getValueFromRemainderTable(pos) == code) return;
            pos++;
            if (pos >= remainder_table_.size()) {
                // Insert fails due to lack of space in the hash table, extend it
                for (size_t i = 0; i < 4; i++) {
                    arr_size_++;
                    for (size_t j = 0; j < r_; j++) {
                        remainder_table_.push_back(0);
                    }
                    is_occupied_.push_back(0);
                    is_continuation_.push_back(0);
                    is_shifted_.push_back(0);
                }
            }
        } while (is_continuation_[pos]);

        // Slot is empty, we can just insert the key
        if (!is_occupied_[pos] && !is_continuation_[pos] && !is_shifted_[pos]) {
            insertToRemainderTable(pos, code);
            is_continuation_[pos] = true;
            is_shifted_[pos] = true;
            is_occupied_[idx] = true;
            return;
        }

        // Otherwise, shift the consecutive elements at and after pos to the right
        uint32_t empty_pos = pos;
        while (is_occupied_[empty_pos] || is_continuation_[empty_pos] || is_shifted_[empty_pos]) {
            empty_pos++;
            if (empty_pos >= remainder_table_.size()) {
                // Insert fails due to lack of space in the hash table, extend it
                for (size_t i = 0; i < 4; i++) {
                    arr_size_++;
                    remainder_table_.push_back(0);
                    is_occupied_.push_back(0);
                    is_continuation_.push_back(0);
                    is_shifted_.push_back(0);
                }
            }
        }

        while (empty_pos > pos) {
            remainder_table_[empty_pos] = remainder_table_[empty_pos - 1];
            is_shifted_[empty_pos] = true;
            is_continuation_[empty_pos] = is_continuation_[empty_pos-1];
            empty_pos--;
        }

        // Finally, we can insert the new element
        insertToRemainderTable(pos, code);
        is_continuation_[pos] = true;
        is_shifted_[pos] = true;
        is_occupied_[idx] = true;
    }

    uint8_t QuotientTrie::calculateCode(char ch, std::bitset<256> zeroAfterOneMask) {
        auto shiftedCharUsage = charUsageMask_ << (size_t) (255 - int(ch));
        auto shiftedZeroAfterOneMask = zeroAfterOneMask << (size_t) (255 - int(ch));
        return shiftedCharUsage.count() + shiftedZeroAfterOneMask.count();
    }

    void QuotientTrie::insertToRemainderTable(uint32_t idx, uint8_t code) {
        // TODO
    }

    uint8_t QuotientTrie::getValueFromRemainderTable(uint32_t idx) {
        // TODO
    }

}


#endif //RANGE_FILTERING_QUOTIENTTRIE_HPP
