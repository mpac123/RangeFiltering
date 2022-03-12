#ifndef RANGE_FILTERING_Splash_HPP
#define RANGE_FILTERING_Splash_HPP

#include <string>
#include <vector>

#include "config.hpp"
#include "louds_dense.hpp"
#include "louds_sparse.hpp"
#include "splash_builder.hpp"
#include "PrefixFilter.h"

namespace range_filtering {

    class Splash : public PrefixFilter {
    public:
        class Iter {
        public:
            Iter() {};
            Iter(const Splash* fst) {
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

            friend class Splash;
        };

    public:
        Splash() {};

        //------------------------------------------------------------------
        // Input keys must be SORTED
        //------------------------------------------------------------------
        Splash(const std::vector<std::string>& keys, SplashRestraintType restraint_type,
               uint64_t absolute_restraint_val,
               double relative_restraint_val,
               double cut_off_coefficient) : restraintType_(restraint_type),
               absoluteRestraintValue_(absolute_restraint_val), relativeRestraintValue_(relative_restraint_val),
               cutOffCoefficient_(cut_off_coefficient){
            create(keys, kIncludeDense, kSparseDenseRatio);
        }

        Splash(const std::vector<std::string>& keys,
             const bool include_dense, const uint32_t sparse_dense_ratio,
               SplashRestraintType restraint_type, uint64_t absolute_restraint_val,
               double relative_restraint_val, double cut_off_coefficient) : restraintType_(restraint_type),
                                                absoluteRestraintValue_(absolute_restraint_val),
                                                relativeRestraintValue_(relative_restraint_val),
                                                cutOffCoefficient_(cut_off_coefficient) {
            create(keys, include_dense, sparse_dense_ratio);
        }

        ~Splash() {}

        void create(const std::vector<std::string>& keys,
                    const bool include_dense, const uint32_t sparse_dense_ratio);

        bool lookupKey(const std::string& key) const;
        bool lookupPrefix(const std::string& prefix) override;
        // This function searches in a conservative way: if inclusive is true
        // and the stored key prefix matches key, iter stays at this key prefix.
        Splash::Iter moveToKeyGreaterThan(const std::string& key, const bool inclusive) const;
        Splash::Iter moveToKeyLessThan(const std::string& key, const bool inclusive) const;
        Splash::Iter moveToFirst() const;
        Splash::Iter moveToLast() const;
        bool lookupRange(const std::string& left_key, const bool left_inclusive,
                         const std::string& right_key, const bool right_inclusive);
        // Accurate except at the boundaries --> undercount by at most 2
        uint64_t approxCount(const std::string& left_key, const std::string& right_key);
        uint64_t approxCount(const Splash::Iter* iter, const Splash::Iter* iter2);

        uint64_t serializedSize() const;
        uint64_t getMemoryUsage() const override;
        level_t getHeight() const;
        level_t getSparseStartLevel() const;

        std::string getName() const override { return "Splash"; }

        char* serialize() const {
            uint64_t size = serializedSize();
            char* data = new char[size];
            char* cur_data = data;
            louds_dense_->serialize(cur_data);
            louds_sparse_->serialize(cur_data);
            assert(cur_data - data == (int64_t)size);
            return data;
        }

        static Splash* deSerialize(char* src) {
            Splash* surf = new Splash();
            surf->louds_dense_ = LoudsDense::deSerialize(src);
            surf->louds_sparse_ = LoudsSparse::deSerialize(src);
            surf->iter_ = Splash::Iter(surf);
            return surf;
        }

        void destroy() {
            louds_dense_->destroy();
            louds_sparse_->destroy();
        }

    private:
        LoudsDense* louds_dense_;
        LoudsSparse* louds_sparse_;
        SplashBuilder* builder_;
        Splash::Iter iter_;
        Splash::Iter iter2_;

        SplashRestraintType restraintType_;
        uint64_t absoluteRestraintValue_;
        double relativeRestraintValue_;
        double cutOffCoefficient_;
    };

    void Splash::create(const std::vector<std::string>& keys,
                      const bool include_dense, const uint32_t sparse_dense_ratio) {
        builder_ = new SplashBuilder(include_dense, sparse_dense_ratio,
                                     restraintType_, absoluteRestraintValue_, relativeRestraintValue_,
                                     cutOffCoefficient_);
        builder_->build(keys);
        louds_dense_ = new LoudsDense(builder_);
        louds_sparse_ = new LoudsSparse(builder_);
        iter_ = Splash::Iter(this);
        delete builder_;
    }

    bool Splash::lookupKey(const std::string& key) const {
        position_t connect_node_num = 0;
        if (!louds_dense_->lookupKey(key, connect_node_num))
            return false;
        else if (connect_node_num != 0)
            return louds_sparse_->lookupKey(key, connect_node_num);
        return true;
    }

    Splash::Iter Splash::moveToKeyGreaterThan(const std::string& key, const bool inclusive) const {
        Splash::Iter iter(this);
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

    Splash::Iter Splash::moveToKeyLessThan(const std::string& key, const bool inclusive) const {
        Splash::Iter iter = moveToKeyGreaterThan(key, false);
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

    Splash::Iter Splash::moveToFirst() const {
        Splash::Iter iter(this);
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

    Splash::Iter Splash::moveToLast() const {
        Splash::Iter iter(this);
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

    bool Splash::lookupRange(const std::string& left_key, const bool left_inclusive,
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

    uint64_t Splash::approxCount(const Splash::Iter* iter, const Splash::Iter* iter2) {
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

    uint64_t Splash::approxCount(const std::string& left_key,
                               const std::string& right_key) {
        iter_.clear(); iter2_.clear();
        iter_ = moveToKeyGreaterThan(left_key, true);
        if (!iter_.isValid()) return 0;
        iter2_ = moveToKeyGreaterThan(right_key, true);
        if (!iter2_.isValid())
            iter2_ = moveToLast();

        return approxCount(&iter_, &iter2_);
    }

    uint64_t Splash::serializedSize() const {
        return (louds_dense_->serializedSize()
                + louds_sparse_->serializedSize());
    }

    uint64_t Splash::getMemoryUsage() const {
        return (sizeof(Splash) + louds_dense_->getMemoryUsage() + louds_sparse_->getMemoryUsage());
    }

    level_t Splash::getHeight() const {
        return louds_sparse_->getHeight();
    }

    level_t Splash::getSparseStartLevel() const {
        return louds_sparse_->getStartLevel();
    }

    bool Splash::lookupPrefix(const std::string &prefix) {
        if (louds_dense_->getHeight() == 0) {
            return louds_sparse_->lookupPrefix(prefix, 0);
        }
        position_t connect_node_num = 0;
        if (!louds_dense_->lookupPrefix(prefix, connect_node_num))
            return false;
        else if (connect_node_num != 0)
            return louds_sparse_->lookupPrefix(prefix, connect_node_num);
        return true;
    }

//============================================================================

    void Splash::Iter::clear() {
        dense_iter_.clear();
        sparse_iter_.clear();
    }

    bool Splash::Iter::getFpFlag() const {
        return could_be_fp_;
    }

    bool Splash::Iter::isValid() const {
        return dense_iter_.isUsed() ? dense_iter_.isValid()
               && (dense_iter_.isComplete() || sparse_iter_.isValid()) : sparse_iter_.isValid();
    }

    int Splash::Iter::compare(const std::string& key) const {
        assert(isValid());
        int dense_compare = dense_iter_.compare(key);
        if (dense_iter_.isComplete() || dense_compare != 0)
            return dense_compare;
        return sparse_iter_.compare(key);
    }

    std::string Splash::Iter::getKey() const {
        if (!isValid())
            return std::string();
        if (dense_iter_.isComplete())
            return dense_iter_.getKey();
        return dense_iter_.getKey() + sparse_iter_.getKey();
    }

    int Splash::Iter::getSuffix(word_t* suffix) const {
        if (!isValid())
            return 0;
        if (dense_iter_.isComplete())
            return dense_iter_.getSuffix(suffix);
        return sparse_iter_.getSuffix(suffix);
    }

    std::string Splash::Iter::getKeyWithSuffix(unsigned* bitlen) const {
        *bitlen = 0;
        if (!isValid())
            return std::string();
        if (dense_iter_.isComplete())
            return dense_iter_.getKeyWithSuffix(bitlen);
        return dense_iter_.getKeyWithSuffix(bitlen) + sparse_iter_.getKeyWithSuffix(bitlen);
    }

    void Splash::Iter::passToSparse() {
        sparse_iter_.setStartNodeNum(dense_iter_.getSendOutNodeNum());
    }

    bool Splash::Iter::incrementDenseIter() {
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

    bool Splash::Iter::incrementSparseIter() {
        if (!sparse_iter_.isValid())
            return false;
        sparse_iter_++;
        return sparse_iter_.isValid();
    }

    bool Splash::Iter::operator ++(int) {
        if (!isValid())
            return false;
        if (incrementSparseIter())
            return true;
        return incrementDenseIter();
    }

    bool Splash::Iter::decrementDenseIter() {
        if (!dense_iter_.isUsed()) {
            return true;
        }
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

    bool Splash::Iter::decrementSparseIter() {
        if (!sparse_iter_.isValid())
            return false;
        sparse_iter_--;
        return sparse_iter_.isValid();
    }

    bool Splash::Iter::operator --(int) {
        if (!isValid())
            return false;
        if (decrementSparseIter())
            return true;
        return decrementDenseIter();
    }
};

#endif //RANGE_FILTERING_Splash_HPP
