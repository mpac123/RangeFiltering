#ifndef RANGE_FILTERING_SURFFACADE_H
#define RANGE_FILTERING_SURFFACADE_H

#include <vector>
#include <PrefixFilter.h>
#include <RangeFilter.h>
#include "../external/SuRF/include/surf.hpp"

namespace range_filtering {
class SuRFFacade : public PrefixFilter, public RangeFilter {
public:
    explicit SuRFFacade(std::vector<std::string> &keys, bool real, bool hash, uint32_t real_bits_cnt, uint32_t hash_bit_cnt);
    explicit SuRFFacade(std::vector<std::string> &keys, uint32_t real_bits_cnt, uint32_t dense_fraction);
    bool lookupPrefix(const std::string &prefix) override;
    bool lookupRange(const std::string &left, const std::string &right) override;
    uint64_t getMemoryUsage() const override;
private:
    surf::SuRF *surf_;
};

SuRFFacade::SuRFFacade(std::vector<std::string> &keys, bool real, bool hash, uint32_t real_bits_cnt, uint32_t hash_bit_cnt) {
    if (real && hash) {
        surf_ = new surf::SuRF(keys, surf::kMixed, hash_bit_cnt, real_bits_cnt);
    } else if (real) {
        surf_ = new surf::SuRF(keys, surf::kReal, 0, real_bits_cnt);
    } else if (hash) {
        surf_ = new surf::SuRF(keys, surf::kMixed, hash_bit_cnt, 0);
    } else {
        surf_ = new surf::SuRF(keys, surf::kNone, 0, 0);
    }
}

SuRFFacade::SuRFFacade(std::vector<std::string> &keys, uint32_t real_bits_cnt, uint32_t sparse_to_dense_ratio) {
    surf_ = new surf::SuRF(keys, true, sparse_to_dense_ratio, surf::kReal, 0, real_bits_cnt);
}

bool SuRFFacade::lookupPrefix(const std::string &prefix) {
    char last_char = prefix[prefix.length() - 1];
    std::string right_key = prefix;
    right_key[prefix.length() - 1] = last_char + 1;
    return surf_->lookupRange(prefix, true, right_key, false);
}

uint64_t SuRFFacade::getMemoryUsage() const {
    return surf_->getMemoryUsage();
}

bool SuRFFacade::lookupRange(const std::string &left, const std::string &right) {
    return surf_->lookupRange(left, true, right, true);
}
}

#endif //RANGE_FILTERING_SURFFACADE_H
