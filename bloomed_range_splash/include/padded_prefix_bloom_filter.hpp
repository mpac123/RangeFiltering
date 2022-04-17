#ifndef RANGE_FILTERING_PADDED_BLOOM_FILTER_HPP
#define RANGE_FILTERING_PADDED_BLOOM_FILTER_HPP

#include <iostream>
#include <BloomFilter.h>
#include <assert.h>

namespace range_filtering_bloomed_range_splash {
    class PaddedPrefixBloomFilter {
    public:
        PaddedPrefixBloomFilter() {};
        explicit PaddedPrefixBloomFilter(const std::vector<std::string>& keys,
                                         uint32_t total_bits,
                                         uint32_t padding);
        bool lookupKey(const std::string& key) const;
        uint64_t getMemoryUsage() const;
    private:
        bloom_filter::BloomFilter* bloomFilter_;
        uint32_t padding_;
    };

    PaddedPrefixBloomFilter::PaddedPrefixBloomFilter(const std::vector<std::string>& keys,
                                                     uint32_t total_bits,
                                                     uint32_t padding) {
        padding_ = padding;
        std::vector<std::string> prefixes;
        for (const auto& key : keys) {
            for (size_t i = padding_ + 1; i < key.length() + 1; i++) {
                prefixes.push_back(key.substr(0, i));
            }
        }
        bloomFilter_ = new bloom_filter::BloomFilter(prefixes, total_bits);
    }

    bool PaddedPrefixBloomFilter::lookupKey(const std::string &key) const {
        assert(key.length() > padding_);
        return bloomFilter_->lookupKey(key);
    }

    uint64_t PaddedPrefixBloomFilter::getMemoryUsage() const {
        return bloomFilter_->getMemoryUsage() + 4;
    }
}

#endif //RANGE_FILTERING_PADDED_BLOOM_FILTER_HPP
