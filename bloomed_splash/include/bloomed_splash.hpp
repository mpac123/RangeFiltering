#ifndef RANGE_FILTERING_BLOOMED_SPLASH_BloomedSplash_HPP
#define RANGE_FILTERING_BLOOMED_SPLASH_BloomedSplash_HPP

#include <string>
#include <vector>

#include "config.hpp"
#include "louds_dense.hpp"
#include "louds_sparse.hpp"
#include "bloomed_splash_builder.hpp"
#include "PrefixFilter.h"
#include "BloomFiltersEnsemble.h"

namespace range_filtering_bloomed_splash {
    class BloomedSplash : public range_filtering::PrefixFilter {
    public:
        class Iter {
        public:
            Iter() {};
            Iter(const BloomedSplash* fst) {
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

            friend class BloomedSplash;
        };

    public:
        BloomedSplash() {};

        //------------------------------------------------------------------
        // Input keys must be SORTED
        //------------------------------------------------------------------
        BloomedSplash(const std::vector<std::string>& keys, uint64_t fst_height,
                      uint32_t bf_size, double bf_last_level_penalty) {
            create(keys, kIncludeDense, kSparseDenseRatio, fst_height, bf_size, bf_last_level_penalty);
        }

        BloomedSplash(const std::vector<std::string>& keys,
             const bool include_dense, const uint32_t sparse_dense_ratio, uint64_t fst_height,
                      uint32_t bf_size, double bf_last_level_penalty) {
            create(keys, include_dense, sparse_dense_ratio, fst_height, bf_size, bf_last_level_penalty);
        }

        ~BloomedSplash() {}

        void create(const std::vector<std::string>& keys,
                    const bool include_dense, const uint32_t sparse_dense_ratio, uint64_t fst_height,
                    uint32_t bf_size, double bf_last_level_penalty);

        bool lookupKey(const std::string& key) const;
        bool lookupPrefix(const std::string& prefix) override;
        // This function searches in a conservative way: if inclusive is true
        // and the stored key prefix matches key, iter stays at this key prefix.
        BloomedSplash::Iter moveToKeyGreaterThan(const std::string& key, const bool inclusive) const;
        BloomedSplash::Iter moveToKeyLessThan(const std::string& key, const bool inclusive) const;
        BloomedSplash::Iter moveToFirst() const;
        BloomedSplash::Iter moveToLast() const;
        bool lookupRange(const std::string& left_key, const bool left_inclusive,
                         const std::string& right_key, const bool right_inclusive);
        // Accurate except at the boundaries --> undercount by at most 2
        uint64_t approxCount(const std::string& left_key, const std::string& right_key);
        uint64_t approxCount(const BloomedSplash::Iter* iter, const BloomedSplash::Iter* iter2);

        uint64_t serializedSize() const;
        uint64_t getMemoryUsage() const override;
        level_t getHeight() const;
        level_t getSparseStartLevel() const;

        void generateKeys(const std::vector<std::string>& keys,
                     std::vector<std::string>& fst_keys,
                     std::vector<std::vector<std::string>>& bf_keys);

        char* serialize() const {
            uint64_t size = serializedSize();
            char* data = new char[size];
            char* cur_data = data;
            louds_dense_->serialize(cur_data);
            louds_sparse_->serialize(cur_data);
            assert(cur_data - data == (int64_t)size);
            return data;
        }

        static BloomedSplash* deSerialize(char* src) {
            BloomedSplash* surf = new BloomedSplash();
            surf->louds_dense_ = LoudsDense::deSerialize(src);
            surf->louds_sparse_ = LoudsSparse::deSerialize(src);
            surf->iter_ = BloomedSplash::Iter(surf);
            return surf;
        }

        void destroy() {
            louds_dense_->destroy();
            louds_sparse_->destroy();
        }

    private:
        LoudsDense* louds_dense_;
        LoudsSparse* louds_sparse_;
        BloomedSplashBuilder* builder_;
        BloomedSplash::Iter iter_;
        BloomedSplash::Iter iter2_;

        uint64_t fst_height_;
        uint32_t bf_size_;
        double bf_last_level_penalty_;

        range_filtering::BloomFiltersEnsemble bloomFilters_;
    };

    void BloomedSplash::create(const std::vector<std::string>& keys,
                      const bool include_dense, const uint32_t sparse_dense_ratio, uint64_t fst_height,
                               uint32_t bf_size, double bf_last_level_penalty) {
        fst_height_ = fst_height;
        bf_size_ = bf_size;
        bf_last_level_penalty_ = bf_last_level_penalty;
        builder_ = new BloomedSplashBuilder(include_dense, sparse_dense_ratio);

        auto fst_keys = std::vector<std::string>();
        auto bf_keys = std::vector<std::vector<std::string>>();
        generateKeys(keys, fst_keys, bf_keys);

        builder_->build(fst_keys);
        louds_dense_ = new LoudsDense(builder_);
        louds_sparse_ = new LoudsSparse(builder_);
        iter_ = BloomedSplash::Iter(this);
        delete builder_;

        bloomFilters_ = range_filtering::BloomFiltersEnsemble(bf_keys, bf_size_, bf_last_level_penalty_);
    }

    void BloomedSplash::generateKeys(const std::vector<std::string>& keys,
                                     std::vector<std::string>& fst_keys,
                                     std::vector<std::vector<std::string>>& bf_keys) {
        for (const auto& key : keys) {
            if (key.length() <= fst_height_) {
                fst_keys.push_back(key);
            } else {
                fst_keys.push_back(key.substr(0, fst_height_));
                for (size_t i = fst_height_ + 1; i <= key.length(); i++) {
                    while (bf_keys.size() < i) {
                        bf_keys.push_back(std::vector<std::string>());
                    }
                    bf_keys[i - 1].push_back(key.substr(0, i));
                }
            }
        }
    }

    bool BloomedSplash::lookupKey(const std::string& key) const {
        position_t connect_node_num = 0;
        if (!louds_dense_->lookupKey(key, connect_node_num))
            return false;
        else if (connect_node_num != 0)
            return louds_sparse_->lookupKey(key, connect_node_num);
        return true;
    }

    BloomedSplash::Iter BloomedSplash::moveToKeyGreaterThan(const std::string& key, const bool inclusive) const {
        BloomedSplash::Iter iter(this);
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

    BloomedSplash::Iter BloomedSplash::moveToKeyLessThan(const std::string& key, const bool inclusive) const {
        BloomedSplash::Iter iter = moveToKeyGreaterThan(key, false);
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

    BloomedSplash::Iter BloomedSplash::moveToFirst() const {
        BloomedSplash::Iter iter(this);
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

    BloomedSplash::Iter BloomedSplash::moveToLast() const {
        BloomedSplash::Iter iter(this);
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

    bool BloomedSplash::lookupRange(const std::string& left_key, const bool left_inclusive,
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

    uint64_t BloomedSplash::approxCount(const BloomedSplash::Iter* iter, const BloomedSplash::Iter* iter2) {
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

    uint64_t BloomedSplash::approxCount(const std::string& left_key,
                               const std::string& right_key) {
        iter_.clear(); iter2_.clear();
        iter_ = moveToKeyGreaterThan(left_key, true);
        if (!iter_.isValid()) return 0;
        iter2_ = moveToKeyGreaterThan(right_key, true);
        if (!iter2_.isValid())
            iter2_ = moveToLast();

        return approxCount(&iter_, &iter2_);
    }

    uint64_t BloomedSplash::serializedSize() const {
        return (louds_dense_->serializedSize()
                + louds_sparse_->serializedSize());
    }

    uint64_t BloomedSplash::getMemoryUsage() const {
        return (sizeof(BloomedSplash) + louds_dense_->getMemoryUsage() + louds_sparse_->getMemoryUsage() + bloomFilters_.getMemoryUsage());
    }

    level_t BloomedSplash::getHeight() const {
        return louds_sparse_->getHeight();
    }

    level_t BloomedSplash::getSparseStartLevel() const {
        return louds_sparse_->getStartLevel();
    }

    bool BloomedSplash::lookupPrefix(const std::string &prefix) {
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
                bool bf_found = bloomFilters_.lookupKey(bf_prefix);
                if (!bf_found) return false;
                // Otherwise, it can be a false positive, so we start doubting
            }
        }
        return true;
    }

//============================================================================

    void BloomedSplash::Iter::clear() {
        dense_iter_.clear();
        sparse_iter_.clear();
    }

    bool BloomedSplash::Iter::getFpFlag() const {
        return could_be_fp_;
    }

    bool BloomedSplash::Iter::isValid() const {
        return dense_iter_.isValid()
               && (dense_iter_.isComplete() || sparse_iter_.isValid());
    }

    int BloomedSplash::Iter::compare(const std::string& key) const {
        assert(isValid());
        int dense_compare = dense_iter_.compare(key);
        if (dense_iter_.isComplete() || dense_compare != 0)
            return dense_compare;
        return sparse_iter_.compare(key);
    }

    std::string BloomedSplash::Iter::getKey() const {
        if (!isValid())
            return std::string();
        if (dense_iter_.isComplete())
            return dense_iter_.getKey();
        return dense_iter_.getKey() + sparse_iter_.getKey();
    }

    int BloomedSplash::Iter::getSuffix(word_t* suffix) const {
        if (!isValid())
            return 0;
        if (dense_iter_.isComplete())
            return dense_iter_.getSuffix(suffix);
        return sparse_iter_.getSuffix(suffix);
    }

    std::string BloomedSplash::Iter::getKeyWithSuffix(unsigned* bitlen) const {
        *bitlen = 0;
        if (!isValid())
            return std::string();
        if (dense_iter_.isComplete())
            return dense_iter_.getKeyWithSuffix(bitlen);
        return dense_iter_.getKeyWithSuffix(bitlen) + sparse_iter_.getKeyWithSuffix(bitlen);
    }

    void BloomedSplash::Iter::passToSparse() {
        sparse_iter_.setStartNodeNum(dense_iter_.getSendOutNodeNum());
    }

    bool BloomedSplash::Iter::incrementDenseIter() {
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

    bool BloomedSplash::Iter::incrementSparseIter() {
        if (!sparse_iter_.isValid())
            return false;
        sparse_iter_++;
        return sparse_iter_.isValid();
    }

    bool BloomedSplash::Iter::operator ++(int) {
        if (!isValid())
            return false;
        if (incrementSparseIter())
            return true;
        return incrementDenseIter();
    }

    bool BloomedSplash::Iter::decrementDenseIter() {
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

    bool BloomedSplash::Iter::decrementSparseIter() {
        if (!sparse_iter_.isValid())
            return false;
        sparse_iter_--;
        return sparse_iter_.isValid();
    }

    bool BloomedSplash::Iter::operator --(int) {
        if (!isValid())
            return false;
        if (decrementSparseIter())
            return true;
        return decrementDenseIter();
    }
};

#endif //RANGE_FILTERING_BloomedSplash_HPP
