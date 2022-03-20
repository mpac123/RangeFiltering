#ifndef RANGE_FILTERING_RANGEFILTER_H
#define RANGE_FILTERING_RANGEFILTER_H

#include <iostream>

namespace range_filtering {
    class RangeFilter {
    public:
        virtual bool lookupRange(const std::string& from, const std::string& to) = 0;
        virtual uint64_t getMemoryUsage() const = 0;
    };
}

#endif //RANGE_FILTERING_RANGEFILTER_H
