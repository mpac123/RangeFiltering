#ifndef RANGE_FILTERING_LIL_ROSETTA_HPP
#define RANGE_FILTERING_LIL_ROSETTA_HPP

#include "RangeFilter.h"
#include <vector>
#include "bf.hpp"

namespace range_filtering_rosetta {
    class LilRosetta : public range_filtering::RangeFilter {
    public:
         LilRosetta(const std::vector<std::string>& keys, uint32_t total_bits, float penalty_top_layers);
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

        void generatePrefixesOfLength(uint32_t length, const std::vector<boost::multiprecision::uint256_t> &keys,
                                      std::vector<std::vector<boost::multiprecision::uint256_t>>& prefixes);
        bool lookupRange(boost::multiprecision::uint256_t from, boost::multiprecision::uint256_t to);
        bool lookupDyadicRange(boost::multiprecision::uint256_t query, unsigned level, unsigned cnt);
    };

    LilRosetta::LilRosetta(const std::vector<std::string> &keys, uint32_t total_bits, float penalty_top_layers) {
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

        std::vector<std::vector<boost::multiprecision::uint256_t>> prefixes;
        for (size_t length = 1; length <= maxLevel_; length++) {
            generatePrefixesOfLength(length, encoded_keys, prefixes);
        }

        uint64_t prefixes_cnt = 0;
        for (const auto& prefixVect : prefixes) {
            prefixes_cnt += prefixVect.size();
        }

        for (size_t i = 0; i < prefixes.size() - 1; i++) {
            bloomFilters_.push_back(BF(prefixes[i], (double) total_bits * (prefixes[i].size() * (1. - penalty_top_layers) / (prefixes_cnt + 0.)) + 1));
        }
        bloomFilters_.push_back(BF(prefixes[prefixes.size() - 1], (double) total_bits * (prefixes[prefixes.size() - 1].size() / (prefixes_cnt + 0.)) + 1));
    }

    void LilRosetta::generatePrefixesOfLength(uint32_t length, const std::vector<boost::multiprecision::uint256_t> &keys,
                                           std::vector<std::vector<boost::multiprecision::uint256_t>>& prefixes) {
        auto trimmed_prefixes = std::set<boost::multiprecision::uint256_t>();
        for (const auto& key : keys) {
            auto mask = ~((boost::multiprecision::uint256_t(1) << (256 - length)) - 1);
            trimmed_prefixes.insert(key & mask);
        }
        std::vector<boost::multiprecision::uint256_t> vect(trimmed_prefixes.begin(), trimmed_prefixes.end());
        prefixes.push_back(vect);
    }

    uint8_t LilRosetta::calculateCode(char ch, std::bitset<256> zeroAfterOneMask) {
        auto shiftedCharUsage = charUsageMask_ << (size_t) (255 - int(ch));
        auto shiftedZeroAfterOneMask = zeroAfterOneMask << (size_t) (255 - int(ch));
        return shiftedCharUsage.count() + shiftedZeroAfterOneMask.count();
    }

    boost::multiprecision::uint256_t LilRosetta::parseStringToUint256(const std::string &key, std::bitset<256> zeroAfterOneMask) {
        assert(key.length() <= MAX_LENGTH);

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

        std::string trimmed_from = left_key;
        std::string trimmed_to = right_key;
        if (left_key.length() > MAX_LENGTH) {
            trimmed_from = left_key.substr(0, MAX_LENGTH);
        }
        if (right_key.length() > MAX_LENGTH) {
            trimmed_to = right_key.substr(0, MAX_LENGTH);
        }

        auto from_int = parseStringToUint256(trimmed_from, zeroAfterOneMask);
        auto to_int = parseStringToUint256(trimmed_to, zeroAfterOneMask);

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

        auto res = bloomFilters_[level].lookupKey(query);
        if (!res) return false;

        if (level == maxLevel_ - 1) return true;

        // Otherwise, start the process of doubting
        auto right_query = query | (boost::multiprecision::uint256_t(1) << (254 - level));
        return lookupDyadicRange(query, level + 1, cnt + 1)
               || lookupDyadicRange(right_query, level + 1, cnt + 1);
    }

    uint64_t LilRosetta::getMemoryUsage() const {
        uint64_t size = 32;
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
