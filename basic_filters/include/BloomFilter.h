#ifndef BLOOMFILTER_BLOOMFILTER_H
#define BLOOMFILTER_BLOOMFILTER_H

#include <vector>
#include <string>
#include <cmath>
#include "MurmurHash3.h"

namespace bloom_filter {

class BloomFilter {
public:
    BloomFilter(std::vector<std::string> &keys, uint32_t m);
    bool lookupKey(const std::string& key);
    uint64_t getMemoryUsage();
    double getFPR() const { return fpr_; }
    uint16_t getNumberOfHashes() const { return k_; }
    uint64_t getSize() const { return bitArray_.size(); }

private:
    std::vector<bool> bitArray_;
    uint16_t k_;
    double fpr_;

    uint16_t calculateNumberOfHashes(uint32_t n, uint32_t m);
    double calculateFPR(uint32_t n, uint32_t m) const;

};
} // namespace bloom_filter

#endif //BLOOMFILTER_BLOOMFILTER_H
