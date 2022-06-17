#ifndef RANGE_FILTERING_LAYEREDCHAREQ_HPP
#define RANGE_FILTERING_LAYEREDCHAREQ_HPP

#include <vector>
#include <string>
#include "Trie.hpp"
#include <MurmurHash3.h>
#include "RangeFilter.h"
#include "bitset"
#include <cassert>
#include <cmath>
#include <cstdint>
#include "popcount.hpp"

#define LOW_MASK(n) ((1ULL << (n)) - 1ULL)

namespace range_filtering {
    class LayeredCHaREQ : public RangeFilter {
    public:
        LayeredCHaREQ(std::vector<std::string> &keys, uint32_t firstLayerHeight,
                      float firstLayerSaturation, float secondLayerSaturation,
                      uint32_t firstLayerCulledBits, uint32_t secondLayerCulledBits, uint32_t lut_block_size = 512);
        bool lookupRange(const std::string& leftKey, const std::string& rightKey) override;
        [[nodiscard]] uint64_t getMemoryUsage() const override;

    private:
        class CompactHashTable {
        public:
            CompactHashTable() {};
            CompactHashTable(LayeredCHaREQ* chareq, uint64_t prefix_cnt, float saturation, uint64_t r, uint64_t full_code_size) {
                chareq_ = chareq;
                r_ = r;
                full_code_size_ = full_code_size;
                r_mask_ = LOW_MASK(r);
                slots_cnt_ = prefix_cnt / saturation;
                uint64_t bitmap_size_bits = slots_cnt_ + trailing_bits_;
                uint64_t bitmap_size_bytes = bitmap_size_bits / 8;
                if (bitmap_size_bits % 8) bitmap_size_bytes++;
                occupied_bitmap_ = (uint64_t *) calloc(bitmap_size_bytes, 1);
                continuation_bitmap_ = (uint64_t *) calloc(bitmap_size_bytes, 1);
                shifted_bitmap_ = (uint64_t *) calloc(bitmap_size_bytes, 1);
            };
            ~CompactHashTable() {
                if (populatedValueTable) {
                    free(occupied_bitmap_);
                    free(continuation_bitmap_);
                    free(shifted_bitmap_);
                    free(values_table_);
                    free(rank_lut_);
                }
            }
            void populateValuesTable(std::vector<uint64_t> &temp_values_table, uint64_t prefix_cnt);
            void insertElement(uint64_t idx, char letter, std::vector<uint64_t> &temp_table);
            bool lookupLetter(uint64_t idx, char letter);
            bool lookupRangeBetweenLetters(uint64_t idx, char letterLeft, char letterRight);
            uint64_t findValueIndex(uint64_t idx);
            [[nodiscard]] uint64_t getMemoryUsage() const;

            uint64_t hashString(std::string& s);
            uint64_t *occupied_bitmap_;
            uint64_t *continuation_bitmap_;
            uint64_t *shifted_bitmap_;
            uint64_t *values_table_;
            uint32_t *rank_lut_;

            uint64_t slots_cnt_ = 0;
            uint64_t values_table_slots_cnt_ = 0; // stored just to be able to count mem usage

            uint64_t r_ = 0;
            uint64_t full_code_size_ = 0;
            uint64_t r_mask_ = 0;

        private:
            bool populatedValueTable = false;
            LayeredCHaREQ* chareq_;
            void createLUT();
            static bool getElementFromBitmap(uint64_t idx, uint64_t* bitmap);
            static void setElementInBitmap(uint64_t idx, uint64_t* bitmap);
            static void clearElementInBitmap(uint64_t idx, uint64_t* bitmap);
            uint64_t getElementFromTable(uint64_t idx);
            void setElementInTable(uint64_t idx, uint64_t value);
            bool isSlotEmpty(uint64_t idx);
            uint64_t findRunIndex(uint64_t idx);
            void insertAndShift(uint64_t idx, uint64_t value, bool isContinuation, bool isShifted, std::vector<uint64_t>& temp_table);

            inline uint64_t encodeChar(char ch) {
                if (ch == 0) return 0;
                if (ch == 127) return r_mask_;
                size_t shiftbits = 255 - int(ch);
                std::bitset<256> shiftedCharUsage = chareq_->char_usage_mask_ << shiftbits;
                std::bitset<256> shiftedZeroAfterOneMask = chareq_->zero_after_one_mask_ << shiftbits;
                auto code = shiftedCharUsage.count() + shiftedZeroAfterOneMask.count();
                return code >> (full_code_size_ - r_) & r_mask_;
            }

        };
        std::bitset<256> char_usage_mask_ = 0;
        std::bitset<256> zero_after_one_mask_ = 0;

        CompactHashTable cht1_;
        CompactHashTable cht2_;

        uint32_t lut_block_size_ = 512;
        const static uint64_t trailing_bits_ = 64;

        std::vector<char> first_level_;
        uint32_t first_layer_height_;

        static inline std::bitset<256> calculateOneToZeroMask(std::bitset<256> charUsageMask) { return (charUsageMask xor (charUsageMask << 1)) & ~charUsageMask; }
        void countKeysAndFillInCharUsageMask(range_filtering::Trie& trie, uint64_t& prefix_cnt_l1, uint64_t &prefix_cnt_l2);
        void traverseNode(range_filtering::Trie::TrieNode* node, std::string& prefix,
                          uint64_t& prefix_cnt_l1, uint64_t& prefix_cnt_l2);
        void populateBitmapsAndTempTable(range_filtering::Trie::TrieNode* node, std::vector<uint64_t> &temp_table_l1,
                                         std::vector<uint64_t> &temp_table_l2, std::string& prefix);

        std::tuple<bool, int32_t, int32_t> queryTopLayer(const std::string& leftKey, const std::string& rightKey);
        int64_t findPosPrefixInTopLayer(const std::string& key);
        bool lookupRangeRecursively(const std::string &leftKey, const std::string &rightKey);

    };

    range_filtering::LayeredCHaREQ::LayeredCHaREQ(std::vector<std::string> &keys, uint32_t firstLayerHeight,
                                                  float firstLayerSaturation, float secondLayerSaturation,
                                                  uint32_t firstLayerCulledBits, uint32_t secondLayerCulledBits,
                                                  uint32_t lut_block_size) {
        assert(!keys.empty());
        lut_block_size_ = lut_block_size;
        first_layer_height_ = firstLayerHeight;

        // Generate keys
        auto trie = Trie(keys);
        uint64_t prefix_cnt_l1 = 0;
        uint64_t prefix_cnt_l2 = 0;
        countKeysAndFillInCharUsageMask(trie, prefix_cnt_l1, prefix_cnt_l2);
        zero_after_one_mask_ = calculateOneToZeroMask(char_usage_mask_);

        // Calculate number of codes and r_ = number of bits per character
        uint64_t codes_cnt = 0;
        uint8_t last_bit = char_usage_mask_[0];
        for (size_t i = 0; i < 256; i++) {
            if (char_usage_mask_[i]) codes_cnt++;
            else codes_cnt += last_bit;
            last_bit = char_usage_mask_[i];
        }
        if (codes_cnt == 0) codes_cnt = 1;
        uint64_t full_code_bits = std::ceil(std::log2(codes_cnt));
        uint64_t r_l1 = full_code_bits - firstLayerCulledBits > 0 ? full_code_bits - firstLayerCulledBits : 1;
        uint64_t r_l2 = full_code_bits - secondLayerCulledBits > 0 ? full_code_bits - secondLayerCulledBits : 1;

        // Initialize compact hash tables
        cht1_ = CompactHashTable(this, prefix_cnt_l1, firstLayerSaturation, r_l1, full_code_bits);
        cht2_ = CompactHashTable(this, prefix_cnt_l2, secondLayerSaturation, r_l2, full_code_bits);
        std::vector<uint64_t> temp_values_table_l1 = std::vector<uint64_t>((prefix_cnt_l1 / firstLayerSaturation) + trailing_bits_);
        std::vector<uint64_t> temp_values_table_l2 = std::vector<uint64_t>((prefix_cnt_l2 / secondLayerSaturation) + trailing_bits_);

        // Populate bitmaps and temp tables
        std::string prefix;
        populateBitmapsAndTempTable(trie.root, temp_values_table_l1, temp_values_table_l2, prefix);

        cht1_.populateValuesTable(temp_values_table_l1, prefix_cnt_l1);
        cht2_.populateValuesTable(temp_values_table_l2, prefix_cnt_l2);
    }

    void LayeredCHaREQ::CompactHashTable::populateValuesTable(std::vector<uint64_t> &temp_values_table, uint64_t prefix_cnt) {
        populatedValueTable = true;
        createLUT();
        values_table_slots_cnt_ = prefix_cnt;
        uint64_t table_size_bits = prefix_cnt * r_;
        uint64_t table_size_bytes = table_size_bits / 8;
        if (table_size_bits % 8) table_size_bytes++;
        values_table_ = (uint64_t *) calloc(table_size_bytes, 1);
        uint64_t curr = 0;
        for (uint64_t i = 0; i < temp_values_table.size(); i++) {
            if (!isSlotEmpty(i)) setElementInTable(curr++, temp_values_table[i]);
        }
    }

    void LayeredCHaREQ::countKeysAndFillInCharUsageMask(range_filtering::Trie &trie, uint64_t &prefix_cnt_l1,
                                                        uint64_t &prefix_cnt_l2) {
        std::string new_string;
        traverseNode(trie.root, new_string, prefix_cnt_l1, prefix_cnt_l2);
    }

    void LayeredCHaREQ::traverseNode(range_filtering::Trie::TrieNode *node, std::string &prefix, uint64_t &prefix_cnt_l1,
                                uint64_t &prefix_cnt_l2) {
        if (prefix.length() == 1) {
            first_level_.push_back(prefix.at(0));
        }
        for (auto child : node->children) {
            auto new_prefix = prefix + child.first;
            if (new_prefix.length() <= first_layer_height_) prefix_cnt_l1++; else prefix_cnt_l2++;
            char_usage_mask_[int(child.first)] = true;
            traverseNode(child.second, new_prefix, prefix_cnt_l1, prefix_cnt_l2);
        }
    }

    uint64_t LayeredCHaREQ::CompactHashTable::hashString(std::string &s) {
        uint32_t hashed_value;
        MurmurHash3_x86_32(s.c_str(), s.size(), 0, (void*) &hashed_value);
        hashed_value %= slots_cnt_;
        return hashed_value;
    }

    void LayeredCHaREQ::populateBitmapsAndTempTable(range_filtering::Trie::TrieNode *node,
                                                    std::vector<uint64_t> &temp_table_l1,
                                                    std::vector<uint64_t> &temp_table_l2, std::string &prefix) {
        if (prefix == "acatal") {
            int a = 1;
        }
        if (prefix.length() < 1) {
            for (auto child : node->children) {
                prefix += child.first;
                populateBitmapsAndTempTable(child.second, temp_table_l1, temp_table_l2, prefix);
                prefix.pop_back();
            }
        } else if (prefix.length() < first_layer_height_ ){
            uint32_t hashed_value = cht1_.hashString(prefix);
            for (auto child : node->children) {
                cht1_.insertElement(hashed_value, child.first, temp_table_l1);
                prefix += child.first;
                populateBitmapsAndTempTable(child.second, temp_table_l1, temp_table_l2, prefix);
                prefix.pop_back();
            }
        } else {
            uint32_t hashed_value = cht2_.hashString(prefix);
            for (auto child : node->children) {
                cht2_.insertElement(hashed_value, child.first, temp_table_l2);
                prefix += child.first;
                populateBitmapsAndTempTable(child.second, temp_table_l1, temp_table_l2, prefix);
                prefix.pop_back();
            }
        }
    }

    bool LayeredCHaREQ::CompactHashTable::getElementFromBitmap(uint64_t idx, uint64_t *bitmap) {
        uint64_t block_id = idx / 64;
        uint64_t offset = idx % 64;
        uint64_t bit = (bitmap[block_id] >> offset) & LOW_MASK(1);
        return bit;
    }

    void LayeredCHaREQ::CompactHashTable::setElementInBitmap(uint64_t idx, uint64_t *bitmap) {
        uint64_t block_id = idx / 64;
        uint64_t offset = idx % 64;
        bitmap[block_id] |= (uint64_t(1) << offset);
    }

    void LayeredCHaREQ::CompactHashTable::clearElementInBitmap(uint64_t idx, uint64_t *bitmap) {
        uint64_t block_id = idx / 64;
        uint64_t offset = idx % 64;
        bitmap[block_id] &= ~(uint64_t(1) << offset);
    }

    uint64_t LayeredCHaREQ::CompactHashTable::getElementFromTable(uint64_t idx) {
        uint64_t pos = r_ * idx;
        uint64_t block_id = pos / 64;
        uint64_t offset = pos % 64;
        int64_t spilled_bits = (offset + r_) - 64;
        uint64_t value = (values_table_[block_id] >> offset) & r_mask_;

        if (spilled_bits > 0) {
            ++block_id;
            uint64_t x = values_table_[block_id] & LOW_MASK(spilled_bits);
            value |= x << (r_ - spilled_bits);
        }
        return value;
    }

    void LayeredCHaREQ::CompactHashTable::setElementInTable(uint64_t idx, uint64_t value) {
        uint64_t pos = r_ * idx;
        uint64_t block_id = pos / 64;
        uint64_t offset = pos % 64;
        int64_t spilled_bits = (offset + r_) - 64;
        value &= r_mask_;
        values_table_[block_id] &= ~(r_ << offset);
        values_table_[block_id] |= value << offset;
        if (spilled_bits > 0) {
            ++block_id;
            values_table_[block_id] &= ~LOW_MASK(spilled_bits);
            values_table_[block_id] |= value >> (r_ - spilled_bits);
        }
    }

    void LayeredCHaREQ::CompactHashTable::insertElement(uint64_t idx, char letter, std::vector<uint64_t> &temp_table) {
        assert(idx <= slots_cnt_ + trailing_bits_);
        uint64_t value = encodeChar(letter) & r_mask_;

        // Slot at idx is empty, we can insert the key
        if (isSlotEmpty(idx)) {
            setElementInBitmap(idx, occupied_bitmap_);
            temp_table[idx] = value;
            return;
        }

        bool isContinuation = false;
        bool isShifted = false;
        bool isOccupied = getElementFromBitmap(idx, occupied_bitmap_);

        if (!isOccupied) {
            setElementInBitmap(idx, occupied_bitmap_);
        }

        uint64_t start = findRunIndex(idx);
        uint64_t s = start;

        // Move cursor to the insert position in the run
        if (isOccupied) {
            do {
                uint64_t currentValue = temp_table[s];
                if (currentValue == value) return; // Value already in the filter
                else if (currentValue > value) break;
                s++;
                assert(s < slots_cnt_ + trailing_bits_);
            } while (getElementFromBitmap(s, continuation_bitmap_));



            if (s == start) {
                // The old start of the run becomes a continuation, new element becomes new start
                setElementInBitmap(start, continuation_bitmap_);
            } else {
                isContinuation = true;
            }
        }

        // Set the shifted bit if we cannot use the canonical slot
        if (s != idx) {
            isShifted = true;
        }

        insertAndShift(s, value, isContinuation, isShifted, temp_table);
    }

    bool LayeredCHaREQ::CompactHashTable::isSlotEmpty(uint64_t idx) {
        return !getElementFromBitmap(idx, occupied_bitmap_)
               && !getElementFromBitmap(idx, continuation_bitmap_)
               && !getElementFromBitmap(idx, shifted_bitmap_);
    }

    uint64_t LayeredCHaREQ::CompactHashTable::findRunIndex(uint64_t idx) {
        // Find the start of the cluster
        uint64_t b = idx;
        while (getElementFromBitmap(b, shifted_bitmap_)) b--;

        // Find the start of the run for the index
        uint64_t s = b;
        while (b != idx) {
            do { s++; } while (getElementFromBitmap(s, continuation_bitmap_));
            do { b++; } while (!getElementFromBitmap(b, occupied_bitmap_));
        }
        return s;
    }

    void LayeredCHaREQ::CompactHashTable::insertAndShift(uint64_t idx, uint64_t value, bool isContinuation,
                                                         bool isShifted, std::vector<uint64_t> &temp_table) {
        bool isOccupied = false;
        bool isPrevOccupied = false; bool isPrevShifted = false; bool isPrevContinuation = false;
        uint64_t prevValue;
        bool empty = false;

        do {
            assert(idx < slots_cnt_ + trailing_bits_);
            isPrevOccupied = getElementFromBitmap(idx, occupied_bitmap_);
            isPrevContinuation = getElementFromBitmap(idx, continuation_bitmap_);
            isPrevShifted = getElementFromBitmap(idx, shifted_bitmap_);
            prevValue = temp_table[idx];
            empty = !isPrevOccupied && !isPrevContinuation && !isPrevShifted;
            if (!empty) {
                isPrevShifted = true;
                if (isPrevOccupied) {
                    isOccupied = true;
                    isPrevOccupied = false;
                }
            }
            if (isOccupied) setElementInBitmap(idx, occupied_bitmap_);
            else clearElementInBitmap(idx, occupied_bitmap_);
            if (isContinuation) setElementInBitmap(idx, continuation_bitmap_);
            else clearElementInBitmap(idx, continuation_bitmap_);
            if (isShifted) setElementInBitmap(idx, shifted_bitmap_);
            else clearElementInBitmap(idx, shifted_bitmap_);
            temp_table[idx] = value;

            // Update current values with prev values
            isOccupied = isPrevOccupied;
            isContinuation = isPrevContinuation;
            isShifted = isPrevShifted;
            value = prevValue;

            idx++;
        } while (!empty);
    }

    void LayeredCHaREQ::CompactHashTable::createLUT() {
        uint64_t bitmap_size_bits = slots_cnt_ + trailing_bits_;
        uint64_t words_per_block = chareq_->lut_block_size_ / 64;
        uint64_t blocks_cnt = bitmap_size_bits / chareq_->lut_block_size_;
        if (bitmap_size_bits % chareq_->lut_block_size_) blocks_cnt++;
        rank_lut_ = (uint32_t *) calloc(blocks_cnt, sizeof(uint32_t));

        // Create an array from isOccupied | isShifted
        uint64_t bitmap_size_blocks = bitmap_size_bits / 64;
        if (bitmap_size_bits % 64) bitmap_size_blocks++;
        uint64_t* bits = (uint64_t *) calloc(bitmap_size_blocks, sizeof(uint64_t));
        for (uint64_t i = 0; i < bitmap_size_blocks; i++) {
            bits[i] = occupied_bitmap_[i] | shifted_bitmap_[i];
        }

        uint32_t cumulative_rank = 0;
        for (uint32_t i = 0; i < blocks_cnt - 1; i++) {
            rank_lut_[i] = cumulative_rank;
            cumulative_rank += getpopcount(bits, i * words_per_block, chareq_->lut_block_size_);
        }
        rank_lut_[blocks_cnt - 1] = cumulative_rank;

        free(bits);
    }

    uint64_t LayeredCHaREQ::CompactHashTable::findValueIndex(uint64_t idx) {
        if (idx == 0) return 0;
        assert(idx <= slots_cnt_ + trailing_bits_);
        uint64_t words_per_block = chareq_->lut_block_size_ / 64;
        uint64_t lut_block_id = idx / chareq_->lut_block_size_;
        uint64_t offset = idx & (chareq_->lut_block_size_ - 1);

        // We need to create array with remaining bits
        uint64_t remaining_bits_size_blocks = (offset + 1) / 64;
        if ((offset + 1) % 64) remaining_bits_size_blocks++;
        uint64_t* bits = (uint64_t *) calloc(remaining_bits_size_blocks, 8);
        for (uint32_t i = 0; i < remaining_bits_size_blocks; i++) {
            bits[i] = occupied_bitmap_[lut_block_id * words_per_block + i]
                      | shifted_bitmap_[lut_block_id * words_per_block + i];
        }
        uint64_t result = rank_lut_[lut_block_id] + getpopcount(bits, 0, offset + 1) - 1;
        free(bits);
        return result;
    }

    bool LayeredCHaREQ::lookupRange(const std::string &leftKey, const std::string &rightKey) {
        if (leftKey > rightKey) return false;

        bool isInTopLayer;
        int32_t topLayerX; int32_t topLayerY;
        std::tie(isInTopLayer, topLayerX, topLayerY) = queryTopLayer(leftKey, rightKey);
        if (!isInTopLayer) return false;
        if (isInTopLayer && topLayerX == -1) return true;

        std::string common_prefix;
        for (size_t i = 0; i < std::min(leftKey.length(), rightKey.length()); i++) {
            if (leftKey[i] != rightKey[i]) break;
            common_prefix += leftKey[i];
        }

        if (common_prefix.empty()) {
            // This will only occur if there are two subtrees to look down at, root at leftKey[0] and rightKey[0]
            // (otherwise we would have already returned result based on the top layer only)
            std::string rightKeyLeftSubtree = leftKey.substr(0, 1);
            rightKeyLeftSubtree += char(127);
            std::string leftKeyRightSubtree = rightKey.substr(0, 1);
            leftKeyRightSubtree += char(0);
            return lookupRangeRecursively(leftKey, rightKeyLeftSubtree) || lookupRangeRecursively(leftKeyRightSubtree, rightKey);
        }

        // All the prefixes in the common_prefix should exist in BF
        for (size_t i = 1; i < common_prefix.length(); i++) {
            std::string prefix = common_prefix.substr(0, i);
            if (prefix.length() < first_layer_height_) {
                uint64_t hash = cht1_.hashString(prefix);
                if (!cht1_.lookupLetter(hash, common_prefix[i])) return false;
            } else {
                uint64_t hash = cht2_.hashString(prefix);
                if (!cht2_.lookupLetter(hash, common_prefix[i])) return false;
            }
        }

        return lookupRangeRecursively(leftKey, rightKey);
    }

    std::tuple<bool, int32_t, int32_t>
    LayeredCHaREQ::queryTopLayer(const std::string &leftKey, const std::string &rightKey) {
        int64_t x = findPosPrefixInTopLayer(leftKey);
        int64_t y = findPosPrefixInTopLayer(rightKey);

        if (y == -1) return std::make_tuple(false, -1, -1);
        if (x == y) {
            if (leftKey.at(0) != first_level_[x]) return std::make_tuple(false, -1, -1);
            else return std::make_tuple(true, x, y);
        }
        if (y - x > 1) return std::make_tuple(true, -1, -1);

        if (rightKey.at(0) > first_level_.at(y)) return std::make_tuple(true, -1, -1);
        return std::make_tuple(true, x, y);
    }

    int64_t LayeredCHaREQ::findPosPrefixInTopLayer(const std::string &key) {
        int a = 0, b = first_level_.size() - 1;
        while (a <= b) {
            int k = (a + b) / 2;
            if (first_level_[k] <= key.at(0) && (k + 1 >= first_level_.size() || first_level_[k + 1] > key.at(0))) {
                return k;
            }
            if (first_level_[k] > key.at(0)) b = k - 1;
            else a = k + 1;
        }
        return -1;
    }

    bool LayeredCHaREQ::lookupRangeRecursively(const std::string &leftKey, const std::string &rightKey) {
        std::string common_prefix;
        for (size_t i = 0; i < std::min(leftKey.length(), rightKey.length()); i++) {
            if (leftKey[i] != rightKey[i]) break;
            common_prefix += leftKey[i];
        }

        if (leftKey.length() == rightKey.length() &&
            (common_prefix.length() == leftKey.length() - 1 || common_prefix.length() == leftKey.length())) {
            std::string prefix = leftKey.substr(0, leftKey.length() - 1);
            if (prefix.length() < first_layer_height_) {
                uint64_t hash = cht1_.hashString(prefix);
                return cht1_.lookupRangeBetweenLetters(hash, leftKey.at(leftKey.length() - 1),
                                                 rightKey.at(rightKey.length() - 1));
            } else {
                uint64_t hash = cht2_.hashString(prefix);
                return cht2_.lookupRangeBetweenLetters(hash, leftKey.at(leftKey.length() - 1),
                                                       rightKey.at(rightKey.length() - 1));
            }
        }

        std::string prefix = leftKey.substr(0, leftKey.length() - 1);

        if (leftKey.length() - common_prefix.length() > 1) {
            if (prefix.length() < first_layer_height_) {
                uint64_t hash = cht1_.hashString(prefix);
                if (cht1_.lookupRangeBetweenLetters(hash, leftKey.at(leftKey.length() - 1), char(127))) return true;
            } else {
                uint64_t hash = cht2_.hashString(prefix);
                if (cht2_.lookupRangeBetweenLetters(hash, leftKey.at(leftKey.length() - 1), char(127))) return true;
            }

            char lastLetter = prefix.at(prefix.length() - 1) + 1;
            std::string newLeftKey = prefix.substr(0, prefix.length() - 1) + lastLetter;
            return lookupRangeRecursively(newLeftKey, rightKey);
        }

        if (leftKey.length() - common_prefix.length() == 0) {
            return true; // We already checked that the common prefix is in the filter
        }

        if (leftKey.length() - common_prefix.length() <= 1) {
            if (prefix.length() < first_layer_height_) {
                uint64_t hash = cht1_.hashString(prefix);
                if (cht1_.lookupRangeBetweenLetters(hash, leftKey.at(leftKey.length() - 1),
                                              rightKey.at(common_prefix.length())))
                    return true;
            } else {
                uint64_t hash = cht2_.hashString(prefix);
                if (cht2_.lookupRangeBetweenLetters(hash, leftKey.at(leftKey.length() - 1),
                                                    rightKey.at(common_prefix.length())))
                    return true;
            }
            std::string newLeftKey = prefix + rightKey.at(common_prefix.length()) + char(0);
            return lookupRangeRecursively(newLeftKey, rightKey);
        }
        return true;
    }

    bool LayeredCHaREQ::CompactHashTable::lookupLetter(uint64_t idx, char letter) {
        uint64_t encodedLetter = encodeChar(letter);

        // If there is no run at this index, letter is not in filter
        if (!getElementFromBitmap(idx, occupied_bitmap_)) return false;

        // Scan the sorted run for the range
        uint64_t s = findRunIndex(idx);
        uint64_t valueS = findValueIndex(s);
        do {
            uint64_t value = getElementFromTable(valueS);
            if (value == encodedLetter) {
                return true;
            } else if (value > encodedLetter) {
                return false;
            }
            s++;
            valueS++;
        } while (getElementFromBitmap(s, continuation_bitmap_));
        return false;
    }

    bool LayeredCHaREQ::CompactHashTable::lookupRangeBetweenLetters(uint64_t idx, char letterLeft, char letterRight) {
        uint64_t leftValue = encodeChar(letterLeft);
        uint64_t rightValue = encodeChar(letterRight);

        // If there is no run at this index, range is definitely empty
        if (!getElementFromBitmap(idx, occupied_bitmap_)) return false;

        // Scan the sorted run for the range
        uint64_t s = findRunIndex(idx);
        uint64_t valueS = findValueIndex(s);
        do {
            uint64_t value = getElementFromTable(valueS);
            if (value >= leftValue && value <= rightValue) {
                return true;
            } else if (value > rightValue) {
                return false;
            }
            s++;
            valueS++;
        } while (getElementFromBitmap(s, continuation_bitmap_));
        return false;
    }

    uint64_t LayeredCHaREQ::CompactHashTable::getMemoryUsage() const {
        uint64_t bits_cnt = 0;
        bits_cnt += 32; //r_
        bits_cnt += 3* slots_cnt_; // 3 bitmpas
        bits_cnt += r_ * values_table_slots_cnt_; // values table
        return bits_cnt / 8;
    }
    uint64_t LayeredCHaREQ::getMemoryUsage() const {
        return 64 + cht1_.getMemoryUsage() + cht2_.getMemoryUsage() + first_level_.size();
    }
}
#endif //RANGE_FILTERING_LAYEREDCHAREQ_HPP
