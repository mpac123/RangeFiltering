#ifndef RANGE_FILTERING_RANGE_FILTERING_BENCH_HPP
#define RANGE_FILTERING_RANGE_FILTERING_BENCH_HPP

#include "RangeFilter.h"
#include "bench.h"
#include "SuRFFacade.h"
#include "../rosetta/include/rosetta.hpp"
#include "../succinct_trie/include/fst.hpp"
#include "../splash/include/splash.hpp"
#include "../rosetta/include/lil_rosetta.hpp"
#include "../range_filters/include/BloomRangeFilter.hpp"
#include "../range_filters/include/RangeBFKR.h"
#include "../bloomed_range_splash/include/bloomed_range_splash.hpp"
#include "../range_filters/include/QuotientTrie.hpp"
#include "../range_filters/include/ReCHaREQ.hpp"
#include <vector>
#include "../chareq/include/CHaREQ.hpp"
#include "../chareq/include/BloomedCHaREQ.hpp"
#include "../chareq/include/LayeredCHaREQ.hpp"

namespace range_filtering_bench {

    void runTestsSuRFReal(uint32_t start_real_bit, uint32_t end_real_bit,
                          std::vector<std::string> insert_keys,
                          std::vector<std::pair<std::string, std::string>> queries) {
        auto trie = range_filtering::Trie(insert_keys);
        for (size_t i = start_real_bit; i <= end_real_bit; i++) {
            auto start = std::chrono::system_clock::now();
            auto surf_real = new range_filtering::SuRFFacade(insert_keys, true, false, i, 0);
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end-start;
            auto [fpr, query_time] = bench::calculateFPR(surf_real, trie, queries);
            std::cout << surf_real->getMemoryUsage() << "\t" << fpr << "\t" << end_real_bit << "\t"
                      << elapsed_seconds.count() << "\t" << query_time << "\t" << trie.getMemoryUsage() << std::endl;
        }
    }

    void runTestsSuRFRealDense(uint32_t ratio_start, uint32_t ration_end,
                          std::vector<std::string> insert_keys,
                          std::vector<std::pair<std::string, std::string>> queries, uint32_t suffix_len) {
        auto trie = range_filtering::Trie(insert_keys);
        for (size_t i = ratio_start; i <= ration_end; i++) {
            auto start = std::chrono::system_clock::now();
            auto surf_real = new range_filtering::SuRFFacade(insert_keys, suffix_len, i);
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end-start;
            auto [fpr, query_time] = bench::calculateFPR(surf_real, trie, queries);
            std::cout << surf_real->getMemoryUsage() << "\t" << fpr << "\t" << i << "\t"
                      << elapsed_seconds.count() << "\t" << query_time << "\t" << trie.getMemoryUsage() << std::endl;
        }
    }

    void runTestsSuRFHash(uint32_t start_real_bit, uint32_t end_real_bit,
                          std::vector<std::string> insert_keys,
                          std::vector<std::pair<std::string, std::string>> queries) {
        auto trie = range_filtering::Trie(insert_keys);
        for (size_t i = start_real_bit; i <= end_real_bit; i++) {
            auto start = std::chrono::system_clock::now();
            auto surf_real = new range_filtering::SuRFFacade(insert_keys, false, true, 0, i);
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end-start;
            auto [fpr, query_time] = bench::calculateFPR(surf_real, trie, queries);
            std::cout << surf_real->getMemoryUsage() << "\t" << fpr << "\t" << end_real_bit << "\t"
                      << elapsed_seconds.count() << "\t" << query_time << "\t" << trie.getMemoryUsage() << std::endl;
        }
    }

    void runTestsSuRFMixed(uint32_t start_real_bit, uint32_t end_real_bit,
                           std::vector<std::string> insert_keys,
                           std::vector<std::pair<std::string, std::string>> queries) {
        auto trie = range_filtering::Trie(insert_keys);
        for (size_t i = start_real_bit; i <= end_real_bit; i++) {
            auto start = std::chrono::system_clock::now();
            auto surf_real = new range_filtering::SuRFFacade(insert_keys, true, true, i, i);
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
                         float top_levels_penalty,
                         std::vector<std::string> &insert_keys,
                         std::vector<std::pair<std::string, std::string>> &queries) {
        auto trie = range_filtering::Trie(insert_keys);
        for (uint64_t size = min_size; size <= max_size; size += step_size) {
            auto start = std::chrono::system_clock::now();
            auto surf_real = new range_filtering_rosetta::LilRosetta(insert_keys, size, top_levels_penalty);
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

    void runTestsRangeBF(uint64_t min_size, uint64_t max_size, uint64_t step_size,
                         std::vector<std::string> &insert_keys,
                         std::vector<std::pair<std::string, std::string>> &queries) {
        auto trie = range_filtering::Trie(insert_keys);
        for (uint64_t size = min_size; size <= max_size; size += step_size) {
            auto start = std::chrono::system_clock::now();
            auto filter = new range_filtering::BloomRangeFilter(insert_keys, size);
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end - start;
            auto[fpr, query_time] = bench::calculateFPR(filter, trie, queries);
            std::cout << filter->getMemoryUsage() << "\t" << fpr << "\t" << size << "\t"
                      << elapsed_seconds.count() << "\t" << query_time << "\t" << trie.getMemoryUsage() << std::endl;
        }
    }

    void runTestsRangeKRBF(uint64_t min_size, uint64_t max_size, uint64_t step_size,
                         std::vector<std::string> &insert_keys,
                         std::vector<std::pair<std::string, std::string>> &queries) {
        auto trie = range_filtering::Trie(insert_keys);
        for (uint64_t size = min_size; size <= max_size; size += step_size) {
            auto start = std::chrono::system_clock::now();
            auto filter = new range_filtering::RangeBFKR(insert_keys, size);
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end - start;
            auto[fpr, query_time] = bench::calculateFPR(filter, trie, queries);
            std::cout << filter->getMemoryUsage() << "\t" << fpr << "\t" << size << "\t"
                      << elapsed_seconds.count() << "\t" << query_time << "\t" << trie.getMemoryUsage() << std::endl;
        }
    }

    void runTestsBloomedRangeSplash(uint64_t min_size, uint64_t max_size, uint64_t step_size,
                           uint64_t fst_height,
                           std::vector<std::string> &insert_keys,
                           std::vector<std::pair<std::string, std::string>> &queries) {
        auto trie = range_filtering::Trie(insert_keys);
        for (uint64_t size = min_size; size <= max_size; size += step_size) {
            auto start = std::chrono::system_clock::now();
            auto filter = new range_filtering_bloomed_range_splash::BloomedRangeSplash(insert_keys, fst_height, size);
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end - start;
            auto[fpr, query_time] = bench::calculateFPR(filter, trie, queries);
            std::cout << filter->getMemoryUsage() << "\t" << fpr << "\t" << size << "\t"
                      << elapsed_seconds.count() << "\t" << query_time << "\t" << trie.getMemoryUsage() << std::endl;
        }
    }

    void runTestsOldChareq(float fill_in_min, float fill_in_max, float fill_in_step,
                        uint32_t top_layer_height,
                        std::vector<std::string> &insert_keys,
                        std::vector<std::pair<std::string, std::string>> &queries) {
        auto trie = range_filtering::Trie(insert_keys);
        for (float fill_in_coeff = fill_in_min; fill_in_coeff <= fill_in_max; fill_in_coeff += fill_in_step) {
            auto start = std::chrono::system_clock::now();
            auto filter = new range_filters::QuotientTrie(insert_keys, 1.0, fill_in_coeff);
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end - start;
            auto[fpr, query_time] = bench::calculateFPR(filter, trie, queries);
            std::cout << filter->getMemoryUsage() << "\t" << fpr << "\t" << fill_in_coeff << "\t"
                      << elapsed_seconds.count() << "\t" << query_time << "\t" << trie.getMemoryUsage() << std::endl;
        }
    }

    void runTestsChareq(float fill_in_min, float fill_in_max, float fill_in_step,
                                    uint32_t top_layer_height,
                                    std::vector<std::string> &insert_keys,
                                    std::vector<std::pair<std::string, std::string>> &queries) {
        auto trie = range_filtering::Trie(insert_keys);
        for (float fill_in_coeff = fill_in_min; fill_in_coeff <= fill_in_max; fill_in_coeff += fill_in_step) {
            auto start = std::chrono::system_clock::now();
            auto filter = new range_filtering::CHaREQ(insert_keys, fill_in_coeff);
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end - start;
            auto[fpr, query_time] = bench::calculateFPR(filter, trie, queries);
            std::cout << filter->getMemoryUsage() << "\t" << fpr << "\t" << fill_in_coeff << "\t"
                      << elapsed_seconds.count() << "\t" << query_time << "\t" << trie.getMemoryUsage() << std::endl;
        }
    }

    void runTestsBloomedChareq(float saturation,
                        uint32_t bf_size_min, uint32_t bf_size_max, uint32_t bf_size_step,
                        std::vector<std::string> &insert_keys,
                        std::vector<std::pair<std::string, std::string>> &queries) {
        auto trie = range_filtering::Trie(insert_keys);
        for (uint32_t bf_size = bf_size_min; bf_size <= bf_size_max; bf_size += bf_size_step) {
            auto start = std::chrono::system_clock::now();
            auto filter = new range_filtering::BloomedCHaREQ(insert_keys, saturation, bf_size);
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end - start;
            auto[fpr, query_time] = bench::calculateFPR(filter, trie, queries);
            std::cout << filter->getMemoryUsage() << "\t" << fpr << "\t" << bf_size << "\t"
                      << elapsed_seconds.count() << "\t" << query_time << "\t" << trie.getMemoryUsage() << std::endl;
        }
    }

    void runTestsLayeredChareq(uint32_t first_layer_height, float first_level_saturation,
                               float second_level_sat_min, float second_layer_sat_max, float second_layer_sat_step,
                               uint32_t first_layer_culled_bits, uint32_t second_layer_culled_bits,
                               std::vector<std::string> &insert_keys,
                               std::vector<std::pair<std::string, std::string>> &queries) {
        auto trie = range_filtering::Trie(insert_keys);
        for (float second_layer_sat = second_level_sat_min; second_layer_sat <= second_layer_sat_max; second_layer_sat += second_layer_sat_step) {
            auto start = std::chrono::system_clock::now();
            auto filter = new range_filtering::LayeredCHaREQ(insert_keys, first_layer_height, first_level_saturation,
                                                             second_layer_sat, first_layer_culled_bits, second_layer_culled_bits);
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end - start;
            auto[fpr, query_time] = bench::calculateFPR(filter, trie, queries);
            std::cout << filter->getMemoryUsage() << "\t" << fpr << "\t" << second_layer_sat << "\t"
                      << elapsed_seconds.count() << "\t" << query_time << "\t" << trie.getMemoryUsage() << std::endl;
        }
    }

    void runTestsReChareq(float fill_in_min, float fill_in_max, float fill_in_step,
                        uint32_t top_layer_height, uint32_t bits_per_char,
                        std::vector<std::string> &insert_keys,
                        std::vector<std::pair<std::string, std::string>> &queries) {
        auto trie = range_filtering::Trie(insert_keys);
        for (float fill_in_coeff = fill_in_min; fill_in_coeff <= fill_in_max; fill_in_coeff += fill_in_step) {
            auto start = std::chrono::system_clock::now();
            auto filter = new range_filters::ReCHaREQ(insert_keys, top_layer_height, fill_in_coeff, bits_per_char);
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end - start;
            auto[fpr, query_time] = bench::calculateFPR(filter, trie, queries);
            std::cout << filter->getMemoryUsage() << "\t" << fpr << "\t" << fill_in_coeff << "\t"
                      << elapsed_seconds.count() << "\t" << query_time << "\t" << trie.getMemoryUsage() << std::endl;
        }
    }

    void runSplashRelative(std::vector<std::string> &insert_keys,
                           std::vector<std::pair<std::string, std::string>> &queries,
                           double cutoff_min, double cutoff_max, double cutoff_interval,
                           double restraint_min, double restraint_max, double restraint_interval) {
        auto trie = range_filtering::Trie(insert_keys);
        for (auto cutoff = cutoff_min; cutoff <= cutoff_max; cutoff += cutoff_interval) {
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

    void runSplashAbsolute(std::vector<std::string> insert_keys,
                           std::vector<std::pair<std::string, std::string>> &queries,
                           double cutoff_min, double cutoff_max, double cutoff_interval,
                           int restraint_min, int restraint_max, int restraint_interval) {
        auto trie = range_filtering::Trie(insert_keys);
        for (auto cutoff = cutoff_min; cutoff <= cutoff_max; cutoff += cutoff_interval) {
            for (auto restraint = restraint_min; restraint < restraint_max; restraint += restraint_interval) {
                auto start = std::chrono::system_clock::now();
                auto splash = new range_filtering_splash::Splash(insert_keys, range_filtering_splash::SplashRestraintType::absolute,
                                                                 restraint, 0, cutoff);
                auto end = std::chrono::system_clock::now();
                std::chrono::duration<double> elapsed_seconds = end - start;
                auto[fpr, query_time] = bench::calculateFPR(splash, trie, queries);
                std::cout << splash->getMemoryUsage() << "\t" << fpr << "\t" << "-" << "\t"
                          << elapsed_seconds.count() << "\t" << query_time << "\t" << trie.getMemoryUsage()
                          << std::endl;
            }
        }
    }
}

#endif //RANGE_FILTERING_RANGE_FILTERING_BENCH_HPP
