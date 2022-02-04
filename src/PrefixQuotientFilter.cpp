#include <PrefixQuotientFilter.h>

namespace range_filtering {

    PrefixQuotientFilter::PrefixQuotientFilter(std::vector<std::string> &keys, uint32_t q, uint32_t r) {
        q_ = q;
        r_ = r;
        auto prefixes = generateAllPrefixes(keys);
        n_ = prefixes.size();
        qf_init(&quotientFilter_, q, r);
        for (const auto& prefix : prefixes) {
            if (!qf_insert(&quotientFilter_, getFingerprint(prefix))) {
                failed_ = true;
                return;
            };
        }
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
        assert(!failed_);
        if (prefix.empty()) {
            return true;
        }
        return qf_may_contain(&quotientFilter_, getFingerprint(prefix));
    }

    uint64_t PrefixQuotientFilter::getMemoryUsage() const {
        assert(!failed_);
        return qf_table_size(q_, r_);
    }

    double PrefixQuotientFilter::getFalsePositiveProbability() {
        assert(!failed_);
        // According to Bender et al.
        return 1.0 - std::pow(1.0 - std::pow(2, - (int32_t) (q_+r_)), n_);
    }

    uint64_t PrefixQuotientFilter::getFingerprint(const std::string& prefix) {
        uint64_t hash[2];
        MurmurHash3_x64_128(prefix.c_str(), prefix.size(), 0, hash);
        return hash[0];
    }
}