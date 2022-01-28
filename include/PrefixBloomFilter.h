#ifndef RANGE_FILTERING_PREFIXBLOOMFILTER_H
#define RANGE_FILTERING_PREFIXBLOOMFILTER_H

#include <BloomFilter.h>
#include <PrefixFilter.h>

namespace range_filtering {

class PrefixBloomFilter : public PrefixFilter {

public:
    // Keys must be sorted
    explicit PrefixBloomFilter(std::vector<std::string> &keys, uint32_t bloom_filter_size);
    bool lookupPrefix(const std::string& prefix) override;
    uint64_t getMemoryUsage() const override;
    std::string getName() const override { return "PrefixBF " + std::to_string(bloomFilter_->getSize()); }

    uint64_t getBFSize() { return bloomFilter_->getSize(); }
    double getFPR() { return bloomFilter_->getFPR(); }
    uint8_t getNumberOfHashes() { return bloomFilter_->getNumberOfHashes(); }

private:
    bloom_filter::BloomFilter *bloomFilter_;

    std::vector<std::string> generateAllPrefixes(std::vector<std::string> &keys);
};
}

#endif //RANGE_FILTERING_PREFIXBLOOMFILTER_H
