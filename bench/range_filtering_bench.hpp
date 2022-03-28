#ifndef RANGE_FILTERING_RANGE_FILTERING_BENCH_HPP
#define RANGE_FILTERING_RANGE_FILTERING_BENCH_HPP

#include "RangeFilter.h"
#include "bench.h"
#include "SuRFFacade.h"
#include "../rosetta/include/rosetta.hpp"
#include "../succinct_trie/include/fst.hpp"
#include "../splash/include/splash.hpp"
#include "../rosetta/include/lil_rosetta.hpp"

#include <vector>

namespace range_filtering_bench {

    struct RangeFilter_Stats {
        RangeFilter_Stats(range_filtering::RangeFilter* rangeFilter, double FPR_, double creationTime_, double queryTime_) {
            FPR = FPR_;
            memoryUsage = rangeFilter->getMemoryUsage();
            creationTime_ = creationTime;
            queryTime_ = queryTime;
        }
        double FPR;
        unsigned long long memoryUsage;
        double creationTime;
        double queryTime;
    };

    void runTestsSuRFReal(uint32_t start_real_bit, uint32_t end_real_bit,
                          std::vector<std::string> insert_keys,
                          std::vector<std::pair<std::string, std::string>> queries) {
        auto trie = range_filtering::Trie(insert_keys);
        for (size_t i = start_real_bit; i <= end_real_bit; i++) {
            auto start = std::chrono::system_clock::now();
            auto surf_real = new range_filtering::SuRFFacade(insert_keys, true, i);
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end-start;
            auto [fpr, query_time] = bench::calculateFPR(surf_real, trie, queries);
            std::cout << surf_real->getMemoryUsage() << "\t" << fpr << "\t" << end_real_bit << "\t"
                      << elapsed_seconds.count() << "\t" << query_time << "\t" << trie.getMemoryUsage() << std::endl;
        }
    }

    void runTestsRosetta(uint64_t min_size, uint64_t max_size, uint64_t step_size,
                         std::vector<std::string> &insert_keys,
                         std::vector<std::pair<std::string, std::string>> &queries) {
        auto trie = range_filtering::Trie(insert_keys);
        for (uint64_t size = min_size; size <= max_size; size += step_size) {
            auto start = std::chrono::system_clock::now();
            auto surf_real = new range_filtering_rosetta::Rosetta(insert_keys, size);
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end - start;
            auto[fpr, query_time] = bench::calculateFPR(surf_real, trie, queries);
            std::cout << surf_real->getMemoryUsage() << "\t" << fpr << "\t" << size << "\t"
                      << elapsed_seconds.count() << "\t" << query_time << "\t" << trie.getMemoryUsage() << std::endl;
        }
    }

    void runTestsLilRosetta(uint64_t min_size, uint64_t max_size, uint64_t step_size,
                         std::vector<std::string> &insert_keys,
                         std::vector<std::pair<std::string, std::string>> &queries) {
        auto trie = range_filtering::Trie(insert_keys);
        for (uint64_t size = min_size; size <= max_size; size += step_size) {
            auto start = std::chrono::system_clock::now();
            auto surf_real = new range_filtering_rosetta::LilRosetta(insert_keys, size);
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end - start;
            auto[fpr, query_time] = bench::calculateFPR(surf_real, trie, queries);
            std::cout << surf_real->getMemoryUsage() << "\t" << fpr << "\t" << size << "\t"
                      << elapsed_seconds.count() << "\t" << query_time << "\t" << trie.getMemoryUsage() << std::endl;
        }
    }

    void runTestsFST(std::vector<std::string> &insert_keys,
                     std::vector<std::pair<std::string, std::string>> &queries) {
        auto trie = range_filtering::Trie(insert_keys);
        auto start = std::chrono::system_clock::now();
        auto fst = new range_filtering::FST(insert_keys);
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        auto[fpr, query_time] = bench::calculateFPR(fst, trie, queries);
        std::cout << fst->getMemoryUsage() << "\t" << fpr << "\t" << "-" << "\t"
                  << elapsed_seconds.count() << "\t" << query_time << "\t" << trie.getMemoryUsage()
                  << std::endl;
    }

    void runTestsSplash(double cutoff, double restraint_min, double restraint_max, double restraint_interval,
                        std::vector<std::string> &insert_keys,
                        std::vector<std::pair<std::string, std::string>> &queries) {
        auto trie = range_filtering::Trie(insert_keys);
        for (auto restraint = restraint_min; restraint <= restraint_max; restraint += restraint_interval) {
            auto start = std::chrono::system_clock::now();
            auto splash = new range_filtering_splash::Splash(insert_keys, range_filtering_splash::SplashRestraintType::relative,
                                                             0, restraint, cutoff);
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end - start;
            auto[fpr, query_time] = bench::calculateFPR(splash, trie, queries);
            std::cout << splash->getMemoryUsage() << "\t" << fpr << "\t" << "-" << "\t"
                      << elapsed_seconds.count() << "\t" << query_time << "\t" << trie.getMemoryUsage()
                      << std::endl;
        }
    }
}

#endif //RANGE_FILTERING_RANGE_FILTERING_BENCH_HPP
