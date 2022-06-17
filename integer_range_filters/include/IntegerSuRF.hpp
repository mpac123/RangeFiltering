#ifndef RANGE_FILTERING_INTEGERSURF_HPP
#define RANGE_FILTERING_INTEGERSURF_HPP

#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "../../external/SuRF/include/surf.hpp"
#include "IntegerRangeFilter.hpp"

namespace range_filtering {
    class IntegerSuRF : public IntegerRangeFilter {
    public:
        explicit IntegerSuRF(std::vector<uint32_t>& keys);
        bool lookupRange(uint32_t leftKey, uint32_t rightKey) override;
        uint64_t getMemoryUsage() const override;
    private:
        surf::SuRF *surf_;
        uint32_t key_length_;

        std::string convertIntToString(uint32_t);
    };

    IntegerSuRF::IntegerSuRF(std::vector<uint32_t> &keys) {
        std::string last_key = std::to_string(keys.at(keys.size() - 1));
        key_length_ = last_key.length();

        std::vector<std::string> str_keys = std::vector<std::string>();
        for (const auto& key : keys) {
            std::string str_key = convertIntToString(key);
            str_keys.push_back(str_key);
        }

        surf_ = new surf::SuRF(str_keys, surf::kReal, 0, 0);
    }

    bool IntegerSuRF::lookupRange(uint32_t leftKey, uint32_t rightKey) {
        auto leftKeyStr = convertIntToString(leftKey);
        auto rightKeyStr = convertIntToString(rightKey);
        return surf_->lookupRange(leftKeyStr, true, rightKeyStr, true);
    }

    std::string IntegerSuRF::convertIntToString(uint32_t i) {
        auto s = std::to_string(i);
        while (s.length() < key_length_) {
            s = "0" + s;
        }
        return s;
    }

    uint64_t IntegerSuRF::getMemoryUsage() const {
        return surf_->getMemoryUsage() + 8;
    }
}

#endif //RANGE_FILTERING_INTEGERSURF_HPP
