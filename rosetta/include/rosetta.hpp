#ifndef RANGE_FILTERING_ROSETTA_HPP
#define RANGE_FILTERING_ROSETTA_HPP

#include "RangeFilter.h"
#include "bf.hpp"
#include <vector>

namespace range_filtering_rosetta {
class Rosetta : public range_filtering::RangeFilter {
public:
    Rosetta(const std::vector<std::string>& keys, uint32_t total_size);
    bool lookupRange(const std::string& from, const std::string& to) override;
    uint64_t getMemoryUsage() const override;
    static boost::multiprecision::uint256_t parseStringToUint256(const std::string& key);
    static unsigned countTrailingZeroes(boost::multiprecision::uint256_t n);

    static const uint32_t MAX_LENGTH = 32;
    static const uint32_t MAX_DOUBTING_DEPTH = 64;
private:
    std::vector<BF> bloomFilters_;
    bool failed_;
    uint32_t maxLevel_;

    void generatePrefixesOfLength(uint32_t length, const std::vector<std::string> &keys,
                                           std::vector<std::vector<boost::multiprecision::uint256_t>>& prefixes);
    bool lookupRange(boost::multiprecision::uint256_t from, boost::multiprecision::uint256_t to);
    bool lookupDyadicRange(boost::multiprecision::uint256_t query, unsigned level, unsigned cnt);
};

Rosetta::Rosetta(const std::vector<std::string>& keys, uint32_t total_size) {
    uint64_t max_length = 0;
    for (const auto& key : keys) {
        max_length = std::max(max_length, key.length());
    }
    if (max_length > MAX_LENGTH) {
        failed_ = true;
        return;
    }
    maxLevel_ = max_length * 8;


    bloomFilters_ = std::vector<BF>();

    std::vector<std::vector<boost::multiprecision::uint256_t>> prefixes;
    for (size_t length = 1; length <= maxLevel_; length++) {
        generatePrefixesOfLength(length, keys, prefixes);
    }

    uint64_t prefixes_cnt = 0;
    for (const auto& prefixVect : prefixes) {
        prefixes_cnt += prefixVect.size();
    }

    for (auto prefixVect : prefixes) {
        bloomFilters_.push_back(BF(prefixVect, (double) total_size * (prefixVect.size() / (prefixes_cnt + 0.)) + 1));
    }
}

void Rosetta::generatePrefixesOfLength(uint32_t length, const std::vector<std::string> &keys,
                                        std::vector<std::vector<boost::multiprecision::uint256_t>>& prefixes) {
    auto trimmed_prefixes = std::set<boost::multiprecision::uint256_t>();
    for (const auto& key : keys) {
        auto parsedKey = parseStringToUint256(key);
        auto mask = ~((boost::multiprecision::uint256_t(1) << (256 - length)) - 1);
        trimmed_prefixes.insert(parsedKey & mask);
    }
    std::vector<boost::multiprecision::uint256_t> vect(trimmed_prefixes.begin(), trimmed_prefixes.end());
    prefixes.push_back(vect);
}

boost::multiprecision::uint256_t Rosetta::parseStringToUint256(const std::string &key) {
    assert(key.length() < MAX_LENGTH);

    auto parsed = boost::multiprecision::uint256_t(0);
    for (size_t i = 0; i < key.length(); i++) {
        parsed += boost::multiprecision::uint256_t(key[i]) << ((MAX_LENGTH - i - 1) * 8);
    }
    return parsed;
}

bool Rosetta::lookupRange(const std::string &from, const std::string &to) {
    assert(!failed_);

    auto from_int = parseStringToUint256(from);
    auto to_int = parseStringToUint256(to);

    return lookupRange(from_int, to_int);
}

bool Rosetta::lookupRange(boost::multiprecision::uint256_t from, boost::multiprecision::uint256_t to) {
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

bool Rosetta::lookupDyadicRange(boost::multiprecision::uint256_t query, unsigned level, unsigned cnt) {
    // if (cnt > MAX_DOUBTING_DEPTH) return true;

    auto res = bloomFilters_[level].lookupKey(query);
    if (!res) return false;

    if (level == maxLevel_ - 1) return true;

    // Otherwise, start the process of doubting
    auto right_query = query | (boost::multiprecision::uint256_t(1) << (254 - level));
    return lookupDyadicRange(query, level + 1, cnt + 1)
        || lookupDyadicRange(right_query, level + 1, cnt + 1);
}

uint64_t Rosetta::getMemoryUsage() const {
    uint64_t size = 0;
    for (auto bf : bloomFilters_) {
        size += bf.getMemoryUsage();
    }
    return size;
}

unsigned Rosetta::countTrailingZeroes(boost::multiprecision::uint256_t n) {
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
} // namespace range_filtering_rosetta

#endif //RANGE_FILTERING_ROSETTA_HPP
