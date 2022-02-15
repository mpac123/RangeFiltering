#include <iostream>
#include <vector>
#include "bench.h"
extern "C" {
#include <qf.h>
};

uint64_t getFingerprint(const std::string& prefix) {
    uint64_t hash[2];
    MurmurHash3_x64_128(prefix.c_str(), prefix.size(), 0, hash);
    return hash[0];
}

int main(int argc, char *argv[]) {
    uint32_t q = 12;
    uint32_t min_r = 4;
    uint32_t max_r = 4;
    uint64_t min_n = 2024;
    uint64_t max_n = 4096;
    uint64_t interval_n = 256;

    std::string data_type = "uniform";
    std::string query_type = "similar";
    std::string input_dir = "/home/mapac/Coding/RangeFiltering/bench/workload-gen/workloads/1mln/";

    if (argc > 6) {
        q = std::stoi(argv[1]);
        min_r = std::stoi(argv[2]);
        max_r = std::stoi(argv[3]);
        min_n = std::stoi(argv[4]);
        max_n = std::stoi(argv[5]);
        interval_n = std::stoi(argv[6]);
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

    std::cout << "q\tr\tn\talpha\tFPR\tQuery time" << std::endl;
    for (size_t n = min_n; n <= max_n; n += interval_n) {
        for (size_t r = min_r; r <= max_r; r++) {
            std::vector<std::string> insert_keys;
            bench::selectKeysToInsert(n, insert_keys, keys);

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
                bool foundInTrie = trie.lookupPrefix(prefix);
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

            //assert(negatives == true_negatives);
            double fp_rate = false_positives / (negatives + false_positives + 0.0);
            double average_query_time = total_query_time / prefixes.size();
            std::cout << q << "\t" << r << "\t" << n << "\t" << double(n / ((2 << (q- 1)) +0.0)) << "\t" << fp_rate << "\t" << average_query_time << std::endl;
        }
    }
}

