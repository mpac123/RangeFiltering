#include <iostream>
#include <vector>
#include "bench.h"
extern "C" {
#include "qf.h"
};

uint64_t getFingerprint(const std::string& prefix) {
    uint64_t hash[2];
    MurmurHash3_x64_128(prefix.c_str(), prefix.size(), 0, hash);
    return hash[0];
}

int main(int argc, char *argv[]) {
    uint32_t min_r = 4;
    uint32_t max_r = 4;
    uint64_t min_n = 26214;
    uint64_t max_n = 996148;
    uint64_t interval_n = 32768;

    std::string data_type = "uniform";
    std::string query_type = "similar";
    std::string input_dir = "/home/mapac/Coding/RangeFiltering/bench/workload-gen/workloads/1mln/";

    if (argc > 5) {
        min_r = std::stoi(argv[1]);
        max_r = std::stoi(argv[2]);
        min_n = std::stoi(argv[3]);
        max_n = std::stoi(argv[4]);
        interval_n = std::stoi(argv[5]);
    }

    std::string input_filename = input_dir + data_type + "_input.txt";
    std::string query_filename = input_dir + data_type + "_queries_" + query_type + ".txt";

    std::vector<std::string> keys;
    bench::loadKeysFromFile(input_filename, keys);

    std::vector<std::string> prefixes;
    bench::loadKeysFromFile(query_filename, prefixes);

    if (keys.empty() || prefixes.empty()) {
        std::cout << "Didn't read files: " << input_filename << " and " << query_filename << std::endl;
        return -1;
    }

    std::cout << "q\tr\tn\talpha\tQF size\tBF size\tQF FPR\tBF FPR\tQuery time" << std::endl;
    for (size_t n = min_n; n <= max_n; n += interval_n) {

        for (size_t r = min_r; r <= max_r; r++) {
            std::vector<std::string> insert_keys;
            bench::selectKeysToInsert(n, insert_keys, keys);

            // Calculate q
            auto q = int(std::ceil(std::log2(insert_keys.size())));
            auto alpha = insert_keys.size() / (double(1 << q));
            if (alpha >= 0.8) q = q + 1;

            struct quotient_filter qf;
            qf_init(&qf, q, r);
            for (const auto &key: insert_keys) {
                if (!qf_insert(&qf, getFingerprint(key))) {
                    std::cout << "Failed to insert" << std::endl;
                    return -1;
                };
            }

            auto trie = range_filtering::Trie(insert_keys);

            uint32_t negatives = 0;
            uint32_t false_positives = 0;
            uint32_t true_negatives = 0;
            double total_query_time = 0.;
            for (const auto &prefix: prefixes) {
                bool foundInTrie = trie.lookupKey(prefix);
                double total_time = 0.0;
                bool foundInFilter;
                for (size_t i = 0; i < 10; i++) {
                    auto start = std::chrono::system_clock::now();
                    foundInFilter = qf_may_contain(&qf, getFingerprint(prefix));
                    auto end = std::chrono::system_clock::now();
                    std::chrono::duration<double> elapsed_seconds = end - start;
                    total_time += elapsed_seconds.count();
                }
                negatives += (int) !foundInFilter;
                false_positives += (int) (!foundInTrie && foundInFilter);
                true_negatives += (int) (!foundInFilter && !foundInTrie);
                total_query_time += total_time / 10;
            }

            assert(negatives == true_negatives);
            double qf_fp_rate = false_positives / (negatives + false_positives + 0.0);
            double average_query_time = total_query_time / prefixes.size();

            auto size = qf_table_size(q, r);
            double eps = 0.09;
            auto size_bf = int(n * std::log2(1/eps) / std::log(2));

            // Create BF with the same size
            auto bf = new bloom_filter::BloomFilter(insert_keys, size_bf);
            negatives = 0;
            false_positives = 0;
            true_negatives = 0;
            for (const auto &prefix: prefixes) {
                bool foundInTrie = trie.lookupKey(prefix);
                bool foundInFilter;
                foundInFilter = bf->lookupKey(prefix);
                negatives += (int) !foundInFilter;
                false_positives += (int) (!foundInTrie && foundInFilter);
                true_negatives += (int) (!foundInFilter && !foundInTrie);
            }
            assert(negatives == true_negatives);
            double bf_fp_rate = false_positives / (negatives + false_positives + 0.0);

            std::cout << q << "\t" << r << "\t" << n << "\t" << double(n / ((2 << (q- 1)) +0.0)) << "\t"
            << size << "\t" << bf->getMemoryUsage() << "\t" << qf_fp_rate << "\t" << bf_fp_rate << "\t"
            << average_query_time << std::endl;
        }
    }
}

