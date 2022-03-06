#include <MultiPrefixBloomFilter.h>

range_filtering::MultiPrefixBloomFilter::MultiPrefixBloomFilter(std::vector<std::string> &keys, uint32_t total_BFs_size,
                                                                uint64_t max_doubting_level,
                                                                MemoryAllocationType memoryAllocationType,
                                                                double maximalDecreasePercentage) {
    auto all_prefixes_list = generateAllPrefixes(keys);
    auto all_prefixes_cnt = 0;
    for (const auto& prefixes : all_prefixes_list) {
        all_prefixes_cnt += prefixes.size();
    }
    bloomFilters_ = std::vector<bloom_filter::BloomFilter*>(all_prefixes_list.size());
    for (size_t i = 0; i < all_prefixes_list.size(); i++) {
        uint32_t BF_size;
        double discount = 0.0;
        switch(memoryAllocationType) {
            case proportional:
                BF_size = uint32_t(std::round((all_prefixes_list[i].size() + 0.) / all_prefixes_cnt * total_BFs_size));
                break;
            case equal:
                BF_size = uint32_t(std::round(total_BFs_size / all_prefixes_list.size()));
                break;
            case proportionalDecreasing:
                discount = 1. - i/(all_prefixes_list.size() - 1.) * maximalDecreasePercentage;
                BF_size = uint32_t(std::round((all_prefixes_list[i].size() + 0.) / all_prefixes_cnt * total_BFs_size * discount));
                if (BF_size == 0) BF_size = 1;
                break;
            case equalDecreasing:
                BF_size = uint32_t(std::round((total_BFs_size / all_prefixes_list.size() * (1. - i/all_prefixes_list.size() * maximalDecreasePercentage))));
                break;
        }
        bloomFilters_[i] = new bloom_filter::BloomFilter(all_prefixes_list[i], BF_size);
    }
    maxDoubtingLevel_ = max_doubting_level;
}

std::vector<std::vector<std::string>> range_filtering::MultiPrefixBloomFilter::generateAllPrefixes(
        std::vector<std::string> &keys) {
    auto all_prefixes_sets = std::vector<std::unordered_set<std::string>>();
    for (const auto& key : keys) {
        for (size_t i = 1; i <= key.length(); i++) {
            if (all_prefixes_sets.size() < i) {
                all_prefixes_sets.push_back(std::unordered_set<std::string>());
            }
            all_prefixes_sets[i-1].insert(key.substr(0, i));
        }
    }
    auto all_prefixes_vectors = std::vector<std::vector<std::string>>(all_prefixes_sets.size());
    for (size_t i = 0; i < all_prefixes_sets.size(); i++) {
        all_prefixes_vectors[i] = std::vector<std::string>(all_prefixes_sets[i].begin(), all_prefixes_sets[i].end());
    }
    return all_prefixes_vectors;
}

bool range_filtering::MultiPrefixBloomFilter::lookupPrefix(const std::string &prefix) {
    if (prefix.empty()) {
        return true;
    }
    if (prefix.size() > bloomFilters_.size()) {
        return false;
    }
    auto may_exist = bloomFilters_[prefix.size() - 1]->lookupKey(prefix);
    if (!may_exist) return false;
    std::string query = prefix.substr(0, prefix.length() - 1);
    for (size_t i = 0; i < maxDoubtingLevel_; i++) {
        if (query.length() == 0) return true;
        may_exist = bloomFilters_[query.size() - 1]->lookupKey(query);
        if (!may_exist) return false;
        query = query.substr(0, query.length() - 1);
    }
    return true;
}

unsigned long long range_filtering::MultiPrefixBloomFilter::getMemoryUsage() const {
    uint64_t usage = sizeof(bloomFilters_);
    for (auto bf : bloomFilters_) {
        usage += bf->getMemoryUsage();
    }
    return usage;
}

std::vector<uint16_t> range_filtering::MultiPrefixBloomFilter::getNumberOfHashes() {
    auto number_of_hashes = std::vector<uint16_t>(bloomFilters_.size());
    for (size_t i = 0; i < bloomFilters_.size(); i++) {
        number_of_hashes[i] = bloomFilters_[i]->getNumberOfHashes();
    }
    return number_of_hashes;
}

std::vector<uint64_t> range_filtering::MultiPrefixBloomFilter::getBFsSizes() {
    auto sizes = std::vector<uint64_t>(bloomFilters_.size());
    for (size_t i = 0; i < bloomFilters_.size(); i++) {
        sizes[i] = bloomFilters_[i]->getSize();
    }
    return sizes;
}