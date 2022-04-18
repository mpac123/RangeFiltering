#ifndef RANGE_FILTERING_INTEGER_ROSETTA_HPP
#define RANGE_FILTERING_INTEGER_ROSETTA_HPP

#include <vector>
#include <iostream>
#include <set>

#include "integer_bf.hpp"
#include "../../integer_range_filters/include/IntegerRangeFilter.hpp"

namespace range_filtering_rosetta {
    class IntegerRosetta : public range_filtering::IntegerRangeFilter {
    public:
        IntegerRosetta(std::vector<uint32_t> &keys, uint32_t total_size);
        bool lookupRange(uint32_t leftKey, uint32_t rightKey) override;
        uint64_t getMemoryUsage() const override;

        static unsigned countTrailingZeroes(uint32_t n);

    private:
        std::vector<IntegerBF> bloomFilters_;
        uint32_t maxLevel_;

        bool lookupDyadicRange(uint32_t query, unsigned level, unsigned cnt);
        bool lookupRangeShifted(uint32_t leftKey, uint32_t rightKey);
    };

    IntegerRosetta::IntegerRosetta(std::vector<uint32_t> &keys, uint32_t total_size) {
        maxLevel_ = std::floor(std::log2(keys[keys.size() - 1])) + 1;
        auto allPrefixes = std::vector<std::vector<uint32_t>>();
        uint64_t prefixesCnt = 0;
        bloomFilters_ = std::vector<IntegerBF>();

        for (size_t length = 1; length <= maxLevel_; length++) {
            std::set<uint32_t> prefixes;
            auto mask = ~((uint32_t(1) << (32 - length)) - 1);
            for (const auto& key : keys) {
                prefixes.insert((key << (32 - maxLevel_)) & mask);
            }
            std::vector<uint32_t> vect(prefixes.begin(), prefixes.end());
            allPrefixes.push_back(vect);
            prefixesCnt += vect.size();
        }

        for (auto prefixGroup : allPrefixes) {
            bloomFilters_.push_back(IntegerBF(prefixGroup, (double) total_size * (prefixGroup.size() / (prefixesCnt + 0.)) + 1));
        }
    }

    bool IntegerRosetta::lookupRange(uint32_t leftKey, uint32_t rightKey) {
        if (leftKey > rightKey) {
            return false;
        }

        leftKey <<= (32 - maxLevel_);
        rightKey <<= (32 - maxLevel_);

        lookupRangeShifted(leftKey, rightKey);
    }

    bool IntegerRosetta::lookupRangeShifted(uint32_t leftKey, uint32_t rightKey) {
        if (leftKey > rightKey) {
            return false;
        }

        auto zeros_cnt = countTrailingZeroes(leftKey);
        unsigned skip_zeros_cnt = 0;
        auto interval_end = leftKey;

        for (size_t i = zeros_cnt; i > 0; i--) {
            auto mask = (uint32_t (1) << i) - 1;
            if ((interval_end | mask) > rightKey) {
                skip_zeros_cnt++;
            } else {
                break;
            }
        }

        interval_end |= ((uint32_t (1) << (zeros_cnt - skip_zeros_cnt)) - 1);

        auto res = lookupDyadicRange(leftKey, std::min(31 - zeros_cnt + skip_zeros_cnt, maxLevel_ - 1), 0);
        if (res) return true;

        return lookupRangeShifted(interval_end + 1, rightKey);
    }

    bool IntegerRosetta::lookupDyadicRange(uint32_t query, unsigned int level, unsigned int cnt) {
        auto res = bloomFilters_[level].lookupKey(query);
        if (!res) return false;

        if (level == maxLevel_ - 1) return true;

        // Otherwise, start the process of doubting
        auto right_query = query | (uint32_t (1) << (30 - level));
        return lookupDyadicRange(query, level + 1, cnt + 1)
               || lookupDyadicRange(right_query, level + 1, cnt + 1);
    }

    uint64_t IntegerRosetta::getMemoryUsage() const {
        uint64_t size = 0;
        for (auto bf : bloomFilters_) {
            size += bf.getMemoryUsage();
        }
        return size;
    }

    unsigned IntegerRosetta::countTrailingZeroes(uint32_t n) {
        uint32_t x = n, bits = 0;

        if (x) {
            /* assuming `x` has 32 bits: lets count the low order 0 bits in batches */
            /* mask the 16 low order bits, add 16 and shift them out if they are all 0 */
            while (!(x & 0x0000FFFF)) { bits += 16; x >>= 16; }
            /* mask the 8 low order bits, add 8 and shift them out if they are all 0 */
            if (!(x & 0x000000FF)) { bits +=  8; x >>=  8; }
            /* mask the 4 low order bits, add 4 and shift them out if they are all 0 */
            if (!(x & 0x0000000F)) { bits +=  4; x >>=  4; }
            /* mask the 2 low order bits, add 2 and shift them out if they are all 0 */
            if (!(x & 0x00000003)) { bits +=  2; x >>=  2; }
            /* mask the low order bit and add 1 if it is 0 */
            bits += (x & 1) ^ 1;
        }
        return (unsigned) bits;
    }
}

#endif //RANGE_FILTERING_INTEGER_ROSETTA_HPP
