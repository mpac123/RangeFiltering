#ifndef RANGE_FILTERING_RANGEBFKRNOENCODING_HPP
#define RANGE_FILTERING_RANGEBFKRNOENCODING_HPP

#include <iostream>
#include <vector>
#include "PrefixFilter.h"
#include "RangeFilter.h"
#include "BloomFilter.h"
#include <bitset>
#include "BFbasic.hpp"
#include "set"
#include "KRFingerprintGeneratorNoEncoding.hpp"

namespace range_filtering {
    class RangeBFKRNoEncoding : public PrefixFilter, public RangeFilter {
    public:
        RangeBFKRNoEncoding(std::vector<std::string>& keys, uint32_t m);
        bool lookupPrefix(const std::string& key) override;
        bool lookupRange(const std::string& leftKey, const std::string& rightKey) override;
        uint64_t getMemoryUsage() const override;
    private:
        BFbasic *bloomFilter_;
        const char endOfWord = 127;

        uint64_t countDistinctPrefixes(std::vector<std::string>& keys);
    };

    RangeBFKRNoEncoding::RangeBFKRNoEncoding(std::vector<std::string> &keys, uint32_t m) {
        auto n = countDistinctPrefixes(keys) + keys.size();
        bloomFilter_ = new BFbasic(n, m);
        for (auto& key : keys) {
            auto keyWithEnd = key + endOfWord;
            auto fingerprintGenerator = KRFingerprintGeneratorNoEncoding(keyWithEnd, bloomFilter_->getK());
            for (size_t i = 1; i <= keyWithEnd.length(); i++) {
                auto hashes = fingerprintGenerator.next();
                for (size_t j = 0; j < bloomFilter_->getK(); j++) {
                    bloomFilter_->insert(hashes[j]);
                }
            }
        }
    }

    uint64_t RangeBFKRNoEncoding::countDistinctPrefixes(std::vector<std::string>& keys) {
        auto set = std::set<std::string>();
        for (auto key : keys) {
            for (size_t i = 1; i <= key.length(); i++) {
                set.insert(key.substr(0, i));
            }
        }
        return set.size();
    }

    bool RangeBFKRNoEncoding::lookupPrefix(const std::string &key) {
        if (key.empty()) {
            return true;
        }
        auto fingerprintGenerator = KRFingerprintGeneratorNoEncoding(key, bloomFilter_->getK());
        for (size_t i = 0; i < key.length(); i++) {
            if (!bloomFilter_->lookupKey(fingerprintGenerator.next())) return false;
        }
        return true;
    }

    bool RangeBFKRNoEncoding::lookupRange(const std::string &leftKey, const std::string &rightKey) {
        if (leftKey > rightKey) return false;
        std::string common_prefix;
        for (size_t i = 0; i < std::min(leftKey.length(), rightKey.length()); i++) {
            if (leftKey[i] != rightKey[i]) break;
            common_prefix += leftKey[i];
        }

        // All the prefixes in the common_prefix should exist in BF
        auto fingerprintGenerator = KRFingerprintGeneratorNoEncoding(leftKey, bloomFilter_->getK());
        std::vector<std::uint64_t> currentPrefixHash = std::vector<uint64_t>(bloomFilter_->getK());
        for (size_t i = 0; i < common_prefix.length(); i++) {
            currentPrefixHash = fingerprintGenerator.next();
            if (!bloomFilter_->lookupKey(currentPrefixHash)) return false;
        }

        // Look up the middle range
        if (leftKey != common_prefix) {
            auto leftHash = fingerprintGenerator.calculateNext(currentPrefixHash, leftKey.at(common_prefix.length()));
            auto rightHash = fingerprintGenerator.calculateNext(currentPrefixHash, rightKey.at(common_prefix.length()));

            if (bloomFilter_->lookupKey(fingerprintGenerator.calculateNext(leftHash, endOfWord))) return true;
            auto currentHash = fingerprintGenerator.increaseHash(leftHash);
            while (currentHash.at(0) != rightHash.at(0)) {
                if (bloomFilter_->lookupKey(currentHash)) return true;
                fingerprintGenerator.increaseHashInPlace(currentHash);
            }
            if (bloomFilter_->lookupKey(rightHash)) return true;

            // Lookup the left subtrees
            for (size_t i = common_prefix.length(); i < leftKey.length(); i++) {
                if (!bloomFilter_->lookupKey(leftHash)) break;
                leftHash = fingerprintGenerator.calculateNext(leftHash, leftKey.at(i));
                rightHash = fingerprintGenerator.calculateNext(rightHash, char(127));

                if (bloomFilter_->lookupKey(fingerprintGenerator.calculateNext(leftHash, endOfWord))) return true;
                currentHash = fingerprintGenerator.increaseHash(leftHash);
                while (currentHash.at(0) != rightHash.at(0)) {
                    if (bloomFilter_->lookupKey(currentHash)) return true;
                    fingerprintGenerator.increaseHashInPlace(currentHash);
                }
                if (bloomFilter_->lookupKey(rightHash)) return true;
            }
        } else {
            if (bloomFilter_->lookupKey(fingerprintGenerator.calculateNext(currentPrefixHash, endOfWord))) return true;
        }

        // Lookup the right subtrees
        auto leftHash = currentPrefixHash;
        auto rightHash = currentPrefixHash;
        for (size_t i = common_prefix.length(); i < rightKey.length(); i++) {
            if (!bloomFilter_->lookupKey(rightHash)) break;
            leftHash = fingerprintGenerator.calculateNext(leftHash, char(0));
            rightHash = fingerprintGenerator.calculateNext(rightHash, rightKey.at(i));

            auto currentHash = leftHash;
            while (currentHash.at(0) != rightHash.at(0)) {
                if (bloomFilter_->lookupKey(currentHash)) return true;
                fingerprintGenerator.increaseHashInPlace(currentHash);
            }
            if (bloomFilter_->lookupKey(rightHash)) return true;
        }

        return false;
    }

    uint64_t RangeBFKRNoEncoding::getMemoryUsage() const {
        return bloomFilter_->getMemoryUsage() + (uint64_t) 256 / 8;
    }
}
#endif //RANGE_FILTERING_RANGEBFKRNOENCODING_HPP
