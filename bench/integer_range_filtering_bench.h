#ifndef RANGE_FILTERING_INTEGER_RANGE_FILTERING_BENCH_H
#define RANGE_FILTERING_INTEGER_RANGE_FILTERING_BENCH_H

#include "../../integer_range_filters/include/IntegerRangeFilter.hpp"
#include "../../integer_range_filters/include/IntegerSuRF.hpp"
#include "../../rosetta/include/integer_rosetta.hpp"
#include "../../integer_range_filters/include/EmptyRangesList.hpp"
#include "Trie.hpp"
#include <algorithm>
#include "assert.h"
#include "SuRFFacade.h"
#include "../integer_range_filters/include/IntegerFilter.hpp"
#include "../succinct_trie/include/fst.hpp"
#include "../splash/include/splash.hpp"
#include "CHaREQ.hpp"
#include "BloomRangeFilter.hpp"

namespace range_filtering_bench {

    double calculateFPR(range_filtering::IntegerRangeFilter &filter,
                        uint64_t universe_size, uint64_t min_range_size, uint64_t max_range_size,
                        std::vector<uint32_t>& keys) {
        uint32_t negatives = 0;
        uint32_t false_positives = 0;
        uint32_t true_negatives = 0;

        for (size_t i = min_range_size; i <= max_range_size; i++) {
            for (size_t j = 0; j + i < universe_size; j++) {
                bool found = false;
                for (size_t k = j; k <= j + i; k++) {
                    found |= std::binary_search(keys.begin(), keys.end(), k);
                    if (found) break;
                }

                bool foundInFilter = (bool) filter.lookupRange(j, j + i);

                assert(!(found && !foundInFilter));
                negatives += !foundInFilter;
                false_positives += (int) (!found && foundInFilter);
                true_negatives += (int) (!foundInFilter && !found);
            }
        }
        assert(negatives == true_negatives);
        double fp_rate = false_positives / (true_negatives + false_positives + 0.0);
        return fp_rate;
    }

    void runTestsIntegerSuRFBase(std::vector<uint32_t> &keys, uint64_t universe_size,
                                 uint64_t range_start, uint64_t range_end) {
        auto surf = range_filtering::IntegerSuRF(keys);
        auto fpr = calculateFPR(surf, universe_size, range_start, range_end, keys);
        std::cout << surf.getMemoryUsage() << "\t" << fpr << "\t" << "0" << "\t"
                  << 0.0 << "\t" << 0.0 << "\t" << keys.size() * 4 << std::endl;
    }

    void runTestsEmptyRanges(std::vector<uint32_t> &keys, uint64_t universe_size,
                                 uint64_t range_start, uint64_t range_end,
                                 float gamma_min, float gamma_max, float gamma_step) {
        for (float gamma = gamma_min; gamma <= gamma_max; gamma += gamma_step) {
            auto empty_range = range_filtering::EmptyRangeList(keys, gamma);
            auto fpr = calculateFPR(empty_range, universe_size, range_start, range_end, keys);
            std::cout << empty_range.getMemoryUsage() << "\t" << fpr << "\t" << "0" << "\t"
                      << 0.0 << "\t" << 0.0 << "\t" << keys.size() * 4 << std::endl;
        }
    }

    void runTestsIntegerRosetta(std::vector<uint32_t> &keys, uint64_t universe_size,
                             uint64_t range_start, uint64_t range_end,
                             uint64_t min_bfs_size, uint64_t max_bfs_size, uint64_t step_bfs_size) {
        for (size_t size = min_bfs_size; size <= max_bfs_size; size+=step_bfs_size) {
            auto empty_range = range_filtering_rosetta::IntegerRosetta(keys, size);
            auto fpr = calculateFPR(empty_range, universe_size, range_start, range_end, keys);
            std::cout << empty_range.getMemoryUsage() << "\t" << fpr << "\t" << "0" << "\t"
                      << 0.0 << "\t" << 0.0 << "\t" << keys.size() * 4 << std::endl;
        }
    }

    void runTestsIntegerSuRFReal(std::vector<uint32_t> &keys, uint64_t universe_size,
                               uint64_t range_start, uint64_t range_end,
                                 uint32_t start_real_bit, uint32_t end_real_bit) {
        std::vector<std::string> stringKeys;
        uint32_t length = range_filtering::IntegerFilter::parseIntegersToStrings(stringKeys, keys);
        for (size_t i = start_real_bit; i <= end_real_bit; i++) {
            auto surf_real = range_filtering::SuRFFacade(stringKeys, true, false, i, 0);
            auto filter = range_filtering::IntegerFilter(surf_real, length);
            auto fpr = calculateFPR(filter, universe_size, range_start, range_end, keys);
            std::cout << filter.getMemoryUsage() << "\t" << fpr << "\t" << "0" << "\t"
                      << 0.0 << "\t" << 0.0 << "\t" << keys.size() * 4 << std::endl;
        }
    }

    void runTestsIntegerFST(std::vector<uint32_t> &keys, uint64_t universe_size,
                            uint64_t range_start, uint64_t range_end) {
        std::vector<std::string> stringKeys;
        uint32_t length = range_filtering::IntegerFilter::parseIntegersToStrings(stringKeys, keys);
        auto fst = range_filtering::FST(stringKeys);
        auto filter = range_filtering::IntegerFilter(fst, length);
        auto fpr = calculateFPR(filter, universe_size, range_start, range_end, keys);
        std::cout << filter.getMemoryUsage() << "\t" << fpr << "\t" << "0" << "\t"
                  << 0.0 << "\t" << 0.0 << "\t" << keys.size() * 4 << std::endl;
    }

    void runTestsIntegerSplashRelative(std::vector<uint32_t> &keys, uint64_t universe_size,
                                       uint64_t range_start, uint64_t range_end,
                                       double cutoff_min, double cutoff_max, double cutoff_interval,
                                       double restraint_min, double restraint_max, double restraint_interval) {
        std::vector<std::string> stringKeys;
        uint32_t length = range_filtering::IntegerFilter::parseIntegersToStrings(stringKeys, keys);
        for (auto cutoff = cutoff_min; cutoff <= cutoff_max; cutoff += cutoff_interval) {
            for (auto restraint = restraint_min; restraint <= restraint_max; restraint += restraint_interval) {
                auto splash = range_filtering_splash::Splash(stringKeys, range_filtering_splash::SplashRestraintType::relative,
                                                                 0, restraint, cutoff);
                auto filter = range_filtering::IntegerFilter(splash, length);
                auto fpr = calculateFPR(filter, universe_size, range_start, range_end, keys);
                std::cout << filter.getMemoryUsage() << "\t" << fpr << "\t" << "0" << "\t"
                          << 0.0 << "\t" << 0.0 << "\t" << keys.size() * 4 << std::endl;
            }
        }
    }

    void runTestsIntegerSplashAbsolute(std::vector<uint32_t> &keys, uint64_t universe_size,
                                       uint64_t range_start, uint64_t range_end,
                                       double cutoff_min, double cutoff_max, double cutoff_interval,
                                       int restraint_min, int restraint_max, int restraint_interval) {
        std::vector<std::string> stringKeys;
        uint32_t length = range_filtering::IntegerFilter::parseIntegersToStrings(stringKeys, keys);
        for (auto cutoff = cutoff_min; cutoff <= cutoff_max; cutoff += cutoff_interval) {
            for (auto restraint = restraint_min; restraint < restraint_max; restraint += restraint_interval) {
                auto splash = range_filtering_splash::Splash(stringKeys, range_filtering_splash::SplashRestraintType::absolute,restraint, 0, cutoff);
                auto filter = range_filtering::IntegerFilter(splash, length);
                auto fpr = calculateFPR(filter, universe_size, range_start, range_end, keys);
                std::cout << filter.getMemoryUsage() << "\t" << fpr << "\t" << "0" << "\t"
                          << 0.0 << "\t" << 0.0 << "\t" << keys.size() * 4 << std::endl;
            }
        }
    }

    void runTestsIntegerCHaREQ(std::vector<uint32_t> &keys, uint64_t universe_size,
                               uint64_t range_start, uint64_t range_end,
                               float fill_in_min, float fill_in_max, float fill_in_step) {
        std::vector<std::string> stringKeys;
        uint32_t length = range_filtering::IntegerFilter::parseIntegersToStrings(stringKeys, keys);
        for (float fill_in_coeff = fill_in_min; fill_in_coeff <= fill_in_max; fill_in_coeff += fill_in_step) {
            auto chareq = range_filtering::CHaREQ(stringKeys, fill_in_coeff);
            auto filter = range_filtering::IntegerFilter(chareq, length);
            auto fpr = calculateFPR(filter, universe_size, range_start, range_end, keys);
            std::cout << filter.getMemoryUsage() << "\t" << fpr << "\t" << "0" << "\t"
                      << 0.0 << "\t" << 0.0 << "\t" << keys.size() * 4 << std::endl;
        }
    }

    void runTestsRangeBF(std::vector<uint32_t>& keys, uint64_t universe_size,
                         uint64_t range_start, uint64_t range_end,
                         uint64_t min_size, uint64_t max_size, uint64_t step_size) {
        std::vector<std::string> stringKeys;
        uint32_t length = range_filtering::IntegerFilter::parseIntegersToStrings(stringKeys, keys);
        for (uint64_t size = min_size; size <= max_size; size += step_size) {
            auto bfr = range_filtering::BloomRangeFilter(stringKeys, size);
            auto filter = range_filtering::IntegerFilter(bfr, length);
            auto fpr = calculateFPR(filter, universe_size, range_start, range_end, keys);
            std::cout << filter.getMemoryUsage() << "\t" << fpr << "\t" << "0" << "\t"
                      << 0.0 << "\t" << 0.0 << "\t" << keys.size() * 4 << std::endl;
        }
    }
}

#endif //RANGE_FILTERING_INTEGER_RANGE_FILTERING_BENCH_H
