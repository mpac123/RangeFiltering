#ifndef RANGE_FILTERING_PREFIXFILTER_H
#define RANGE_FILTERING_PREFIXFILTER_H

#include <iostream>

namespace range_filtering {
class PrefixFilter {
public:
    virtual bool lookupPrefix(const std::string& prefix) = 0;
    virtual uint64_t getMemoryUsage() const = 0;
};
}

#endif //RANGE_FILTERING_PREFIXFILTER_H
