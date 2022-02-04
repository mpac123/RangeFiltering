#ifndef RANGE_FILTERING_KARPRABINFINGERPRINT_H
#define RANGE_FILTERING_KARPRABINFINGERPRINT_H

#include <iostream>

namespace range_filtering {

class KarpRabinFingerprint {
public:
    static uint8_t generate_8bit(std::string& pattern);
};
} // namespace range_filtering

#endif //RANGE_FILTERING_KARPRABINFINGERPRINT_H
