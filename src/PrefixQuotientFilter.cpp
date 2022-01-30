#include <PrefixQuotientFilter.h>

namespace range_filtering {

    PrefixQuotientFilter::PrefixQuotientFilter(std::vector<std::string> &keys, uint32_t q, uint32_t r) {
        q_ = q;
        r_ = r;
        auto prefixes = generateAllPrefixes(keys);
        quotientFilter_ = new quotient_filter::QuotientFilter(prefixes, q, r);
    }

    std::vector<std::string> PrefixQuotientFilter::generateAllPrefixes(std::vector<std::string> &keys) {
        std::unordered_set<std::string> all_prefixes = std::unordered_set<std::string>();
        for (const auto& key : keys) {
            for (size_t i = 1; i <= key.length(); i++) {
                all_prefixes.insert(key.substr(0, i));
            }
        }
        auto result = std::vector<std::string>(all_prefixes.begin(), all_prefixes.end());
        return result;
    }

    bool PrefixQuotientFilter::lookupPrefix(const std::string &prefix) {
        if (prefix.empty()) {
            return true;
        }
        return quotientFilter_->lookupKey(prefix);
    }

    uint64_t PrefixQuotientFilter::getMemoryUsage() const {
        return quotientFilter_->getMemoryUsage();
    }
}