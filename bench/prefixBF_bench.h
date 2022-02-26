#ifndef RANGE_FILTERING_PREFIXBF_BENCH_H
#define RANGE_FILTERING_PREFIXBF_BENCH_H

#include <vector>
#include <unordered_set>
#include "PrefixBloomFilter.h"
#include "PrefixQuotientFilter.h"
#include "bench.h"
#include "SuRFFacade.h"
#include "MultiPrefixBloomFilter.h"
#include "MultiPrefixQuotientFilter.h"
#include "SurfingTrie.h"

namespace prefixBF_bench {

    struct PrefixBF_Stats {
        PrefixBF_Stats(range_filtering::PrefixBloomFilter *prefixBF, double FPR_, double creationTime_, double queryTime_) {
            arr_size = prefixBF->getBFSize();
            theoretical_FPR = prefixBF->getFPR();
            hashesCnt = prefixBF->getNumberOfHashes();
            FPR = FPR_;
            memoryUsage = prefixBF->getMemoryUsage();
            creationTime = creationTime_;
            queryTime = queryTime_;
        }
        PrefixBF_Stats(range_filtering::MultiPrefixBloomFilter *prefixBF, double FPR_, double creationTime_, double queryTime_) {
            arr_size = 0;
            for (auto size : prefixBF->getBFsSizes()) {
                arr_size += size;
            }
            theoretical_FPR = -1;
            hashesCnt = -1;
            FPR = FPR_;
            memoryUsage = prefixBF->getMemoryUsage();
            creationTime = creationTime_;
            queryTime = queryTime_;
        }
        uint32_t arr_size;
        double theoretical_FPR;
        uint8_t hashesCnt;
        double FPR;
        uint64_t memoryUsage;
        double creationTime;
        double queryTime;
    };

    inline std::ostream & operator<<(std::ostream & strm, const PrefixBF_Stats &s) {
        strm << s.memoryUsage << "\t" << s.FPR << "\t" << s.theoretical_FPR << "\t"
             << s.arr_size << "\t" << unsigned(s.hashesCnt) << "\t" << s.creationTime << "\t" << s.queryTime;
        return strm;
    }

    struct PrefixQF_Stats {
        PrefixQF_Stats(range_filtering::PrefixQuotientFilter *prefixQF, double FPR_, uint32_t r_, double creationTime_, double queryTime_) {
            falsePositiveProb = prefixQF->getFalsePositiveProbability();
            FPR = FPR_;
            memoryUsage = prefixQF->getMemoryUsage();
            q = prefixQF->getQ();
            r = r_;
            creationTime = creationTime_;
            queryTime = queryTime_;
        }
        PrefixQF_Stats(range_filtering::MultiPrefixQuotientFilter *prefixQF, double FPR_, uint32_t r_, double creationTime_, double queryTime_) {
            falsePositiveProb = -1;
            FPR = FPR_;
            memoryUsage = prefixQF->getMemoryUsage();
            q = -1;
            r = r_;
            creationTime = creationTime_;
            queryTime = queryTime_;
        }
        double falsePositiveProb;
        double FPR;
        uint64_t memoryUsage;
        uint32_t q;
        uint32_t r;
        double creationTime;
        double queryTime;
    };

    inline std::ostream & operator<<(std::ostream & strm, const PrefixQF_Stats &s) {
        strm << s.memoryUsage << "\t" << s.FPR << "\t" << s.falsePositiveProb << "\t"
             << s.q << "\t" << s.r << "\t" << s.creationTime << "\t" << s.queryTime;
        return strm;
    }

    void runTestsPBF(uint32_t start_size, uint32_t end_size, uint32_t interval,
                     std::vector<std::string> insert_keys,
                     std::vector<std::string> prefixes,
                     uint64_t max_doubting_level,
                     bool multilevel,
                     range_filtering::MultiPrefixBloomFilter::MemoryAllocationType memoryAllocationType,
                     double maxMemoryAllocationPercentageDifference) {
        auto trie = range_filtering::Trie(insert_keys);
        for (uint32_t s = start_size; s <= end_size; s += interval) {
            auto start = std::chrono::system_clock::now();
            range_filtering::PrefixFilter* prefix_BF;
            if (multilevel) {
                prefix_BF = new range_filtering::MultiPrefixBloomFilter(insert_keys, s, max_doubting_level,
                                                                        memoryAllocationType,
                                                                        maxMemoryAllocationPercentageDifference);
            } else {
                prefix_BF = new range_filtering::PrefixBloomFilter(insert_keys, s, max_doubting_level);
            }
            auto end = std::chrono::system_clock::now();

            std::chrono::duration<double> elapsed_seconds = end-start;
            auto [fpr, query_time] = bench::calculateFPR(prefix_BF, trie, prefixes);
            if (multilevel) {
                std::cout << PrefixBF_Stats((range_filtering::MultiPrefixBloomFilter *) prefix_BF, fpr,
                                            elapsed_seconds.count(), query_time) << std::endl;
            } else {
                std::cout << PrefixBF_Stats((range_filtering::PrefixBloomFilter *) prefix_BF, fpr,
                                            elapsed_seconds.count(), query_time) << std::endl;
            }
        }
    }

    void runTestsPQF(uint32_t start_r, uint32_t end_r,
                     std::vector<std::string> insert_keys,
                     std::vector<std::string> prefixes,
                     uint64_t max_doubting_level) {
        auto trie = range_filtering::Trie(insert_keys);
        for (uint32_t r = start_r; r <= end_r; r++) {
            auto start = std::chrono::system_clock::now();
            auto prefix_QF = new range_filtering::PrefixQuotientFilter(insert_keys, r, max_doubting_level);
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end-start;

            if (!prefix_QF->hasFailed()) {
                auto [fpr, query_time] = bench::calculateFPR(prefix_QF, trie, prefixes);
                std::cout << PrefixQF_Stats(prefix_QF, fpr, r, elapsed_seconds.count(), query_time)
                          << std::endl;
            } else {
                // std::cout << "failed" << std::endl;
            }
        }
    }

    void runTestsMultiPQF(uint32_t start_r, uint32_t end_r,
                     std::vector<std::string> insert_keys,
                     std::vector<std::string> prefixes,
                     uint64_t max_doubting_level) {
        auto trie = range_filtering::Trie(insert_keys);
        for (uint32_t r = start_r; r <= end_r; r++) {
            auto start = std::chrono::system_clock::now();
            auto prefix_QF = new range_filtering::MultiPrefixQuotientFilter(insert_keys, r, max_doubting_level);
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end-start;

            if (!prefix_QF->hasFailed()) {
                auto [fpr, query_time] = bench::calculateFPR(prefix_QF, trie, prefixes);
                std::cout << PrefixQF_Stats(prefix_QF, fpr, r, elapsed_seconds.count(), query_time)
                          << std::endl;
            }
        }
    }

    void runTestsSuRFReal(uint32_t start_real_bit, uint32_t end_real_bit,
                     std::vector<std::string> insert_keys,
                     std::vector<std::string> prefixes) {
        auto trie = range_filtering::Trie(insert_keys);
        for (size_t i = start_real_bit; i <= end_real_bit; i++) {
            auto start = std::chrono::system_clock::now();
            auto surf_real = new range_filtering::SuRFFacade(insert_keys, true, i);
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end-start;
            auto [fpr, query_time] = bench::calculateFPR(surf_real, trie, prefixes);
            std::cout << surf_real->getMemoryUsage() << "\t" << fpr << "\t" << end_real_bit << "\t"
                      << elapsed_seconds.count() << "\t" << query_time << std::endl;
        }
    }

    void runTestsSurfingTrie(uint32_t start_real_bit, uint32_t end_real_bit,
                          std::vector<std::string> insert_keys,
                          std::vector<std::string> prefixes) {
        auto trie = range_filtering::Trie(insert_keys);
        for (size_t i = start_real_bit; i <= end_real_bit; i++) {
            auto start = std::chrono::system_clock::now();
            auto surfing_trie = new range_filtering::SurfingTrie(insert_keys, i);
            auto surf_real = new range_filtering::SuRFFacade(insert_keys, true, i);
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end-start;
            auto [fpr, query_time] = bench::calculateFPR(surfing_trie, surf_real, trie, prefixes);
            std::cout << surfing_trie->getMemoryUsage() << "\t" << fpr << "\t" << end_real_bit << "\t"
                      << elapsed_seconds.count() << "\t" << query_time << std::endl;
        }
    }

};


#endif //RANGE_FILTERING_PREFIXBF_BENCH_H
