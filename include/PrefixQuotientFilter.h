#ifndef RANGE_FILTERING_PREFIXQUOTIENTFILTER_H
#define RANGE_FILTERING_PREFIXQUOTIENTFILTER_H

#include <PrefixFilter.h>
#include <unordered_set>
#include <vector>
#include "MurmurHash3.h"
#include <cmath>
#include <cassert>
extern "C" {
#include <qf.h>
};

namespace range_filtering {

class PrefixQuotientFilter : public PrefixFilter {
public:
    explicit PrefixQuotientFilter(std::vector<std::string> &keys, uint32_t r, uint64_t max_doubting_level = 0);

    bool lookupPrefix(const std::string &prefix) override;

    unsigned long long getMemoryUsage() const override;

    std::string getName() const override {
        return "PrefixQF q=" + std::to_string(q_) + " r = " + std::to_string(r_);
    }

    double getFalsePositiveProbability();
    bool hasFailed() const { return failed_; }
    uint32_t getQ() const { return q_; }

private:
    uint32_t q_;
    uint32_t r_;
    uint64_t n_;
    bool failed_;
    uint64_t maxDoubtingLevel_;

    struct quotient_filter quotientFilter_;

    std::vector<std::string> generateAllPrefixes(std::vector<std::string> &keys);
    static uint64_t getFingerprint(const std::string &prefix);
};

} // namespace range_filtering

#endif //RANGE_FILTERING_PREFIXQUOTIENTFILTER_H
