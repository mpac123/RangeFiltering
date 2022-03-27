#ifndef RANGE_FILTERING_BF_HPP
#define RANGE_FILTERING_BF_HPP

#include <iostream>
#include <boost/multiprecision/cpp_int.hpp>
#include "MurmurHash3.h"

namespace range_filtering_rosetta {
    class BF {
    public:
        BF(std::vector<boost::multiprecision::uint256_t>& keys, uint32_t m, uint16_t k);
        bool lookupKey(const boost::multiprecision::uint256_t& key, uint16_t k);
        uint64_t getMemoryUsage();

        uint16_t static calculateNumberOfHashes(uint32_t n, uint32_t m);

    private:
        std::vector<bool> bitArray_;
    };

    BF::BF(std::vector<boost::multiprecision::uint256_t> &keys, uint32_t m, uint16_t k) {
        bitArray_ = std::vector<bool>(m);

        for (const auto& key : keys) {
            for (size_t i = 0; i < k; i++) {
                uint32_t result;
                MurmurHash3_x86_32(&key, 32, i, (void*) &result);
                bitArray_[result % bitArray_.size()] = true;
            }
        }
    }

    uint16_t BF::calculateNumberOfHashes(uint32_t n, uint32_t m) {
        auto k = (int16_t) std::floor(std::log(2) * m / (n + 0.0) + 0.5);
        if (k == 0) return 1;
        if (k > 16) return 16;
        return k;
    }

    bool BF::lookupKey(const boost::multiprecision::uint256_t &key, uint16_t k) {
        for (size_t i = 0 ; i < k ; i++) {
            uint32_t result;
            MurmurHash3_x86_32(&key, 32, i, (void*) &result);
            if (!bitArray_[result % bitArray_.size()]) {
                return false;
            }
        }
        return true;
    }

    uint64_t BF::getMemoryUsage() {
        return std::ceil(bitArray_.size() / 8.0);
    }
}

#endif //RANGE_FILTERING_BF_HPP
