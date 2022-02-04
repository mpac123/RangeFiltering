#include <PrefixBloomFilter.h>

range_filtering::PrefixBloomFilter::PrefixBloomFilter(std::vector<std::string> &keys,
                                                      uint32_t bloom_filter_size) : PrefixFilter() {
    auto all_prefixes = generateAllPrefixes(keys);
    bloomFilter_ = new bloom_filter::BloomFilter(all_prefixes, bloom_filter_size);
}

std::vector<std::string> range_filtering::PrefixBloomFilter::generateAllPrefixes(std::vector<std::string> &keys) {
    std::vector<std::string> all_prefixes = std::vector<std::string>();
    for (const auto& key : keys) {
        for (size_t i = 1; i <= key.length(); i++) {
            all_prefixes.push_back(key.substr(0, i));
        }
    }
    return all_prefixes;
}

bool range_filtering::PrefixBloomFilter::lookupPrefix(const std::string& prefix) {
    if (prefix.empty()) {
        return true;
    }
    return bloomFilter_->lookupKey(prefix);
}

uint64_t range_filtering::PrefixBloomFilter::getMemoryUsage() const {
    return bloomFilter_->getMemoryUsage();
}