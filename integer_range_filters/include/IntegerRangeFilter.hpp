#ifndef RANGE_FILTERING_INTEGERRANGEFILTER_HPP
#define RANGE_FILTERING_INTEGERRANGEFILTER_HPP

#include <iostream>

namespace range_filtering {
    class IntegerRangeFilter {
    public:
        virtual bool lookupRange(uint32_t keyLeft, uint32_t keyRight) = 0;
        virtual uint64_t getMemoryUsage() const = 0;
    };
}

#endif //RANGE_FILTERING_INTEGERRANGEFILTER_HPP
