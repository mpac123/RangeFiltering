#ifndef RANGE_FILTERING_KRFINGERPRINTGENERATORNOENCODING_HPP
#define RANGE_FILTERING_KRFINGERPRINTGENERATORNOENCODING_HPP

#include <vector>
#include <iostream>
#include <bitset>
#include <assert.h>

namespace range_filtering {
    class KRFingerprintGeneratorNoEncoding {
    public:
        KRFingerprintGeneratorNoEncoding(const std::string& pattern, uint32_t k);
        std::vector<uint64_t> next();
        std::vector<uint64_t> calculateNext(std::vector<uint64_t>& currentHash, char character);
        std::vector<uint64_t> increaseHash(std::vector<uint64_t>& currentHash);
        void increaseHashInPlace(std::vector<uint64_t>& hash);
    private:
        uint32_t k_;
        uint32_t i_ = 0;
        std::vector<uint64_t> current_;
        uint64_t d_;
        std::string pattern_;

        const std::vector<uint64_t> primes = {
                911398991,
                972666347,
                952664521,
                937552883,
                957667369,
                911404069,
                911411407,
                911421647,
                911440273,
                911465669,
                911527481,
                901554919,
                911574529,
                911598593,
                911619389,
                911631869,
        };
        const std::vector<uint64_t> ds = {
                937555517,
                937597291,
                927598963,
                927323857,
                927325507,
                987324421,
                897324227,
                900324137,
                909323873,
                910124459,
                920124503,
                920126533,
                980123513,
                980133883,
                987252857,
                987264611
        };
    };

    KRFingerprintGeneratorNoEncoding::KRFingerprintGeneratorNoEncoding(const std::string &pattern, uint32_t k) {
        assert(k <= 16);
        k_ = k;
        pattern_ = pattern;
        i_ = 0;
        current_ = std::vector<uint64_t>();
        for (size_t i = 0; i < k; i++) {
            current_.push_back(0);
        }
    }

    std::vector<uint64_t> KRFingerprintGeneratorNoEncoding::next() {
        for (size_t i = 0; i < k_; i++) {
            current_[i] = (current_[i]*ds[i] + pattern_[i_]) % primes[i];
        }
        i_++;
        return current_;
    }

    std::vector<uint64_t> KRFingerprintGeneratorNoEncoding::calculateNext(std::vector<uint64_t>& currentHash, char character) {
        auto new_hash = std::vector<uint64_t>();
        for (size_t i = 0; i < k_; i++) {
            new_hash.push_back((currentHash[i]*ds[i] + character) % primes[i]);
        }
        return new_hash;
    }

    std::vector<uint64_t> KRFingerprintGeneratorNoEncoding::increaseHash(std::vector<uint64_t>& currentHash) {
        auto new_hash = std::vector<uint64_t>();
        for (size_t i = 0; i < k_; i++) {
            new_hash.push_back((currentHash[i] + 1) % primes[i]);
        }
        return new_hash;
    }

    void KRFingerprintGeneratorNoEncoding::increaseHashInPlace(std::vector<uint64_t> &hash) {
        for (size_t i = 0; i < k_; i++) {
            hash[i] = (hash[i] + 1) % primes[i];
        }
    }
}

#endif //RANGE_FILTERING_KRFINGERPRINTGENERATORNOENCODING_HPP
