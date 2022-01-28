#ifndef RANGE_FILTERING_PREFIXBF_BENCH_H
#define RANGE_FILTERING_PREFIXBF_BENCH_H

#include <vector>
#include <unordered_set>
#include "PrefixBloomFilter.h"
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

    inline std::ostream & operator<<(std::ostream & strm, const PrefixBF_Stats &s) {
        strm << s.arr_size << "\t" << s.memoryUsage << "\t" << s.FPR << "\t" << s.theoretical_FPR << "\t" << unsigned(s.hashesCnt);
        return strm;
    }

    void runTests(uint32_t start_size, uint32_t end_size, uint32_t interval,
                                         std::vector<std::string> insert_keys,
                                         std::unordered_set<std::string> prefixes) {
        auto trie = range_filtering::Trie(insert_keys);
        for (uint32_t s = start_size; s <= end_size; s += interval) {
            auto prefix_BF = new range_filtering::PrefixBloomFilter(insert_keys, s);
            std::cout << PrefixBF_Stats(prefix_BF, bench::calculateFPR(prefix_BF, trie, prefixes)) << std::endl;
        }
    }

};


#endif //RANGE_FILTERING_PREFIXBF_BENCH_H
