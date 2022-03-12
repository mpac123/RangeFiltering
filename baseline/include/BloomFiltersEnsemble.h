#ifndef RANGE_FILTERING_BLOOMFILTERSENSEMBLE_H
#define RANGE_FILTERING_BLOOMFILTERSENSEMBLE_H

#include <vector>
#include <iostream>
#include <BloomFilter.h>

namespace range_filtering {
    class BloomFiltersEnsemble {
    public:
        BloomFiltersEnsemble() : padding_(0) {};
        explicit BloomFiltersEnsemble(std::vector<std::vector<std::string>> &keys,
                                      uint32_t total_BFs_size,
                                      double maximalLevelPenalty);
        bool lookupKey(const std::string& prefix) const;
        uint64_t getMemoryUsage() const;

    private:
        std::vector<bloom_filter::BloomFilter*> bloomFilters_;
        uint32_t  padding_;
    };
}

#endif //RANGE_FILTERING_BLOOMFILTERSENSEMBLE_H
