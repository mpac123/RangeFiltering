#include "BloomFiltersEnsemble.h"

range_filtering::BloomFiltersEnsemble::BloomFiltersEnsemble(std::vector<std::vector<std::string>> &keys,
                                                            uint32_t total_BFs_size,
                                                            double maximalLevelPenalty) {
    padding_ = 0;
    for (size_t i = 0; i < keys.size(); i++) {
        if (keys[i].size() > 0) {
            padding_ = i;
            break;
        }
    }

    auto all_prefixes_cnt = 0;
    for (const auto& prefixes : keys) {
        all_prefixes_cnt += prefixes.size();
    }

    bloomFilters_ = std::vector<bloom_filter::BloomFilter*>(keys.size() - padding_);
    for (size_t i = 0; i < keys.size() - padding_; i++) {
        auto penalty = 1. - ((i + 0.0) / (keys.size() - padding_ - 1.0) * maximalLevelPenalty);
        auto BF_size = uint32_t(std::round((keys[padding_].size() + 0.) / all_prefixes_cnt * total_BFs_size * penalty));
        if (BF_size == 0) BF_size = 1;
        bloomFilters_[i] = new bloom_filter::BloomFilter(keys[i + padding_], BF_size);
    }
}

bool range_filtering::BloomFiltersEnsemble::lookupKey(const std::string &prefix) const {
    if ((int)prefix.size() - (int)padding_ > (int)bloomFilters_.size()) {
        return false;
    }
    if (prefix.size() <= padding_) return false;
    return bloomFilters_[prefix.size() - 1 - padding_]->lookupKey(prefix);
}

uint64_t range_filtering::BloomFiltersEnsemble::getMemoryUsage() const {
    uint64_t usage = sizeof(bloomFilters_);
    for (auto bf : bloomFilters_) {
        usage += bf->getMemoryUsage();
    }
    // 4 bytes for storing padding
    return usage + 4;
}

