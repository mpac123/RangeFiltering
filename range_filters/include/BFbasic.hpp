#ifndef RANGE_FILTERING_BFBASIC_HPP
#define RANGE_FILTERING_BFBASIC_HPP

#include <iostream>
#include <vector>
#include <cmath>

namespace range_filtering {
    class BFbasic {
    public:
        BFbasic(uint32_t n, uint32_t m);
        bool lookupKey(std::vector<uint64_t> hashes);
        uint64_t getMemoryUsage();
        void insert(uint64_t hash);
        uint16_t getK() { return k_; }
    private:
        std::vector<bool> bitArray_;
        uint16_t k_;
        uint16_t calculateNumberOfHashes(uint32_t n, uint32_t m);
    };

    BFbasic::BFbasic(uint32_t n, uint32_t m) {
        k_ = calculateNumberOfHashes(n, m);
        bitArray_ = std::vector<bool>(m);
    }

    void BFbasic::insert(uint64_t hash) {
        bitArray_[hash % bitArray_.size()] = true;
    }

    bool BFbasic::lookupKey(std::vector<uint64_t> hashes) {
        for (size_t i = 0; i < k_; i++) {
            if (!bitArray_[hashes[i] % bitArray_.size()]) return false;
        }
        return true;
    }

    uint16_t BFbasic::calculateNumberOfHashes(uint32_t n, uint32_t m) {
        auto k = (int16_t) std::floor(std::log(2) * m / (n + 0.0) + 0.5);
        if (k == 0) return 1;
        if (k > 16) return 16;
        return k;
    }

    uint64_t BFbasic::getMemoryUsage() {
        return std::ceil(bitArray_.size() / 8.0) + sizeof(uint16_t);
    }
}

#endif //RANGE_FILTERING_BFBASIC_HPP
