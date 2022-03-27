#ifndef RANGE_FILTERING_SURFFACADE_H
#define RANGE_FILTERING_SURFFACADE_H

#include <vector>
#include <PrefixFilter.h>
#include <RangeFilter.h>
#include "../external/SuRF/include/surf.hpp"

namespace range_filtering {
class SuRFFacade : public PrefixFilter, public RangeFilter {
public:
    explicit SuRFFacade(std::vector<std::string> &keys, bool real, uint32_t bits_cnt);
    bool lookupPrefix(const std::string &prefix) override;
    bool lookupRange(const std::string &left, const std::string &right) override;
    uint64_t getMemoryUsage() const override;
    std::string getName() const override { return name_; }
private:
    surf::SuRF *surf_;
    std::string name_;
};

SuRFFacade::SuRFFacade(std::vector<std::string> &keys, bool real, uint32_t bits_cnt) {
    if (real) {
        surf_ = new surf::SuRF(keys, surf::kReal, 0, bits_cnt);
        name_ = "SuRF Real";
    } else {
        surf_ = new surf::SuRF(keys, surf::kNone, 0, 0);
        name_ = "SuRF Base";
    }
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
