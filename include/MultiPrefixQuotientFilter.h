#ifndef RANGE_FILTERING_MULTIPREFIXQUOTIENTFILTER_H
#define RANGE_FILTERING_MULTIPREFIXQUOTIENTFILTER_H

#include <PrefixFilter.h>
#include <unordered_set>
extern "C" {
#include <qf.h>
};
#include <vector>
#include <cmath>
#include <MurmurHash3.h>

namespace range_filtering {

    class MultiPrefixQuotientFilter : public PrefixFilter {
    public:
        explicit MultiPrefixQuotientFilter(std::vector<std::string> &keys, uint32_t r, uint64_t max_doubting_level = 0);
        bool lookupPrefix(const std::string& prefix) override;
        unsigned long long getMemoryUsage() const override;
        std::string getName() const override { return "MultiPrefixQF"; }

        bool hasFailed() const { return failed_; }
        std::vector<uint32_t> getQs() { return qs_; };

    private:
        uint32_t r_;
        bool failed_;
        uint64_t maxDoubtingLevel_;
        std::vector<struct quotient_filter> quotientFilters_;
        std::vector<uint32_t> qs_;
        std::vector<std::vector<std::string>> generateAllPrefixes(std::vector<std::string> &keys);
        uint64_t getFingerprint(const std::string& prefix);

    };
}

#endif //RANGE_FILTERING_MULTIPREFIXQUOTIENTFILTER_H
