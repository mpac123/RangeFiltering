#include "KarpRabinFingerprint.h"

namespace range_filtering {

    uint8_t KarpRabinFingerprint::generate_8bit(std::string &pattern) {
        uint64_t m = pattern.size();
        const uint16_t q = 251; // a prime number
        const uint16_t d = 256; // number of characters in the input alphabet;
        uint16_t h = 1;
        uint16_t p = 0; // hash value for pattern

        // The value of h is pow(d, m-1) mod q
        for (size_t i = 0; i < m; i++) {
            h = (h * d) % q;
        }

        // Calculate the hash value of pattern
        for (size_t i = 0; i < m; i++) {
            p = (d * p + pattern[i]) % q;
        }

        return p;
    }

}