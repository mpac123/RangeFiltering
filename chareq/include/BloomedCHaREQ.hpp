#ifndef RANGE_FILTERING_BLOOMEDCHAREQ_HPP
#define RANGE_FILTERING_BLOOMEDCHAREQ_HPP

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
#include "BloomFilter.h"
#include <unordered_set>

#define LOW_MASK(n) ((1ULL << (n)) - 1ULL)

namespace range_filtering {
    class BloomedCHaREQ : public RangeFilter {
    public:
        BloomedCHaREQ(std::vector<std::string> &keys, float chareqSaturation,
                         uint64_t bfSize, uint32_t lut_block_size = 512);
        bool lookupRange(const std::string& leftKey, const std::string& rightKey) override;
        uint64_t getMemoryUsage() const override;

    private:
        class CharIterator {
        public:
            CharIterator(std::bitset<256> &char_usage_mask) : char_usage_mask_(char_usage_mask) {
                scanRightUntilNext();
            }
            CharIterator(std::bitset<256> &char_usage_mask, char ch) : char_usage_mask_(char_usage_mask) {
                scanRightUntilNext();
                while (hasNext() and i < int(ch)) {
                    scanRightUntilNext();
                }
            }
            bool hasNext() { return i < 256; }
            char next() {
                auto val = char(i);
                scanRightUntilNext();
                return val;
            }

        private:
            std::bitset<256> &char_usage_mask_;
            uint32_t i = 0;

            bool scanRightUntilNext() {
                i++;
                if (i > 255) {
                    return false;
                }
                while (!char_usage_mask_[i]) {
                    i++;
                    if (i > 255) {
                        return false;
                    }
                }
                return true;
            }
        };
        std::bitset<256> char_usage_mask_ = 0;
        std::bitset<256> zero_after_one_mask_ = 0;

        uint64_t *occupied_bitmap_;
        uint64_t *continuation_bitmap_;
        uint64_t *shifted_bitmap_;
        uint64_t *values_table_;
        uint32_t *rank_lut_;
        uint32_t lut_block_size_ = 512;

        uint64_t slots_cnt_ = 0;
        uint64_t values_table_slots_cnt_ = 0; // stored just to be able to count mem usage
        const static uint64_t trailing_bits_ = 32;

        uint64_t r_ = 0;
        uint64_t r_mask_ = 0;

        bloom_filter::BloomFilter bf_;

        static inline std::bitset<256> calculateOneToZeroMask(std::bitset<256> charUsageMask) { return (charUsageMask xor (charUsageMask << 1)) & ~charUsageMask; }
        void populatePrefixListAndFillInCharUsageMask(std::vector<std::string>& keys, std::vector<std::string> &prefixes);
        uint64_t hashString(std::string& s);
        void populateBitmapsAndTempTable(std::vector<std::string>& keys, std::vector<uint64_t> &temp_table);
        void insertElement(uint64_t idx, char letter, std::vector<uint64_t> &temp_table);

        bool getElementFromBitmap(uint64_t idx, uint64_t* bitmap);
        void setElementInBitmap(uint64_t idx, uint64_t* bitmap);
        void clearElementInBitmap(uint64_t idx, uint64_t* bitmap);
        uint64_t getElementFromTable(uint64_t idx);
        void setElementInTable(uint64_t idx, uint64_t value);
        bool isSlotEmpty(uint64_t idx);
        uint64_t findRunIndex(uint64_t idx);
        void insertAndShift(uint64_t idx, uint64_t value, bool isContinuation, bool isShifted, std::vector<uint64_t>& temp_table);

        inline uint64_t encodeChar(char ch) {
            if (ch == 0) return 0;
            if (ch == 127) return r_mask_;
            size_t shiftbits = 255 - int(ch);
            std::bitset<256> shiftedCharUsage = char_usage_mask_ << shiftbits;
            std::bitset<256> shiftedZeroAfterOneMask = zero_after_one_mask_ << shiftbits;
            return shiftedCharUsage.count() + shiftedZeroAfterOneMask.count();
        }

        uint64_t createLUTAndCountNotEmptySlots();
        void populateValuesTable(std::vector<uint64_t>& temp_table);
        std::tuple<bool, int32_t, int32_t> queryTopLayer(const std::string& leftKey, const std::string& rightKey);
        int64_t findPosPrefixInTopLayer(const std::string& key);
        //bool lookupRangeRecursively(const std::string &leftKey, const std::string &rightKey);
        bool lookupLetter(uint64_t idx, char letter);
        bool lookupRangeBetweenLetters(uint64_t idx, char letterLeft, char letterRight);
        uint64_t findValueIndex(uint64_t idx);
        bool lookupRecursively(std::string& prefix, char left, char right, uint32_t max_level);
    };

    range_filtering::BloomedCHaREQ::BloomedCHaREQ(std::vector<std::string> &keys, float chareqSaturation,
                                                  uint64_t bfSize, uint32_t lut_block_size) : bf_() {
        assert(keys.size() > 0);

        lut_block_size_ = lut_block_size;

        // Generate keys
        auto prefixes = std::vector<std::string>();
        populatePrefixListAndFillInCharUsageMask(keys, prefixes);
        zero_after_one_mask_ = calculateOneToZeroMask(char_usage_mask_);

        bf_ = bloom_filter::BloomFilter(prefixes, bfSize);

        // Calculate number of codes and r_ = number of bits per character
        uint64_t codes_cnt = 0;
        uint8_t last_bit = char_usage_mask_[0];
        for (size_t i = 0; i < 256; i++) {
            if (char_usage_mask_[i]) codes_cnt++;
            else codes_cnt += last_bit;
            last_bit = char_usage_mask_[i];
        }
        if (codes_cnt == 0) codes_cnt = 1;
        r_ = std::ceil(std::log2(codes_cnt));
        r_mask_ = LOW_MASK(r_);

        // Initialize bitmasks
        slots_cnt_ = keys.size() / chareqSaturation;
        uint64_t bitmap_size_bits = slots_cnt_ + trailing_bits_;
        uint64_t bitmap_size_bytes = bitmap_size_bits / 8;
        if (bitmap_size_bits % 8) bitmap_size_bytes++;
        occupied_bitmap_ = (uint64_t *) calloc(bitmap_size_bytes, 1);
        continuation_bitmap_ = (uint64_t *) calloc(bitmap_size_bytes, 1);
        shifted_bitmap_ = (uint64_t *) calloc(bitmap_size_bytes, 1);
        std::vector<uint64_t> temp_values_table = std::vector<uint64_t>(bitmap_size_bits);

        // Populate bitmaps and temp table with prefixes
        populateBitmapsAndTempTable(keys, temp_values_table);

        // Create LUT, count non-empty slots, and populate values table
        uint64_t entries_cnt = createLUTAndCountNotEmptySlots();
        values_table_slots_cnt_ = entries_cnt;
        uint64_t table_size_bits = entries_cnt * r_;
        uint64_t table_size_bytes = table_size_bits / 8;
        if (table_size_bits % 8) table_size_bytes++;
        values_table_ = (uint64_t *) calloc(table_size_bytes, 1);
        populateValuesTable(temp_values_table);
    }

    void BloomedCHaREQ::populatePrefixListAndFillInCharUsageMask(std::vector<std::string>& keys, std::vector<std::string> &prefixes) {
        std::unordered_set<std::string> all_prefixes = std::unordered_set<std::string>();
        for (auto key : keys) {
            for (size_t i = 1; i < key.length(); i++) {
                all_prefixes.insert(key.substr(0, i));
                char_usage_mask_[key[i - 1]] = true;
            }
            char_usage_mask_[key[key.length() - 1]] = true;
        }
        prefixes = std::vector<std::string>(all_prefixes.begin(), all_prefixes.end());
    }

    uint64_t BloomedCHaREQ::hashString(std::string &s) {
        uint32_t hashed_value;
        MurmurHash3_x86_32(s.c_str(), s.size(), 0, (void*) &hashed_value);
        hashed_value %= slots_cnt_;
        return hashed_value;
    }

    void BloomedCHaREQ::populateBitmapsAndTempTable(std::vector<std::string> &keys, std::vector<uint64_t> &temp_table) {
        for (auto key : keys) {
            auto prefix = key.substr(0, key.length() - 1);
            uint32_t hashed_prefix = hashString(prefix);
            insertElement(hashed_prefix, key[key.length() - 1], temp_table);
        }
    }

    bool BloomedCHaREQ::getElementFromBitmap(uint64_t idx, uint64_t *bitmap) {
        uint64_t block_id = idx / 64;
        uint64_t offset = idx % 64;
        uint64_t bit = (bitmap[block_id] >> offset) & LOW_MASK(1);
        return bit;
    }

    void BloomedCHaREQ::setElementInBitmap(uint64_t idx, uint64_t *bitmap) {
        uint64_t block_id = idx / 64;
        uint64_t offset = idx % 64;
        bitmap[block_id] |= (uint64_t(1) << offset);
    }

    void BloomedCHaREQ::clearElementInBitmap(uint64_t idx, uint64_t *bitmap) {
        uint64_t block_id = idx / 64;
        uint64_t offset = idx % 64;
        bitmap[block_id] &= ~(uint64_t(1) << offset);
    }

    uint64_t BloomedCHaREQ::getElementFromTable(uint64_t idx) {
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

    void BloomedCHaREQ::setElementInTable(uint64_t idx, uint64_t value) {
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

    void BloomedCHaREQ::insertElement(uint64_t idx, char letter, std::vector<uint64_t> &temp_table) {
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

    bool BloomedCHaREQ::isSlotEmpty(uint64_t idx) {
        return !getElementFromBitmap(idx, occupied_bitmap_)
               && !getElementFromBitmap(idx, continuation_bitmap_)
               && !getElementFromBitmap(idx, shifted_bitmap_);
    }

    uint64_t BloomedCHaREQ::findRunIndex(uint64_t idx) {
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

    void BloomedCHaREQ::insertAndShift(uint64_t idx, uint64_t value, bool isContinuation, bool isShifted,
                                       std::vector<uint64_t> &temp_table) {
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

    uint64_t BloomedCHaREQ::createLUTAndCountNotEmptySlots() {
        uint64_t bitmap_size_bits = slots_cnt_ + trailing_bits_;
        uint64_t words_per_block = lut_block_size_ / 64;
        uint64_t blocks_cnt = bitmap_size_bits / lut_block_size_;
        if (bitmap_size_bits % lut_block_size_) blocks_cnt++;
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
            cumulative_rank += getpopcount(bits, i * words_per_block, lut_block_size_);
        }
        rank_lut_[blocks_cnt - 1] = cumulative_rank;

        // Add remaining bits to the cumulative rank to get number of all the slots
        uint32_t remaining_bits = bitmap_size_bits % lut_block_size_;
        cumulative_rank += getpopcount(bits, (blocks_cnt - 1) * words_per_block, remaining_bits);

        free(bits);
        return cumulative_rank;
    }

    uint64_t BloomedCHaREQ::findValueIndex(uint64_t idx) {
        if (idx == 0) return 0;
        assert(idx <= slots_cnt_ + trailing_bits_);
        uint64_t words_per_block = lut_block_size_ / 64;
        uint64_t lut_block_id = idx / lut_block_size_;
        uint64_t offset = idx & (lut_block_size_ - 1);

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

    void BloomedCHaREQ::populateValuesTable(std::vector<uint64_t> &temp_table) {
        uint64_t curr = 0;
        for (uint64_t i = 0; i < temp_table.size(); i++) {
            if (!isSlotEmpty(i)) setElementInTable(curr++, temp_table[i]);
        }
    }

    bool BloomedCHaREQ::lookupRange(const std::string &leftKey, const std::string &rightKey) {
        if (leftKey > rightKey) return false;

        // First, check out the BF for the common prefix
        std::string common_prefix;
        for (size_t i = 0; i < std::min(leftKey.length(), rightKey.length()); i++) {
            if (leftKey[i] != rightKey[i]) break;
            common_prefix += leftKey[i];
        }

        for (size_t i = 1; i <= common_prefix.length(); i++) {
            auto prefix = common_prefix.substr(0, i);
            auto hashedPrefix = hashString(prefix);
            if (lookupLetter(hashedPrefix, common_prefix[i])) {
                return true;
            }
            if (!bf_.lookupKey(prefix)) {
                return false;
            }
        }

        // Find first char
        auto iter = new CharIterator(char_usage_mask_);
        char firstCh = iter->next();
        char lastCh = firstCh;
        while (iter->hasNext()) {
            lastCh = iter->next();
        }

        if (leftKey.length() == rightKey.length() && rightKey.length() == common_prefix.length() + 1) {
            auto hashedPrefix = hashString(common_prefix);
            if (lookupRangeBetweenLetters(hashedPrefix, leftKey[leftKey.length() - 1], rightKey[rightKey.length() - 1])) return true;
            iter = new CharIterator(char_usage_mask_, leftKey[leftKey.length() - 1]);
            char nextChar;
            do {
                nextChar = iter->next();
                std::string newKey = common_prefix + nextChar;
                if (bf_.lookupKey(newKey)) {
                    if (lookupRecursively(newKey, firstCh, lastCh, std::max(leftKey.length(), rightKey.length()) + 3)) {
                        return true;
                    }
                }
            } while (iter->hasNext() && nextChar <= rightKey[rightKey.length() - 1]);
        }

        if (leftKey.length() == common_prefix.length() && leftKey.length() + 1 == rightKey.length()) {
            auto hashedPrefix = hashString(common_prefix);
            if (lookupRangeBetweenLetters(hashedPrefix, firstCh, rightKey[rightKey.length() - 1])) return true;
            iter = new CharIterator(char_usage_mask_);
            char nextChar;
            do {
                nextChar = iter->next();
                std::string newKey = common_prefix + nextChar;
                if (bf_.lookupKey(newKey)) {
                    if (lookupRecursively(newKey, firstCh, lastCh, std::max(leftKey.length(), rightKey.length()) + 3)) {
                        return true;
                    }
                }
            } while (iter->hasNext() && nextChar <= rightKey[rightKey.length() - 1]);
        }

        auto hashedPrefix = hashString(common_prefix);
        if (lookupRangeBetweenLetters(hashedPrefix,
                                      leftKey.length() > common_prefix.length() ? leftKey[common_prefix.length()] : firstCh,
                                      rightKey[common_prefix.length()])) {
            return true;
        }

        // Go down the left branch
        for (size_t i = common_prefix.length(); i < leftKey.length(); i++) {
            auto prefix = leftKey.substr(0, i);
            auto hashedPrefix = hashString(prefix);
            if (lookupRangeBetweenLetters(hashedPrefix, leftKey[i], lastCh)) return true;
            if (!bf_.lookupKey(prefix)) break;
            iter = new CharIterator(char_usage_mask_, leftKey[i] + 1);
            char nextChar;
            do {
                nextChar = iter->next();
                std::string newKey = prefix + nextChar;
                if (bf_.lookupKey(newKey)) {
                    if (lookupRecursively(newKey, firstCh, lastCh, std::max(leftKey.length(), rightKey.length()) + 3)) {
                        return true;
                    }
                }
            } while (iter->hasNext());
        }

        iter = new CharIterator(char_usage_mask_, (leftKey.length() > common_prefix.length() ? leftKey[common_prefix.length()] : firstCh) + 1);
        char nextChar = iter->next();
        while (nextChar < rightKey[common_prefix.length()]) {
            std::string newKey = common_prefix + nextChar;
            if (bf_.lookupKey(newKey)) {
                if (lookupRecursively(newKey, firstCh, lastCh, std::max(leftKey.length(), rightKey.length())) + 3) {
                    return true;
                }
            }
            nextChar = iter->next();
        }

        // Go down the right branch
        for (size_t i = common_prefix.length(); i < rightKey.length(); i++) {
            auto prefix = rightKey.substr(0, i);
            hashedPrefix = hashString(prefix);
            if (lookupRangeBetweenLetters(hashedPrefix, firstCh, rightKey[i])) return true;
            if (!bf_.lookupKey(prefix)) break;
            iter = new CharIterator(char_usage_mask_);
            char nextChar;
            do {
                nextChar = iter->next();
                std::string newKey = prefix + nextChar;
                if (bf_.lookupKey(newKey)) {
                    if (lookupRecursively(newKey, firstCh, lastCh, std::max(leftKey.length(), rightKey.length()) + 3)) {
                        return true;
                    }
                }
            } while (iter->hasNext() and nextChar < rightKey[i]);
        }


        return false;
    }

    bool BloomedCHaREQ::lookupRecursively(std::string& prefix, char left, char right, uint32_t max_level) {
        if (prefix.length() > max_level) return true;
        auto hashedPrefix = hashString(prefix);
        if (lookupRangeBetweenLetters(hashedPrefix, left, right)) return true;
        auto iter = new CharIterator(char_usage_mask_, left);
        char current = iter->next();
        while (current <= right) {
            std::string new_prefix = prefix + current;
            if (lookupRecursively(new_prefix, left, right, max_level + 1)) return true;
            if (!iter->hasNext()) break;
            current = iter->next();
        }
        return false;
    }

    bool BloomedCHaREQ::lookupLetter(uint64_t idx, char letter) {
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

    bool BloomedCHaREQ::lookupRangeBetweenLetters(uint64_t idx, char letterLeft, char letterRight) {
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

    uint64_t BloomedCHaREQ::getMemoryUsage() const {
        uint64_t bits_count = 0;
        bits_count += 512; // Letters maps
        bits_count += 32; // r_
        bits_count += 3 * slots_cnt_; // 3 bitmaps
        bits_count += r_ * values_table_slots_cnt_; // values table
        bits_count += bf_.getSize();
        return bits_count / 8;
    }
}
#endif //RANGE_FILTERING_BLOOMEDCHAREQ_HPP
