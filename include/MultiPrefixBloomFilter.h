#ifndef RANGE_FILTERING_MULTIPREFIXBLOOMFILTER_H
#define RANGE_FILTERING_MULTIPREFIXBLOOMFILTER_H

#include <BloomFilter.h>
#include <PrefixFilter.h>
#include <unordered_set>

namespace range_filtering {

class MultiPrefixBloomFilter : public PrefixFilter {

public:
    enum MemoryAllocationType { proportional, equal, proportionalDecreasing, equalDecreasing };

public:
    explicit MultiPrefixBloomFilter(std::vector<std::string> &keys, uint32_t total_BFs_size,
                                    uint64_t max_doubting_level = 0,
                                    MemoryAllocationType memoryAllocationType = MemoryAllocationType::proportional,
                                    double maximalDecreasePercentage = 0.0);
    bool lookupPrefix(const std::string& prefix) override;
    uint64_t getMemoryUsage() const override;
    std::string getName() const override { return "MultiPrefixBF"; }

    std::vector<uint64_t> getBFsSizes();
    std::vector<uint16_t> getNumberOfHashes();


private:
    std::vector<bloom_filter::BloomFilter*> bloomFilters_;
    uint64_t maxDoubtingLevel_;
    uint32_t totalBFsSize_;
    std::vector<std::vector<std::string>> generateAllPrefixes(std::vector<std::string> &keys);
};
}

#endif //RANGE_FILTERING_MULTIPREFIXBLOOMFILTER_H
