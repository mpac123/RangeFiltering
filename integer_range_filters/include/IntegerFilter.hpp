#ifndef RANGE_FILTERING_INTEGERFILTER_HPP
#define RANGE_FILTERING_INTEGERFILTER_HPP

#include "RangeFilter.h"
#include "IntegerRangeFilter.hpp"
#include <vector>

namespace range_filtering {
    class IntegerFilter : public IntegerRangeFilter {
    public:
        IntegerFilter(RangeFilter& filter, uint32_t max_key_length)
            : filter_(filter), key_length_(max_key_length) {};
        bool lookupRange(uint32_t leftKey, uint32_t rightKey) override;
        uint64_t getMemoryUsage() const override;
        static uint32_t parseIntegersToStrings(std::vector<std::string>& stringKeys,
                                               std::vector<uint32_t>& integerKeys);
    private:
        RangeFilter& filter_;
        uint32_t key_length_;
        static std::string convertIntToString(uint32_t int_key, uint32_t key_length);
    };

    uint32_t IntegerFilter::parseIntegersToStrings(std::vector<std::string> &stringKeys, std::vector<uint32_t> &integerKeys) {
        std::string last_key = std::to_string(integerKeys.at(integerKeys.size() - 1));
        uint32_t key_length = last_key.length();

        for (const auto& key : integerKeys) {
            std::string str_key = convertIntToString(key, key_length);
            stringKeys.push_back(str_key);
        }
        return key_length;
    }

    std::string IntegerFilter::convertIntToString(uint32_t i, uint32_t key_length) {
        auto s = std::to_string(i);
        while (s.length() < key_length) {
            s = "0" + s;
        }
        return s;
    }

    bool IntegerFilter::lookupRange(uint32_t leftKey, uint32_t rightKey) {
        auto leftKeyStr = convertIntToString(leftKey, key_length_);
        auto rightKeyStr = convertIntToString(rightKey, key_length_);
        return filter_.lookupRange(leftKeyStr, rightKeyStr);
    }

    uint64_t IntegerFilter::getMemoryUsage() const {
        return filter_.getMemoryUsage() + 8;
    }
}
#endif //RANGE_FILTERING_INTEGERFILTER_HPP
