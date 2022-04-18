#ifndef RANGE_FILTERING_INTEGER_BF_HPP
#define RANGE_FILTERING_INTEGER_BF_HPP

#include <iostream>
#include <vector>
#include "MurmurHash3.h"
#include "cmath"

namespace range_filtering_rosetta {
    class IntegerBF {
    public:
        IntegerBF(std::vector<uint32_t>& keys, uint32_t m);
        bool lookupKey(uint32_t key);
        uint64_t getMemoryUsage();
        uint16_t static calculateNumberOfHashes(uint32_t n, uint32_t m);

    private:
        std::vector<bool> bitArray_;
        uint16_t k_;
    };

    IntegerBF::IntegerBF(std::vector<uint32_t> &keys, uint32_t m) {
        bitArray_ = std::vector<bool>(m);
        k_ = calculateNumberOfHashes(keys.size(), m);

        for (const auto& key : keys) {
            for (size_t i = 0; i < k_; i++) {
                uint32_t result;
                MurmurHash3_x86_32(&key, 4, i, (void*) &result);
                bitArray_[result % bitArray_.size()] = true;
            }
        }
    }

    uint16_t IntegerBF::calculateNumberOfHashes(uint32_t n, uint32_t m) {
        auto k = (int64_t) std::floor(std::log(2) * m / (n + 0.0) + 0.5);
        if (k == 0) return 1;
        if (k > 65535) return 65535;
        return k;
    }

    bool IntegerBF::lookupKey(uint32_t key) {
        for (size_t i = 0 ; i < k_ ; i++) {
            uint32_t result;
            MurmurHash3_x86_32(&key, 4, i, (void*) &result);
            if (!bitArray_[result % bitArray_.size()]) {
                return false;
            }
        }
        return true;
    }

    uint64_t IntegerBF::getMemoryUsage() {
        return sizeof(uint16_t) + std::ceil(bitArray_.size() / 8.0);
    }
}

#endif //RANGE_FILTERING_INTEGER_BF_HPP
