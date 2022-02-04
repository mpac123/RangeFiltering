#ifndef BLOOM_FILTER_QUOTIENTFILTER_H
#define BLOOM_FILTER_QUOTIENTFILTER_H

#include <iostream>
#include <vector>
#include <MurmurHash3.h>
#include <cmath>

namespace quotient_filter {
class QuotientFilter {
public:
    QuotientFilter(std::vector<std::string> &keys, uint32_t q, uint32_t r);
    bool lookupKey(const std::string& key);
    uint64_t getMemoryUsage();
    double calculateFalsePositiveProbability();
    bool hasFailed() { return failed_; };

private:
    std::vector<uint32_t> remainder_table_;
    std::vector<bool> is_occupied_;
    std::vector<bool> is_continuation_;
    std::vector<bool> is_shifted_;

    uint32_t p_;
    uint32_t q_;
    uint64_t n_;
    bool failed_;

    uint64_t arr_size_;

    void insertKey(const std::string& key);
};
}

#endif //BLOOM_FILTER_QUOTIENTFILTER_H
