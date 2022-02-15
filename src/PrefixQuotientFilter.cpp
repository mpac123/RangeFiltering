#include <PrefixQuotientFilter.h>

namespace range_filtering {

    PrefixQuotientFilter::PrefixQuotientFilter(std::vector<std::string> &keys, uint32_t r, uint64_t max_doubting_level) {
        r_ = r;
        maxDoubtingLevel_ = max_doubting_level;
        auto prefixes = generateAllPrefixes(keys);

        if (prefixes.empty()) q_ = 2; else {
            auto q = int(std::ceil(std::log2(prefixes.size())));
            auto alpha = prefixes.size() / (double(2 << (q - 1)));
            if (alpha < 0.75) q_ = q; else q_ = q + 1;
        }

        n_ = prefixes.size();
        qf_init(&quotientFilter_, q_, r);
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
        auto may_exist = qf_may_contain(&quotientFilter_, getFingerprint(prefix));
        if (!may_exist) return false;
        std::string query = prefix.substr(0, prefix.length() - 1);
        for (size_t i = 0; i < maxDoubtingLevel_; i++) {
            if (query.length() == 0) return true;
            may_exist = qf_may_contain(&quotientFilter_, getFingerprint(query));
            if (!may_exist) return false;
            query = prefix.substr(0, query.length() - 1);
        }
        return true;
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