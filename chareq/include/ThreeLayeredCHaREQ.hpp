#ifndef RANGE_FILTERING_THREELAYEREDCHAREQ_HPP
#define RANGE_FILTERING_THREELAYEREDCHAREQ_HPP

#include <vector>
#include <string>
#include "Trie.hpp"
#include <MurmurHash3.h>
#include "RangeFilter.h"
#include "bitset"
#include <cassert>
#include <cmath>
#include <cstdint>
#include "popcount.hpp"

#define LOW_MASK(n) ((1ULL << (n)) - 1ULL)

namespace range_filtering {
    class ThreeLayeredCHaREQ : public RangeFilter {
    public:
        ThreeLayeredCHaREQ(std::vector<std::string> &keys, uint32_t firstLayerHeight,
                           uint32_t secondLayerHeight, float firstLayerSaturation,
                           float secondLayerSaturation, float thirdLayerSaturation,
                           uint32_t firstLayerCulledBits, )
    };
}

#endif //RANGE_FILTERING_THREELAYEREDCHAREQ_HPP
