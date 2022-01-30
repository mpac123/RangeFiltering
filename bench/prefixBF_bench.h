#ifndef RANGE_FILTERING_PREFIXBF_BENCH_H
#define RANGE_FILTERING_PREFIXBF_BENCH_H

#include <vector>
#include <unordered_set>
#include "PrefixBloomFilter.h"
#include "PrefixQuotientFilter.h"
#include "bench.h"

namespace prefixBF_bench {

    struct PrefixBF_Stats {
        PrefixBF_Stats(range_filtering::PrefixBloomFilter *prefixBF, double FPR_) {
            arr_size = prefixBF->getBFSize();
            theoretical_FPR = prefixBF->getFPR();
            hashesCnt = prefixBF->getNumberOfHashes();
            FPR = FPR_;
            memoryUsage = prefixBF->getMemoryUsage();
        }
        uint32_t arr_size;
        double theoretical_FPR;
        uint8_t hashesCnt;
        double FPR;
        uint64_t memoryUsage;
    };

    struct PrefixQF_Stats {
        PrefixQF_Stats(range_filtering::PrefixQuotientFilter *prefixQF, double FPR_, uint32_t q_, uint32_t r_) {
            falsePositiveProb = prefixQF->getFalsePositiveProbability();
            FPR = FPR_;
            memoryUsage = prefixQF->getMemoryUsage();
            q = q_;
            r = r_;
        }
        double falsePositiveProb;
        double FPR;
        uint64_t memoryUsage;
        uint32_t q;
        uint32_t r;
    };

    inline std::ostream & operator<<(std::ostream & strm, const PrefixBF_Stats &s) {
        strm << s.arr_size << "\t" << s.memoryUsage << "\t" << s.FPR << "\t" << s.theoretical_FPR << "\t" << unsigned(s.hashesCnt);
        return strm;
    }

    inline std::ostream & operator<<(std::ostream & strm, const PrefixQF_Stats &s) {
        strm << s.memoryUsage << "\t" << s.FPR << "\t" << s.falsePositiveProb << "\t" << s.q << "\t" << s.r;
        return strm;
    }

    void runTestsPBF(uint32_t start_size, uint32_t end_size, uint32_t interval,
                                         std::vector<std::string> insert_keys,
                                         std::unordered_set<std::string> prefixes) {
        auto trie = range_filtering::Trie(insert_keys);
        for (uint32_t s = start_size; s <= end_size; s += interval) {
            auto prefix_BF = new range_filtering::PrefixBloomFilter(insert_keys, s);
            std::cout << PrefixBF_Stats(prefix_BF, bench::calculateFPR(prefix_BF, trie, prefixes)) << std::endl;
        }
    }

    void runTestsPQF(uint32_t start_q, int32_t end_q, int32_t start_r, int32_t end_r,
                     std::vector<std::string> insert_keys,
                     std::unordered_set<std::string> prefixes) {
        auto trie = range_filtering::Trie(insert_keys);
        for (uint32_t q = start_q; q <= end_q; q++) {
            for (uint32_t r = start_r; r <= end_r; r++) {
                auto prefix_QF = new range_filtering::PrefixQuotientFilter(insert_keys, q, r);
                std::cout << q << " " << r << std::endl;
                if (!prefix_QF->hasFailed()) {
                    std::cout << PrefixQF_Stats(prefix_QF, bench::calculateFPR(prefix_QF, trie, prefixes), q, r)
                              << std::endl;
                }
            }
        }
    }

};


#endif //RANGE_FILTERING_PREFIXBF_BENCH_H
