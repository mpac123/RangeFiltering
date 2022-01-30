#ifndef RANGE_FILTERING_PREFIXQUOTIENTFILTER_H
#define RANGE_FILTERING_PREFIXQUOTIENTFILTER_H

#include <QuotientFilter.h>
#include <PrefixFilter.h>
#include <unordered_set>

namespace range_filtering {

class PrefixQuotientFilter : public PrefixFilter {
public:
    explicit PrefixQuotientFilter(std::vector<std::string> &keys, uint32_t q, uint32_t r);

    bool lookupPrefix(const std::string &prefix) override;

    uint64_t getMemoryUsage() const override;

    std::string getName() const override {
        return "PrefixQF q=" + std::to_string(q_) + " r = " + std::to_string(r_);
    }

    double getFalsePositiveProbability() { return quotientFilter_->calculateFalsePositiveProbability(); }
    bool hasFailed() { return quotientFilter_->hasFailed(); }

private:
    uint32_t q_;
    uint32_t r_;

    quotient_filter::QuotientFilter *quotientFilter_;

    std::vector<std::string> generateAllPrefixes(std::vector<std::string> &keys);
};

} // namespace range_filtering

#endif //RANGE_FILTERING_PREFIXQUOTIENTFILTER_H
