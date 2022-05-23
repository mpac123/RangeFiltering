#ifndef RANGE_FILTERING_BLOOMED_RANGE_SPLASH_HPP
#define RANGE_FILTERING_BLOOMED_RANGE_SPLASH_HPP

#include <string>
#include <vector>
#include <bitset>

#include "config.hpp"
#include "louds_dense.hpp"
#include "louds_sparse.hpp"
#include "bloomed_range_splash_builder.hpp"
#include "RangeFilter.h"
#include "padded_prefix_bloom_filter.hpp"

namespace range_filtering_bloomed_range_splash {
    class BloomedRangeSplash : public range_filtering::RangeFilter {
    public:
        class Iter {
        public:
            Iter() {};
            Iter(const BloomedRangeSplash* fst) {
                dense_iter_ = LoudsDense::Iter(fst->louds_dense_);
                sparse_iter_ = LoudsSparse::Iter(fst->louds_sparse_);
                could_be_fp_ = false;
            }

            void clear();
            bool isValid() const;
            bool getFpFlag() const;
            int compare(const std::string& key) const;
            std::string getKey() const;
            int getSuffix(word_t* suffix) const;
            std::string getKeyWithSuffix(unsigned* bitlen) const;

            // Returns true if the status of the iterator after the operation is valid
            bool operator ++(int);
            bool operator --(int);

        private:
            void passToSparse();
            bool incrementDenseIter();
            bool incrementSparseIter();
            bool decrementDenseIter();
            bool decrementSparseIter();

        private:
            // true implies that dense_iter_ is valid
            LoudsDense::Iter dense_iter_;
            LoudsSparse::Iter sparse_iter_;
            bool could_be_fp_;

            friend class BloomedRangeSplash;
        };

    public:
        BloomedRangeSplash() {};

        //------------------------------------------------------------------
        // Input keys must be SORTED
        //------------------------------------------------------------------
        BloomedRangeSplash(const std::vector<std::string>& keys, uint64_t fst_height,
                      uint32_t bf_size) {
            create(keys, kIncludeDense, kSparseDenseRatio, fst_height, bf_size);
        }

        BloomedRangeSplash(const std::vector<std::string>& keys,
                      const bool include_dense, const uint32_t sparse_dense_ratio, uint64_t fst_height,
                      uint32_t bf_size) {
            create(keys, include_dense, sparse_dense_ratio, fst_height, bf_size);
        }

        ~BloomedRangeSplash() {}

        void create(const std::vector<std::string>& keys,
                    const bool include_dense, const uint32_t sparse_dense_ratio, uint64_t fst_height,
                    uint32_t bf_size);

        bool lookupKey(const std::string& key) const;
        bool lookupPrefix(const std::string& prefix);
        // This function searches in a conservative way: if inclusive is true
        // and the stored key prefix matches key, iter stays at this key prefix.
        BloomedRangeSplash::Iter moveToKeyGreaterThan(const std::string& key, const bool inclusive) const;
        BloomedRangeSplash::Iter moveToKeyLessThan(const std::string& key, const bool inclusive) const;
        BloomedRangeSplash::Iter moveToFirst() const;
        BloomedRangeSplash::Iter moveToLast() const;
        bool lookupRange(const std::string& left_key, const std::string& right_key) override;
        bool lookupRange(const std::string& left_key, const bool left_inclusive,
                         const std::string& right_key, const bool right_inclusive);
        // Accurate except at the boundaries --> undercount by at most 2
        uint64_t approxCount(const std::string& left_key, const std::string& right_key);
        uint64_t approxCount(const BloomedRangeSplash::Iter* iter, const BloomedRangeSplash::Iter* iter2);

        uint64_t serializedSize() const;
        uint64_t getMemoryUsage() const override;
        level_t getHeight() const;
        level_t getSparseStartLevel() const;

        void generateKeys(const std::vector<std::string>& keys,
                          std::vector<std::string>& fst_keys);

        char* serialize() const {
            uint64_t size = serializedSize();
            char* data = new char[size];
            char* cur_data = data;
            louds_dense_->serialize(cur_data);
            louds_sparse_->serialize(cur_data);
            assert(cur_data - data == (int64_t)size);
            return data;
        }

        static BloomedRangeSplash* deSerialize(char* src) {
            BloomedRangeSplash* surf = new BloomedRangeSplash();
            surf->louds_dense_ = LoudsDense::deSerialize(src);
            surf->louds_sparse_ = LoudsSparse::deSerialize(src);
            surf->iter_ = BloomedRangeSplash::Iter(surf);
            return surf;
        }

        void destroy() {
            louds_dense_->destroy();
            louds_sparse_->destroy();
        }

    private:
        LoudsDense* louds_dense_;
        LoudsSparse* louds_sparse_;
        BloomedSplashRangeBuilder* builder_;
        BloomedRangeSplash::Iter iter_;
        BloomedRangeSplash::Iter iter2_;

        uint64_t fst_height_;
        uint32_t bf_size_;

        PaddedPrefixBloomFilter bloomFilter_;
        std::bitset<256> charUsageMask_;

        bool lookupSuRFOrBF(const std::string& key);
        bool lookupRangeOneLevel(const std::string &leftKey, const std::string &rightKey);
        bool lookupRangeRecursively(const std::string &leftKey, const std::string& rightKey,
                               char firstCharInAlphabet, char lastLetterInAlphabet);
        std::string getNextUp(std::string word, char lastLetterInAlphabet);
        std::string getNextDown(std::string word, char firstLetterInAlphabet);
    };

    void BloomedRangeSplash::create(const std::vector<std::string>& keys,
                               const bool include_dense, const uint32_t sparse_dense_ratio, uint64_t fst_height,
                               uint32_t bf_size) {
        for (const auto& key : keys) {
            for (const auto& letter : key) {
                charUsageMask_[int(letter)] = true;
            }
        }

        fst_height_ = fst_height;
        bf_size_ = bf_size;
        builder_ = new BloomedSplashRangeBuilder(include_dense, sparse_dense_ratio);

        auto fst_keys = std::vector<std::string>();
        generateKeys(keys, fst_keys);

        builder_->build(fst_keys);
        louds_dense_ = new LoudsDense(builder_);
        louds_sparse_ = new LoudsSparse(builder_);
        iter_ = BloomedRangeSplash::Iter(this);
        delete builder_;

        bloomFilter_ = PaddedPrefixBloomFilter(keys, bf_size_, fst_height);
    }

    void BloomedRangeSplash::generateKeys(const std::vector<std::string>& keys,
                                     std::vector<std::string>& fst_keys) {
        for (const auto& key : keys) {
            if (key.length() <= fst_height_) {
                fst_keys.push_back(key);
            } else {
                fst_keys.push_back(key.substr(0, fst_height_));
            }
        }
    }

    bool BloomedRangeSplash::lookupKey(const std::string& key) const {
        position_t connect_node_num = 0;
        if (!louds_dense_->lookupKey(key, connect_node_num))
            return false;
        else if (connect_node_num != 0)
            return louds_sparse_->lookupKey(key, connect_node_num);
        return true;
    }

    BloomedRangeSplash::Iter BloomedRangeSplash::moveToKeyGreaterThan(const std::string& key, const bool inclusive) const {
        BloomedRangeSplash::Iter iter(this);
        iter.could_be_fp_ = louds_dense_->moveToKeyGreaterThan(key, inclusive, iter.dense_iter_);

        if (!iter.dense_iter_.isValid())
            return iter;
        if (iter.dense_iter_.isComplete())
            return iter;

        if (!iter.dense_iter_.isSearchComplete()) {
            iter.passToSparse();
            iter.could_be_fp_ = louds_sparse_->moveToKeyGreaterThan(key, inclusive, iter.sparse_iter_);
            if (!iter.sparse_iter_.isValid())
                iter.incrementDenseIter();
            return iter;
        } else if (!iter.dense_iter_.isMoveLeftComplete()) {
            iter.passToSparse();
            iter.sparse_iter_.moveToLeftMostKey();
            return iter;
        }

        assert(false); // shouldn't reach here
        return iter;
    }

    BloomedRangeSplash::Iter BloomedRangeSplash::moveToKeyLessThan(const std::string& key, const bool inclusive) const {
        BloomedRangeSplash::Iter iter = moveToKeyGreaterThan(key, false);
        if (!iter.isValid()) {
            iter = moveToLast();
            return iter;
        }
        if (!iter.getFpFlag()) {
            iter--;
            if (lookupKey(key))
                iter--;
        }
        return iter;
    }

    BloomedRangeSplash::Iter BloomedRangeSplash::moveToFirst() const {
        BloomedRangeSplash::Iter iter(this);
        if (louds_dense_->getHeight() > 0) {
            iter.dense_iter_.setToFirstLabelInRoot();
            iter.dense_iter_.moveToLeftMostKey();
            if (iter.dense_iter_.isMoveLeftComplete())
                return iter;
            iter.passToSparse();
            iter.sparse_iter_.moveToLeftMostKey();
        } else {
            iter.sparse_iter_.setToFirstLabelInRoot();
            iter.sparse_iter_.moveToLeftMostKey();
        }
        return iter;
    }

    BloomedRangeSplash::Iter BloomedRangeSplash::moveToLast() const {
        BloomedRangeSplash::Iter iter(this);
        if (louds_dense_->getHeight() > 0) {
            iter.dense_iter_.setToLastLabelInRoot();
            iter.dense_iter_.moveToRightMostKey();
            if (iter.dense_iter_.isMoveRightComplete())
                return iter;
            iter.passToSparse();
            iter.sparse_iter_.moveToRightMostKey();
        } else {
            iter.sparse_iter_.setToLastLabelInRoot();
            iter.sparse_iter_.moveToRightMostKey();
        }
        return iter;
    }

    bool BloomedRangeSplash::lookupRange(const std::string &leftKey, const std::string &rightKey) {
        if (leftKey > rightKey) return false;

        if (leftKey.size() <= fst_height_ && rightKey.size() <= fst_height_) {
            return lookupRange(leftKey, true, rightKey, true);
        }

        std::string common_prefix;
        for (size_t i = 0; i < std::min(leftKey.length(), rightKey.length()); i++) {
            if (leftKey[i] != rightKey[i]) break;
            common_prefix += leftKey[i];
        }

        // Substring of common_prefix of length fst_height should exist in FST
        if (!lookupPrefix(common_prefix.substr(0, std::min(fst_height_, common_prefix.length())))) return false;

        // The prefixes of length >fst_height should exist in BF
        for (size_t i = fst_height_ + 1; i < common_prefix.length() + 1; i++) {
            if (!bloomFilter_.lookupKey(common_prefix.substr(0, i))) return false;
        }

        char first_letter_in_alphabet = 127;
        char last_letter_in_alphabet = 0;
        for (size_t i = 0; i < 128; i++) {
            if (charUsageMask_[i]) {
                first_letter_in_alphabet = i;
                break;
            }
        }
        for (size_t i = 127; i >= 0; i--) {
            if (charUsageMask_[i]) {
                last_letter_in_alphabet = i;
                break;
            }
        }

        // Look up the middle range
        if (leftKey != common_prefix) {
            auto newLeftKey = leftKey.substr(0, common_prefix.length() + 1);
            auto newRightKey = rightKey.substr(0, common_prefix.length() + 1);

            if (lookupRangeOneLevel(newLeftKey, newRightKey)) return true;

            // Look up the left subtrees
            for (size_t i = common_prefix.length() + 1; i <leftKey.length(); i++) {
                if (!lookupSuRFOrBF(newLeftKey)) break;
                newRightKey = newLeftKey + last_letter_in_alphabet;
                newLeftKey = leftKey.substr(0, i + 1);
                if (lookupRangeOneLevel(newLeftKey, newRightKey)) return true;
            }
        } else {
            if (lookupSuRFOrBF(common_prefix)) return true;
        }

        // Lookup the right subtrees
        auto newLeftKey = common_prefix;
        auto newRightKey = common_prefix;
        for (size_t i = common_prefix.length(); i < rightKey.length(); i++) {
            if (!lookupSuRFOrBF(newRightKey)) break;
            newLeftKey += first_letter_in_alphabet;
            newRightKey = rightKey.substr(0, i + 1);

            char nextLetter = newLeftKey.at(newLeftKey.length() - 1) + 1;
            auto shiftedNewLeftKey = newLeftKey.substr(0, newLeftKey.length() - 1) + nextLetter;
            if (lookupRangeOneLevel(shiftedNewLeftKey, newRightKey)) return true;
        }

        return false;
    }

    bool BloomedRangeSplash::lookupSuRFOrBF(const std::string& key) {
        if (key.length() <= fst_height_) {
            return lookupPrefix(key);
        } else {
            return bloomFilter_.lookupKey(key);
        }
    }

    bool BloomedRangeSplash::lookupRangeRecursively(const std::string &leftKey, const std::string &rightKey,
                                                    char firstCharInAlphabet, char lastLetterInAlphabet) {
        std::string common_prefix = "";
        for (size_t i = 0; i < std::min(leftKey.length(), rightKey.length()); i++) {
            if (leftKey[i] != rightKey[i]) break;
            common_prefix += leftKey[i];
        }

        if (leftKey.length() == rightKey.length() &&
            (common_prefix.length() == leftKey.length() - 1 || common_prefix.length() == leftKey.length())) {
            return lookupRangeOneLevel(leftKey, rightKey);
        }

        if (leftKey.length() - common_prefix.length() > 1) {
            std::string newRightKey = leftKey.substr(0, leftKey.length() - 1);
            newRightKey += lastLetterInAlphabet;
            if (lookupRangeOneLevel(leftKey, newRightKey)) return true;
            std::string newLeftKey = getNextUp(newRightKey, lastLetterInAlphabet);
            return lookupRangeRecursively(newLeftKey, rightKey, firstCharInAlphabet, lastLetterInAlphabet);
        }

        if (leftKey.length() - common_prefix.length() <= 1) {
            std::string newRightKey = leftKey.substr(0, leftKey.length() - 1);
            newRightKey += rightKey.at(common_prefix.length());
            if (lookupRangeOneLevel(leftKey, newRightKey)) return true;
            std::string newLeftKey = getNextDown(newRightKey, firstCharInAlphabet);
            return lookupRangeRecursively(newLeftKey, rightKey, firstCharInAlphabet, lastLetterInAlphabet);
        }
        return true;
    }

    std::string BloomedRangeSplash::getNextUp(std::string word, char lastLetterInAlphabet) {
        while (word.at(word.length() - 1) >= lastLetterInAlphabet) {
            word = word.substr(0, word.length() - 1);
            if (word.length() == 0) return word;
        }
        auto current = word.at(word.length() - 1);
        while (!charUsageMask_[current]) {
            current++;
        }
        auto next = word.substr(0, word.length() - 1);
        next += current;
        return next;
    }

    std::string BloomedRangeSplash::getNextDown(std::string word, char firstLetterInAlphabet) {
        return word + firstLetterInAlphabet;
    }

    bool BloomedRangeSplash::lookupRangeOneLevel(const std::string &leftKey, const std::string &rightKey) {
        assert(leftKey.length() == rightKey.length());
        for (uint8_t letter = leftKey.at(leftKey.length() - 1); letter <= rightKey.at(rightKey.length() - 1); letter += 1) {
            if (charUsageMask_[letter] == false) continue;
            auto query = leftKey.substr(0, leftKey.length() - 1);
            query += letter;
            if (query.length() <= fst_height_) {
                if (lookupPrefix(query)) return true;
            } else {
                if (bloomFilter_.lookupKey(query)) return true;
            }
        }
        return false;
    }

    bool BloomedRangeSplash::lookupRange(const std::string& left_key, const bool left_inclusive,
                                    const std::string& right_key, const bool right_inclusive) {
        iter_.clear();
        louds_dense_->moveToKeyGreaterThan(left_key, left_inclusive, iter_.dense_iter_);
        if (!iter_.dense_iter_.isValid()) return false;
        if (!iter_.dense_iter_.isComplete()) {
            if (!iter_.dense_iter_.isSearchComplete()) {
                iter_.passToSparse();
                louds_sparse_->moveToKeyGreaterThan(left_key, left_inclusive, iter_.sparse_iter_);
                if (!iter_.sparse_iter_.isValid()) {
                    iter_.incrementDenseIter();
                }
            } else if (!iter_.dense_iter_.isMoveLeftComplete()) {
                iter_.passToSparse();
                iter_.sparse_iter_.moveToLeftMostKey();
            }
        }
        if (!iter_.isValid()) return false;
        int compare = iter_.compare(right_key);
        if (compare == kCouldBePositive)
            return true;
        if (right_inclusive)
            return (compare <= 0);
        else
            return (compare < 0);
    }

    uint64_t BloomedRangeSplash::approxCount(const BloomedRangeSplash::Iter* iter, const BloomedRangeSplash::Iter* iter2) {
        if (!iter->isValid() || !iter2->isValid()) return 0;
        position_t out_node_num_left = 0, out_node_num_right = 0;
        uint64_t count = louds_dense_->approxCount(&(iter->dense_iter_),
                                                   &(iter2->dense_iter_),
                                                   out_node_num_left,
                                                   out_node_num_right);
        count += louds_sparse_->approxCount(&(iter->sparse_iter_),
                                            &(iter2->sparse_iter_),
                                            out_node_num_left,
                                            out_node_num_right);
        return count;
    }

    uint64_t BloomedRangeSplash::approxCount(const std::string& left_key,
                                        const std::string& right_key) {
        iter_.clear(); iter2_.clear();
        iter_ = moveToKeyGreaterThan(left_key, true);
        if (!iter_.isValid()) return 0;
        iter2_ = moveToKeyGreaterThan(right_key, true);
        if (!iter2_.isValid())
            iter2_ = moveToLast();

        return approxCount(&iter_, &iter2_);
    }

    uint64_t BloomedRangeSplash::serializedSize() const {
        return (louds_dense_->serializedSize()
                + louds_sparse_->serializedSize());
    }

    uint64_t BloomedRangeSplash::getMemoryUsage() const {
        return (sizeof(BloomedRangeSplash) + louds_dense_->getMemoryUsage() + louds_sparse_->getMemoryUsage() + bloomFilter_.getMemoryUsage());
    }

    level_t BloomedRangeSplash::getHeight() const {
        return louds_sparse_->getHeight();
    }

    level_t BloomedRangeSplash::getSparseStartLevel() const {
        return louds_sparse_->getStartLevel();
    }

    bool BloomedRangeSplash::lookupPrefix(const std::string &prefix) {
        auto fst_prefix = prefix.substr(0, fst_height_);
        bool fst_found_ = false;
        if (louds_dense_->getHeight() == 0) {
            fst_found_ = louds_sparse_->lookupPrefix(fst_prefix, 0);
        } else {
            position_t connect_node_num = 0;
            if (!louds_dense_->lookupPrefix(fst_prefix, connect_node_num))
                fst_found_ = false;
            else if (connect_node_num != 0)
                fst_found_ = louds_sparse_->lookupPrefix(fst_prefix, connect_node_num);
            else fst_found_ = true;
        }

        if (!fst_found_) return false;
        for (size_t i = 0; i < 3; i++) {
            auto bf_prefix = prefix.substr(0, prefix.length() - i);
            if (bf_prefix.length() > fst_height_) {
                bool bf_found = bloomFilter_.lookupKey(bf_prefix);
                if (!bf_found) return false;
                // Otherwise, it can be a false positive, so we start doubting
            }
        }
        return true;
    }

//============================================================================

    void BloomedRangeSplash::Iter::clear() {
        dense_iter_.clear();
        sparse_iter_.clear();
    }

    bool BloomedRangeSplash::Iter::getFpFlag() const {
        return could_be_fp_;
    }

    bool BloomedRangeSplash::Iter::isValid() const {
        return dense_iter_.isValid()
               && (dense_iter_.isComplete() || sparse_iter_.isValid());
    }

    int BloomedRangeSplash::Iter::compare(const std::string& key) const {
        assert(isValid());
        int dense_compare = dense_iter_.compare(key);
        if (dense_iter_.isComplete() || dense_compare != 0)
            return dense_compare;
        return sparse_iter_.compare(key);
    }

    std::string BloomedRangeSplash::Iter::getKey() const {
        if (!isValid())
            return std::string();
        if (dense_iter_.isComplete())
            return dense_iter_.getKey();
        return dense_iter_.getKey() + sparse_iter_.getKey();
    }

    int BloomedRangeSplash::Iter::getSuffix(word_t* suffix) const {
        if (!isValid())
            return 0;
        if (dense_iter_.isComplete())
            return dense_iter_.getSuffix(suffix);
        return sparse_iter_.getSuffix(suffix);
    }

    std::string BloomedRangeSplash::Iter::getKeyWithSuffix(unsigned* bitlen) const {
        *bitlen = 0;
        if (!isValid())
            return std::string();
        if (dense_iter_.isComplete())
            return dense_iter_.getKeyWithSuffix(bitlen);
        return dense_iter_.getKeyWithSuffix(bitlen) + sparse_iter_.getKeyWithSuffix(bitlen);
    }

    void BloomedRangeSplash::Iter::passToSparse() {
        sparse_iter_.setStartNodeNum(dense_iter_.getSendOutNodeNum());
    }

    bool BloomedRangeSplash::Iter::incrementDenseIter() {
        if (!dense_iter_.isValid())
            return false;

        dense_iter_++;
        if (!dense_iter_.isValid())
            return false;
        if (dense_iter_.isMoveLeftComplete())
            return true;

        passToSparse();
        sparse_iter_.moveToLeftMostKey();
        return true;
    }

    bool BloomedRangeSplash::Iter::incrementSparseIter() {
        if (!sparse_iter_.isValid())
            return false;
        sparse_iter_++;
        return sparse_iter_.isValid();
    }

    bool BloomedRangeSplash::Iter::operator ++(int) {
        if (!isValid())
            return false;
        if (incrementSparseIter())
            return true;
        return incrementDenseIter();
    }

    bool BloomedRangeSplash::Iter::decrementDenseIter() {
        if (!dense_iter_.isValid())
            return false;

        dense_iter_--;
        if (!dense_iter_.isValid())
            return false;
        if (dense_iter_.isMoveRightComplete())
            return true;

        passToSparse();
        sparse_iter_.moveToRightMostKey();
        return true;
    }

    bool BloomedRangeSplash::Iter::decrementSparseIter() {
        if (!sparse_iter_.isValid())
            return false;
        sparse_iter_--;
        return sparse_iter_.isValid();
    }

    bool BloomedRangeSplash::Iter::operator --(int) {
        if (!isValid())
            return false;
        if (decrementSparseIter())
            return true;
        return decrementDenseIter();
    }
}
#endif //RANGE_FILTERING_BLOOMED_RANGE_SPLASH_HPP
