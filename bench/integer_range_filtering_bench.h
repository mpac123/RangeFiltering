#ifndef RANGE_FILTERING_INTEGER_RANGE_FILTERING_BENCH_H
#define RANGE_FILTERING_INTEGER_RANGE_FILTERING_BENCH_H

#include "../../integer_range_filters/include/IntegerRangeFilter.hpp"
#include "../../integer_range_filters/include/IntegerSuRF.hpp"
#include "../../rosetta/include/integer_rosetta.hpp"
#include "../../integer_range_filters/include/EmptyRangesList.hpp"
#include "Trie.hpp"
#include <algorithm>
#include "assert.h"

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

    void runTestsIntegerSuRFReal(std::vector<uint32_t> &keys, uint64_t universe_size,
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
}

#endif //RANGE_FILTERING_INTEGER_RANGE_FILTERING_BENCH_H
