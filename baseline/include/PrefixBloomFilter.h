#ifndef RANGE_FILTERING_PREFIXBLOOMFILTER_H
#define RANGE_FILTERING_PREFIXBLOOMFILTER_H

#include <BloomFilter.h>
#include <PrefixFilter.h>
#include <unordered_set>

namespace range_filtering {

class PrefixBloomFilter : public PrefixFilter {

public:
    explicit PrefixBloomFilter(std::vector<std::string> &keys, uint32_t bloom_filter_size, uint64_t max_doubting_level = 0);
    bool lookupPrefix(const std::string& prefix) override;
    uint64_t getMemoryUsage() const override;

    uint64_t getBFSize() { return bloomFilter_->getSize(); }
    double getFPR() { return bloomFilter_->getFPR(); }
    uint16_t getNumberOfHashes() { return bloomFilter_->getNumberOfHashes(); }

private:
    bloom_filter::BloomFilter *bloomFilter_;
    uint64_t maxDoubtingLevel_;
    std::vector<std::string> generateAllPrefixes(std::vector<std::string> &keys);
};
}

#endif //RANGE_FILTERING_PREFIXBLOOMFILTER_H
