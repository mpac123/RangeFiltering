#include "BloomFilter.h"

namespace bloom_filter {
    BloomFilter::BloomFilter(std::vector<std::string> &keys, uint32_t m) {
        k_ = calculateNumberOfHashes(keys.size(), m);
        fpr_ = calculateFPR(keys.size(), m);

        bitArray_ = std::vector<bool>(m);

        for (const auto& key : keys) {
            for (size_t i = 0 ; i < k_ ; i++) {
                uint32_t result;
                MurmurHash3_x86_32(key.c_str(), key.size(), i, (void*) &result);
                bitArray_[result % bitArray_.size()] = true;
            }
        }
    }

    uint16_t BloomFilter::calculateNumberOfHashes(uint32_t n, uint32_t m) {
        auto k = (int16_t) std::floor(std::log(2) * m / (n + 0.0) + 0.5);
        if (k == 0) return 1;
        return k;
    }

    bool BloomFilter::lookupKey(const std::string& key) {
        for (size_t i = 0 ; i < k_ ; i++) {
            uint32_t result;
            MurmurHash3_x86_32(key.c_str(), key.size(), i, (void*) &result);
            if (!bitArray_[result % bitArray_.size()]) {
                return false;
            }
        }
        return true;
    }

    double BloomFilter::calculateFPR(uint32_t n, uint32_t size) const {
        auto a = - (double) k_ * (double) n / (size + 0.0);
        auto e = std::exp(a);
        return std::pow(1. - e, k_);
    }

    uint64_t BloomFilter::getMemoryUsage() {
        return sizeof(uint16_t) + std::ceil(bitArray_.size() / 8.0);
    }
}