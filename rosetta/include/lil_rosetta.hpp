#ifndef RANGE_FILTERING_LIL_ROSETTA_HPP
#define RANGE_FILTERING_LIL_ROSETTA_HPP

#include "RangeFilter.h"
#include <vector>
#include "bf.hpp"

namespace range_filtering_rosetta {
    class LilRosetta : public range_filtering::RangeFilter {
    public:
         LilRosetta(const std::vector<std::string>& keys, uint32_t total_bits);
         bool lookupRange(const std::string& left_key, const std::string& right_key) override;
         uint64_t getMemoryUsage() const override;

         boost::multiprecision::uint256_t parseStringToUint256(const std::string& key, std::bitset<256> zeroAfterOneMask);
         uint8_t calculateCode(char ch, std::bitset<256> zeroAfterOneMask);
        static unsigned countTrailingZeroes(boost::multiprecision::uint256_t n);
        std::bitset<256> calculateOneToZeroMask() { return (charUsageMask_ xor (charUsageMask_ << 1)) & ~charUsageMask_; }


        static const uint32_t MAX_LENGTH = 32;
        static const uint32_t MAX_DOUBTING_DEPTH = 64;

    private:
        std::vector<BF> bloomFilters_;
        std::bitset<256> charUsageMask_;
        bool failed_;
        uint32_t maxLevel_;
        uint16_t k_;
        uint8_t bitsPerChar_;

        void insertPrefixesOfLength(uint32_t length, const std::vector<boost::multiprecision::uint256_t>& prefixes, uint32_t single_bf_size);
        bool lookupRange(boost::multiprecision::uint256_t from, boost::multiprecision::uint256_t to);
        bool lookupDyadicRange(boost::multiprecision::uint256_t query, unsigned level, unsigned cnt);
    };

    LilRosetta::LilRosetta(const std::vector<std::string> &keys, uint32_t total_bits) {
        uint64_t max_length = 0;
        for (const auto& key : keys) {
            max_length = std::max(max_length, key.length());
            for (auto ch : key) {
                charUsageMask_[int(ch)] = true;
            }
        }

        if (max_length > MAX_LENGTH) {
            failed_ = true;
            return;
        }

        auto last_bit = charUsageMask_[0];
        uint64_t codes_cnt = 0;
        for (size_t i = 0; i < charUsageMask_.size(); i++) {
            if (charUsageMask_[i]) codes_cnt++;
            else codes_cnt += last_bit;
            last_bit = charUsageMask_[i];
        }

        bitsPerChar_ = std::ceil(std::log2(codes_cnt));

        maxLevel_ = max_length * bitsPerChar_;

        bloomFilters_ = std::vector<BF>();
        uint64_t bf_size = total_bits / maxLevel_;
        k_ = BF::calculateNumberOfHashes(keys.size(), bf_size);

        auto zeroAfterOneMask = calculateOneToZeroMask();
        auto encoded_keys = std::vector<boost::multiprecision::uint256_t>();

        for (const auto& key : keys) {
            encoded_keys.emplace_back(parseStringToUint256(key, zeroAfterOneMask));
        }

        for (size_t length = 1; length <= maxLevel_; length++) {
            insertPrefixesOfLength(length, encoded_keys, bf_size);
        }
    }

    void LilRosetta::insertPrefixesOfLength(uint32_t length, const std::vector<boost::multiprecision::uint256_t> &prefixes,
                                            uint32_t single_bf_size) {
        auto trimmed_prefixes = std::vector<boost::multiprecision::uint256_t>();
        for (const auto& key : prefixes) {
            auto mask = ~((boost::multiprecision::uint256_t(1) << (256 - length)) - 1);
            trimmed_prefixes.emplace_back(key & mask);
        }
        bloomFilters_.emplace_back(BF(trimmed_prefixes, single_bf_size, k_));
    }

    uint8_t LilRosetta::calculateCode(char ch, std::bitset<256> zeroAfterOneMask) {
        auto shiftedCharUsage = charUsageMask_ << (size_t) (255 - int(ch));
        auto shiftedZeroAfterOneMask = zeroAfterOneMask << (size_t) (255 - int(ch));
        return shiftedCharUsage.count() + shiftedZeroAfterOneMask.count();
    }

    boost::multiprecision::uint256_t LilRosetta::parseStringToUint256(const std::string &key, std::bitset<256> zeroAfterOneMask) {
        assert(key.length() < MAX_LENGTH);

        auto parsed = boost::multiprecision::uint256_t(0);
        for (size_t i = 0; i < key.length(); i++) {
            if (i > maxLevel_ / bitsPerChar_) break;
            parsed += boost::multiprecision::uint256_t(calculateCode(key[i], zeroAfterOneMask)) << (maxLevel_ - (i + 1) * bitsPerChar_);
        }
        return parsed << MAX_LENGTH * 8 - maxLevel_;
    }

    bool LilRosetta::lookupRange(const std::string &left_key, const std::string &right_key) {
        assert(!failed_);

        auto zeroAfterOneMask = calculateOneToZeroMask();


        auto from_int = parseStringToUint256(left_key, zeroAfterOneMask);
        auto to_int = parseStringToUint256(right_key, zeroAfterOneMask);

        return lookupRange(from_int, to_int);
    }

    bool LilRosetta::lookupRange(boost::multiprecision::uint256_t from, boost::multiprecision::uint256_t to) {
        if (from > to) {
            return false;
        }

        auto zeros_cnt = countTrailingZeroes(from);
        unsigned skip_zeros_cnt = 0;
        auto interval_end = from;

        for (size_t i = zeros_cnt; i > 0; i--) {
            auto mask = (boost::multiprecision::uint256_t(1) << i) - 1;
            if ((interval_end | mask) > to) {
                skip_zeros_cnt++;
            } else {
                break;
            }
        }

        interval_end |= ((boost::multiprecision::uint256_t(1) << (zeros_cnt - skip_zeros_cnt)) - 1);

        auto res = lookupDyadicRange(from, std::min(255 - zeros_cnt + skip_zeros_cnt, maxLevel_ - 1), 0);
        if (res) return true;

        return lookupRange(interval_end + 1, to);
    }

    bool LilRosetta::lookupDyadicRange(boost::multiprecision::uint256_t query, unsigned level, unsigned cnt) {
        //if (cnt > MAX_DOUBTING_DEPTH) return true;

        auto res = bloomFilters_[level].lookupKey(query, k_);
        if (!res) return false;

        if (level == maxLevel_ - 1) return true;

        // Otherwise, start the process of doubting
        auto right_query = query | (boost::multiprecision::uint256_t(1) << (254 - level));
        return lookupDyadicRange(query, level + 1, cnt + 1)
               || lookupDyadicRange(right_query, level + 1, cnt + 1);
    }

    uint64_t LilRosetta::getMemoryUsage() const {
        uint64_t size = sizeof(uint16_t) + 32;
        for (auto bf : bloomFilters_) {
            size += bf.getMemoryUsage();
        }
        return size;
    }

    unsigned LilRosetta::countTrailingZeroes(boost::multiprecision::uint256_t n) {
        boost::multiprecision::uint256_t x = n, bits = 0;

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

#endif //RANGE_FILTERING_LIL_ROSETTA_HPP
