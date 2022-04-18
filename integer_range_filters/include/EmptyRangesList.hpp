#ifndef RANGE_FILTERING_EMPTYRANGESLIST_HPP
#define RANGE_FILTERING_EMPTYRANGESLIST_HPP

#include <vector>
#include <string>

#include "IntegerRangeFilter.hpp"

namespace range_filtering {
    class EmptyRangeList : public IntegerRangeFilter {
    public:
        EmptyRangeList(std::vector<
        uint32_t> &keys, float gamma);
        bool lookupRange(uint32_t keyLeft,uint32_t keyRight) override;
        uint64_t getMemoryUsage() const override;

    private:
        std::vector<std::pair<
        uint32_t,
        uint32_t>> emptyRangeList_;
        uint32_t low_;
        uint32_t high_;

        int32_t findPredecessor(int32_t i,int32_t j,int32_t key);
    };

    EmptyRangeList::EmptyRangeList(std::vector<uint32_t> &keys, float gamma) {
        if (keys.size() == 0) {
            low_ = 0;
            high_ = 0;
            return;
        }

        low_ = keys[0];
        high_ = keys[keys.size() - 1];

        std::vector<uint32_t> emptyIntervalsSizes = std::vector<uint32_t>();
        for (size_t i = 0 ; i < keys.size() - 1; i++) {
            emptyIntervalsSizes.push_back(keys[i+1] - keys[i]);
        }

        float average = (high_ - low_ + 0.0) / emptyIntervalsSizes.size();
        emptyRangeList_.emplace_back(0, keys[0]);
        for (size_t i = 0; i < emptyIntervalsSizes.size(); i++) {
            if (emptyIntervalsSizes[i] > gamma * average) {
                emptyRangeList_.emplace_back(keys[i] + 1, emptyIntervalsSizes[i]);
            }
        }
    }

    bool EmptyRangeList::lookupRange(uint32_t keyLeft,uint32_t keyRight) {
        if (keyLeft < low_ and keyRight < low_) return false;
        if (keyRight > high_ and keyLeft > high_) return false;

        if (keyLeft == low_) return true;
        if (keyRight == high_) return true;

        keyLeft = std::max(keyLeft, low_);
        keyRight = std::min(keyRight, high_);

        if (keyLeft > keyRight) return false;

        auto predecessorInd = findPredecessor(0, emptyRangeList_.size(), keyLeft);
        if (predecessorInd >= emptyRangeList_.size()) predecessorInd = emptyRangeList_.size() - 1;
        if (emptyRangeList_[predecessorInd].first + emptyRangeList_[predecessorInd].second == 0) return true;
        if (emptyRangeList_[predecessorInd].first + emptyRangeList_[predecessorInd].second - 1 > keyLeft
        && emptyRangeList_[predecessorInd].first + emptyRangeList_[predecessorInd].second - 1 > keyRight) {
            return false;
        } else {
            return true;
        }
    }

    int32_t EmptyRangeList::findPredecessor(int32_t i, int32_t j, int32_t key) {
        if (j <= i + 1) return i;
        int32_t ind = (i + j + 0.0) / 2;
        if (emptyRangeList_[ind].first == key) {
            return ind;
        }
        if (emptyRangeList_[ind].first >= key) {
            return findPredecessor(i, ind, key);
        }
        int32_t t = findPredecessor(ind + 1, j, key);
        return emptyRangeList_[t].first <= key ? t : ind;
    }

    uint64_t EmptyRangeList::getMemoryUsage() const  {
        return 2 * sizeof(int32_t) + emptyRangeList_.size() * 2 * sizeof(
                int32_t);
    }
}

#endif //RANGE_FILTERING_EMPTYRANGESLIST_HPP
