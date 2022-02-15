#include <PrefixBloomFilter.h>

range_filtering::PrefixBloomFilter::PrefixBloomFilter(std::vector<std::string> &keys,
                                                      uint32_t bloom_filter_size,
                                                      uint64_t max_doubting_level) : PrefixFilter() {
    auto all_prefixes = generateAllPrefixes(keys);
    bloomFilter_ = new bloom_filter::BloomFilter(all_prefixes, bloom_filter_size);
    maxDoubtingLevel_ = max_doubting_level;
}

std::vector<std::string> range_filtering::PrefixBloomFilter::generateAllPrefixes(std::vector<std::string> &keys) {
    std::unordered_set<std::string> all_prefixes = std::unordered_set<std::string>();
    for (const auto& key : keys) {
        for (size_t i = 1; i <= key.length(); i++) {
            all_prefixes.insert(key.substr(0, i));
        }
    }
    auto result = std::vector<std::string>(all_prefixes.begin(), all_prefixes.end());
    return result;
}

bool range_filtering::PrefixBloomFilter::lookupPrefix(const std::string& prefix) {
    if (prefix.empty()) {
        return true;
    }
    auto may_exist = bloomFilter_->lookupKey(prefix);
    if (!may_exist) return false;
    std::string query = prefix.substr(0, prefix.length() - 1);
    for (size_t i = 0; i < maxDoubtingLevel_; i++) {
        if (query.length() == 0) return true;
        may_exist = bloomFilter_->lookupKey(query);
        if (!may_exist) return false;
        query = prefix.substr(0, query.length() - 1);
    }
    return true;
}

uint64_t range_filtering::PrefixBloomFilter::getMemoryUsage() const {
    return bloomFilter_->getMemoryUsage();
}