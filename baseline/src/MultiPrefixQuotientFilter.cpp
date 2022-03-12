#include <MultiPrefixQuotientFilter.h>

range_filtering::MultiPrefixQuotientFilter::MultiPrefixQuotientFilter(std::vector<std::string> &keys, uint32_t r,
                                                                      uint64_t max_doubting_level) {
    auto all_prefixes_list = generateAllPrefixes(keys);
    quotientFilters_ = std::vector<struct quotient_filter>();
    for (size_t i = 0; i < all_prefixes_list.size(); i++) {
        struct quotient_filter qf;
        uint32_t q = int(std::ceil(std::log2(all_prefixes_list[i].size())));
        auto alpha = all_prefixes_list[i].size() / (double(2 << (q - 1)));
        if (alpha > 0.6) q++;
        qf_init(&qf, q, r);
        for (const auto& prefix : all_prefixes_list[i]) {
            if (!qf_insert(&qf, getFingerprint(prefix))) {
                failed_ = true;
                return;
            }
        }
        quotientFilters_.push_back(qf);
        qs_.push_back(q);
    }
    maxDoubtingLevel_ = max_doubting_level;
    r_ = r;
}

std::vector<std::vector<std::string>> range_filtering::MultiPrefixQuotientFilter::generateAllPrefixes(
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

uint64_t range_filtering::MultiPrefixQuotientFilter::getFingerprint(const std::string& prefix) {
    uint64_t hash[2];
    MurmurHash3_x64_128(prefix.c_str(), prefix.size(), 0, hash);
    return hash[0];
}

bool range_filtering::MultiPrefixQuotientFilter::lookupPrefix(const std::string &prefix) {
    if (prefix.empty()) {
        return true;
    }
    if (prefix.size() > quotientFilters_.size()) {
        return false;
    }
    auto may_exist = qf_may_contain(&quotientFilters_[prefix.size() - 1], getFingerprint(prefix));
    if (!may_exist) return false;
    std::string query = prefix.substr(0, prefix.length() - 1);
    for (size_t i = 0; i < maxDoubtingLevel_; i++) {
        if (query.length() == 0) return true;
        may_exist = qf_may_contain(&quotientFilters_[query.size() - 1], getFingerprint(query));
        if (!may_exist) return false;
        query = query.substr(0, query.length() - 1);
    }
    return true;
}

uint64_t range_filtering::MultiPrefixQuotientFilter::getMemoryUsage() const {
    uint64_t usage = sizeof(quotientFilters_);
    for (auto q : qs_) {
        usage += qf_table_size(q, r_);
    }
    return usage;
}