#include "QuotientFilter.h"

quotient_filter::QuotientFilter::QuotientFilter(std::vector<std::string> &keys, uint32_t q, uint32_t r) {
    failed_ = (q == 0 || r == 0);
    if (failed_) return;
    q_ = q;
    p_ = q + r;
    n_ = keys.size();

    arr_size_ = std::pow(2, q) + 3;

    remainder_table_ = std::vector<uint32_t>(arr_size_);
    is_occupied_ = std::vector<bool>(arr_size_);
    is_continuation_ = std::vector<bool>(arr_size_);
    is_shifted_ = std::vector<bool>(arr_size_);

    for (const auto& key : keys) {
        insertKey(key);
    }
}

void quotient_filter::QuotientFilter::insertKey(const std::string &key) {
    uint32_t fingerprint;
    MurmurHash3_x86_32(key.c_str(), key.size(), 0, (void*) &fingerprint);
    fingerprint %= (int)(std::pow(2, p_));
    uint32_t quotient = fingerprint / (std::pow(2, p_ - q_) + 0.0);
    uint32_t remainder = fingerprint % (int)(std::pow(2, p_ - q_));

    // Slot is empty, we can just insert the key
     if (!is_occupied_[quotient] && !is_continuation_[quotient] && !is_shifted_[quotient]) {
        remainder_table_[quotient] = remainder;
        is_occupied_[quotient] = true;
        return;
    }

    // locate the start of the cluster and count number of runs within cluster
    uint32_t pos = quotient;
    uint32_t runs_count = 0;
    while (is_shifted_[pos]) {
        pos--;
        if (is_occupied_[pos]) {
            runs_count++;
        }
    }

    while (runs_count > 0) {
        pos++;
        if (!is_continuation_[pos]) {
            runs_count--;
        }
    };

    // pos now indicates the quotient's run
    do {
        // If the element has been already inserted, terminate
        if (remainder_table_[pos] == remainder) return;
        pos++;
        if (pos >= remainder_table_.size()) {
            // Insert fails due to lack of space in the hash table, extend it
            for (size_t i = 0; i < 4; i++) {
                arr_size_++;
                remainder_table_.push_back(0);
                is_occupied_.push_back(0);
                is_continuation_.push_back(0);
                is_shifted_.push_back(0);
            }
        }
    } while (is_continuation_[pos]);

    // Slot is empty, we can just insert the key
    if (!is_occupied_[pos] && !is_continuation_[pos] && !is_shifted_[pos]) {
        remainder_table_[pos] = remainder;
        is_continuation_[pos] = true;
        is_shifted_[pos] = true;
        is_occupied_[quotient] = true;
        return;
    }
    // Otherwise, shift the consecutive elements at and after pos to the right
    uint32_t empty_pos = pos;
    while (is_occupied_[empty_pos] || is_continuation_[empty_pos] || is_shifted_[empty_pos]) {
        empty_pos++;
        if (empty_pos >= remainder_table_.size()) {
            // Insert fails due to lack of space in the hash table, extend it
            for (size_t i = 0; i < 4; i++) {
                arr_size_++;
                remainder_table_.push_back(0);
                is_occupied_.push_back(0);
                is_continuation_.push_back(0);
                is_shifted_.push_back(0);
            }
        }
    }

    while (empty_pos > pos) {
        remainder_table_[empty_pos] = remainder_table_[empty_pos - 1];
        is_shifted_[empty_pos] = true;
        is_continuation_[empty_pos] = is_continuation_[empty_pos-1];
        empty_pos--;
    }

    // Finally, we can insert the new element
    remainder_table_[pos] = remainder;
    is_continuation_[pos] = true;
    is_shifted_[pos] = true;
    is_occupied_[quotient] = true;
}

bool quotient_filter::QuotientFilter::lookupKey(const std::string &key) {
    uint32_t fingerprint;
    MurmurHash3_x86_32(key.c_str(), key.size(), 0, (void*) &fingerprint);
    fingerprint %= (int)(std::pow(2, p_));
    uint32_t quotient = fingerprint / (std::pow(2, p_ - q_) + 0.0);
    uint32_t remainder = fingerprint % (int)(std::pow(2, p_ - q_));

    // Slot is empty
    if (!is_occupied_[quotient]) {
        return false;
    }

    // locate the start of the cluster and count number of runs within cluster
    uint32_t pos = quotient;
    int32_t runs_count = 0;
    while (is_shifted_[pos]) {
        pos--;
        if (is_occupied_[pos]) {
            runs_count++;
        }
    }

    while (runs_count > 0) {
        pos++;
        if (!is_continuation_[pos]) {
            runs_count--;
        }
    };

    // pos now indicates the quotient's run
    do {
        if (remainder_table_[pos] == remainder) return true;
        pos++;
    } while (is_continuation_[pos]);
    return (remainder_table_[pos] == remainder);
}

uint64_t quotient_filter::QuotientFilter::getMemoryUsage() {
    return std::ceil(((uint64_t) std::pow(2, p_-q_)) * remainder_table_.size() / 8.0)
    + 3 * std::ceil(remainder_table_.size() / 8.0);
}

double quotient_filter::QuotientFilter::calculateFalsePositiveProbability() {
    // According to Bender et al.
    return 1 - std::pow(1 - std::pow(2, -p_), n_);
}