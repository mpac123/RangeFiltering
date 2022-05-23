#ifndef RANGE_FILTERING_POPCOUNT_HPP
#define RANGE_FILTERING_POPCOUNT_HPP

#include <sys/types.h>
#include <stdio.h>
#include <stdint.h>

#define LOW_MASK(n) ((1ULL << (n)) - 1ULL)

#ifndef popcount
#define popcount __builtin_popcountll
#endif

namespace range_filtering {
    inline uint64_t getpopcount(uint64_t* bits, uint64_t start, uint64_t nbits) {
        if (nbits == 0) { return 0; }

        uint64_t count = 0;
        uint64_t s = start;
        while (nbits >= 64) {
            count += popcount(bits[s++]);
            nbits -= 64;
        }

        if (nbits > 0) {
            count += popcount(bits[s] & LOW_MASK(nbits));
        }

        return count;
    }
}

#endif //RANGE_FILTERING_POPCOUNT_HPP
