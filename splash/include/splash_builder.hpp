#ifndef SPLASH_BUILDER_H_
#define SPLASH_BUILDER_H_

#include "config.hpp"
#include "../../baseline/include/Trie.hpp"
#include <vector>
#include <cassert>

namespace range_filtering_splash {
    class SplashBuilder {
    public:
        SplashBuilder() : sparse_start_level_(0) {};
        explicit SplashBuilder(bool include_dense, uint32_t sparse_dense_ratio,
                               SplashRestraintType restraint_type, uint64_t absolute_restraint_val,
                               double relative_restraint_val, double cutOffCoefficient)
                : include_dense_(include_dense), sparse_dense_ratio_(sparse_dense_ratio),
                  sparse_start_level_(0), restraintType_(restraint_type),
                  absoluteRestraintValue_(absolute_restraint_val), relativeRestraintValue_(relative_restraint_val),
                  cutOffCoefficient_(cutOffCoefficient) {};

        ~SplashBuilder() {};

        // Fills in the LOUDS-dense and sparse vectors (members of this class)
        // through a single scan of the sorted key list.
        // After build, the member vectors are used in SuRF constructor.
        // After build, the member vectors are used in SuRF constructor.
        // REQUIRED: provided key list must be sorted.
        void build(const std::vector<std::string>& keys);

        static bool readBit(const std::vector<word_t>& bits, const position_t pos) {
            assert(pos < (bits.size() * kWordSize));
            position_t word_id = pos / kWordSize;
            position_t offset = pos % kWordSize;
            return (bits[word_id] & (kMsbMask >> offset));
        }

        static void setBit(std::vector<word_t>& bits, const position_t pos) {
            assert(pos < (bits.size() * kWordSize));
            position_t word_id = pos / kWordSize;
            position_t offset = pos % kWordSize;
            bits[word_id] |= (kMsbMask >> offset);
        }

        level_t getTreeHeight() const {
            return labels_.size();
        }

        // const accessors
        const std::vector<std::vector<word_t> >& getBitmapLabels() const {
            return bitmap_labels_;
        }
        const std::vector<std::vector<word_t> >& getBitmapChildIndicatorBits() const {
            return bitmap_child_indicator_bits_;
        }
        const std::vector<std::vector<word_t> >& getPrefixkeyIndicatorBits() const {
            return prefixkey_indicator_bits_;
        }
        const std::vector<std::vector<label_t> >& getLabels() const {
            return labels_;
        }
        const std::vector<std::vector<word_t> >& getChildIndicatorBits() const {
            return child_indicator_bits_;
        }
        const std::vector<std::vector<word_t> >& getLoudsBits() const {
            return louds_bits_;
        }
        const std::vector<std::vector<word_t> >& getSuffixes() const {
            return suffixes_;
        }
        const std::vector<position_t>& getSuffixCounts() const {
            return suffix_counts_;
        }
        const std::vector<position_t>& getNodeCounts() const {
            return node_counts_;
        }
        level_t getSparseStartLevel() const {
            return sparse_start_level_;
        }
        level_t getSuffixLen() const {
            return hash_suffix_len_ + real_suffix_len_;
        }
        level_t getHashSuffixLen() const {
            return hash_suffix_len_;
        }
        level_t getRealSuffixLen() const {
            return real_suffix_len_;
        }

        bool isExact() const { return restraintType_ == SplashRestraintType::none; }

    private:
        static bool isSameKey(const std::string& a, const std::string& b) {
            return a.compare(b) == 0;
        }

        // Fill in the LOUDS-Sparse vectors through a single scan
        // of the sorted key list.
        void buildSparse(const std::vector<std::string>& keys, range_filtering::Trie &trie);

        // Walks down the current partially-filled trie by comparing key to
        // its previous key in the list until their prefixes do not match.
        // The previous key is stored as the last items in the per-level
        // label vector.
        // For each matching prefix byte(label), it sets the corresponding
        // child indicator bit to 1 for that label.
        level_t skipCommonPrefix(const std::string& key);

        // Starting at the start_level of the trie, the function inserts
        // key bytes to the trie vectors until the first byte/label where
        // key and next_key do not match.
        // This function is called after skipCommonPrefix. Therefore, it
        // guarantees that the stored prefix of key is unique in the trie.
        level_t insertKeyBytesToTrie(const std::string& key, const std::string& next_key,
                                     const level_t start_level, range_filtering::Trie::TrieNode* node);

        // Fills in the suffix byte for key

        inline bool isCharCommonPrefix(const label_t c, const level_t level) const;
        inline bool isLevelEmpty(const level_t level) const;
        inline void moveToNextItemSlot(const level_t level);
        void insertKeyByte(const char c, const level_t level, const bool is_start_of_node, const bool is_term);
        inline void storeSuffix(const level_t level, const word_t suffix);

        // Compute sparse_start_level_ according to the pre-defined
        // size ratio between Sparse and Dense levels.
        // Dense size < Sparse size / sparse_dense_ratio_
        inline void determineCutoffLevel();

        inline uint64_t computeDenseMem(const level_t downto_level) const;
        inline uint64_t computeSparseMem(const level_t start_level) const;

        // Fill in the LOUDS-Dense vectors based on the built
        // Sparse vectors.
        // Called after sparse_start_level_ is set.
        void buildDense();

        void initDenseVectors(const level_t level);
        void setLabelAndChildIndicatorBitmap(const level_t level, const position_t node_num, const position_t pos);

        position_t getNumItems(const level_t level) const;
        void addLevel();
        bool isStartOfNode(const level_t level, const position_t pos) const;
        bool isTerminator(const level_t level, const position_t pos) const;

        bool cutOffNode(range_filtering::Trie::TrieNode* node);
        bool isRestrained(range_filtering::Trie::TrieNode* node) const;

    private:
        // trie level < sparse_start_level_: LOUDS-Dense
        // trie level >= sparse_start_level_: LOUDS-Sparse
        bool include_dense_;
        uint32_t sparse_dense_ratio_;
        level_t sparse_start_level_;

        SplashRestraintType restraintType_;
        uint64_t absoluteRestraintValue_;
        double relativeRestraintValue_;
        double cutOffCoefficient_;

        // LOUDS-Sparse bit/byte vectors
        std::vector<std::vector<label_t> > labels_;
        std::vector<std::vector<word_t> > child_indicator_bits_;
        std::vector<std::vector<word_t> > louds_bits_;

        // LOUDS-Dense bit vectors
        std::vector<std::vector<word_t> > bitmap_labels_;
        std::vector<std::vector<word_t> > bitmap_child_indicator_bits_;
        std::vector<std::vector<word_t> > prefixkey_indicator_bits_;

        level_t hash_suffix_len_;
        level_t real_suffix_len_;
        std::vector<std::vector<word_t> > suffixes_;
        std::vector<position_t> suffix_counts_;

        // auxiliary per level bookkeeping vectors
        std::vector<position_t> node_counts_;
        std::vector<bool> is_last_item_terminator_;

        std::string last_inserted_ = "";
    };

    void SplashBuilder::build(const std::vector<std::string>& keys) {
        assert(keys.size() > 0);
        auto trie = range_filtering::Trie(keys);
        buildSparse(keys, trie);
        if (include_dense_) {
            determineCutoffLevel();
            buildDense();
        }
    }

    void SplashBuilder::buildSparse(const std::vector<std::string>& keys, range_filtering::Trie &trie) {
        for (position_t i = 0; i < keys.size(); i++) {
            level_t level = 0;
            if (i > 0) level = skipCommonPrefix(keys[i]);

            // find the node in Trie
            auto current_node = trie.root;
            for (size_t j = 0; j < level; j++) {
                current_node = current_node->children[keys[i][j]];
            }

            position_t curpos = i;
            while ((i + 1 < keys.size()) && isSameKey(keys[curpos], keys[i+1]))
                i++;
            if (i < keys.size() - 1)
                insertKeyBytesToTrie(keys[curpos], keys[i+1], level, current_node);
            else // for last key, there is no successor key in the list
                insertKeyBytesToTrie(keys[curpos], std::string(), level, current_node);
        }
    }

    level_t SplashBuilder::skipCommonPrefix(const std::string& key) {
        level_t level = 0;
        while (level < key.length() && level < last_inserted_.length() && key[level] == last_inserted_[level]) {
            setBit(child_indicator_bits_[level], getNumItems(level) - 1);
            level++;
        }
        return level;
    }

    level_t SplashBuilder::insertKeyBytesToTrie(const std::string& key, const std::string& next_key,
                                                const level_t start_level, range_filtering::Trie::TrieNode* node) {
        assert(start_level < key.length());

        last_inserted_ = last_inserted_.substr(0, start_level);
        level_t level = start_level;
        bool is_start_of_node = false;
        bool is_term = false;
        // If it is the start of level, the louds bit needs to be set.
        if (isLevelEmpty(level))
            is_start_of_node = true;

        // After skipping the common prefix, the first following byte
        // shoud be in an the node as the previous key.
        insertKeyByte(key[level], level, is_start_of_node, is_term);
        last_inserted_ += key[level];
        node = node->children[key[level]];
        level++;
        if (!isSameKey(key.substr(0, level), next_key.substr(0, level)) && level == key.length()) {
            return level;
        }

        // All the following bytes inserted must be the start of a
        // new node.
//        while (isSameKey(key.substr(0, level), next_key.substr(0, level)) && level < key.length() && level < next_key.length() && key[level] == next_key[level]) {
//            is_start_of_node = true;
//            insertKeyByte(key[level], level, is_start_of_node, is_term);
//            level++;
//        }
        is_start_of_node = true;
        while (level < key.length() && !cutOffNode(node)) {

            insertKeyByte(key[level], level, is_start_of_node, is_term);
            last_inserted_ += key[level];
            node = node->children[key[level]];
            level++;
        }

        if (level == key.length() && isSameKey(key.substr(0, level), next_key.substr(0, level))) {
            is_term = true;
            insertKeyByte(kTerminator, level, is_start_of_node, is_term);
            level++;
        }

        // The last byte inserted makes key unique in the trie.
//        if (level < key.length()) {
//            // Store all the nodes till the end of the word
//            while (level < key.length()) {
//                is_start_of_node = true;
//                insertKeyByte(key[level], level, is_start_of_node, is_term);
//                level++;
//            }
//        } else {
//            is_term = true;
//            insertKeyByte(kTerminator, level, is_start_of_node, is_term);
//            level++;
//        }

        return level;
    }

    inline bool SplashBuilder::isCharCommonPrefix(const label_t c, const level_t level) const {
        return (level < getTreeHeight())
               && (!is_last_item_terminator_[level])
               && (c == labels_[level].back());
    }

    inline bool SplashBuilder::isLevelEmpty(const level_t level) const {
        return (level >= getTreeHeight()) || (labels_[level].size() == 0);
    }

    inline void SplashBuilder::moveToNextItemSlot(const level_t level) {
        assert(level < getTreeHeight());
        position_t num_items = getNumItems(level);
        if (num_items % kWordSize == 0) {
            child_indicator_bits_[level].push_back(0);
            louds_bits_[level].push_back(0);
        }
    }

    void SplashBuilder::insertKeyByte(const char c, const level_t level, const bool is_start_of_node, const bool is_term) {
        // level should be at most equal to tree height
        if (level >= getTreeHeight())
            addLevel();

        assert(level < getTreeHeight());

        // sets parent node's child indicator
        if (level > 0)
            setBit(child_indicator_bits_[level-1], getNumItems(level-1) - 1);

        labels_[level].push_back(c);
        if (is_start_of_node) {
            setBit(louds_bits_[level], getNumItems(level) - 1);
            node_counts_[level]++;
        }
        is_last_item_terminator_[level] = is_term;

        moveToNextItemSlot(level);
    }


    inline void SplashBuilder::storeSuffix(const level_t level, const word_t suffix) {
        level_t suffix_len = getSuffixLen();
        position_t pos = suffix_counts_[level-1] * suffix_len;
        assert(pos <= (suffixes_[level-1].size() * kWordSize));
        if (pos == (suffixes_[level-1].size() * kWordSize))
            suffixes_[level-1].push_back(0);
        position_t word_id = pos / kWordSize;
        position_t offset = pos % kWordSize;
        position_t word_remaining_len = kWordSize - offset;
        if (suffix_len <= word_remaining_len) {
            word_t shifted_suffix = suffix << (word_remaining_len - suffix_len);
            suffixes_[level-1][word_id] += shifted_suffix;
        } else {
            word_t suffix_left_part = suffix >> (suffix_len - word_remaining_len);
            suffixes_[level-1][word_id] += suffix_left_part;
            suffixes_[level-1].push_back(0);
            word_id++;
            word_t suffix_right_part = suffix << (kWordSize - (suffix_len - word_remaining_len));
            suffixes_[level-1][word_id] += suffix_right_part;
        }
        suffix_counts_[level-1]++;
    }

    inline void SplashBuilder::determineCutoffLevel() {
        level_t cutoff_level = 0;
        uint64_t dense_mem = computeDenseMem(cutoff_level);
        uint64_t sparse_mem = computeSparseMem(cutoff_level);
        while ((cutoff_level < getTreeHeight()) && (dense_mem * sparse_dense_ratio_ < sparse_mem)) {
            cutoff_level++;
            dense_mem = computeDenseMem(cutoff_level);
            sparse_mem = computeSparseMem(cutoff_level);
        }
        sparse_start_level_ = cutoff_level--;
    }

    inline uint64_t SplashBuilder::computeDenseMem(const level_t downto_level) const {
        assert(downto_level <= getTreeHeight());
        uint64_t mem = 0;
        for (level_t level = 0; level < downto_level; level++) {
            mem += (2 * kFanout * node_counts_[level]);
            if (level > 0)
                mem += (node_counts_[level - 1] / 8 + 1);
            mem += (suffix_counts_[level] * getSuffixLen() / 8);
        }
        return mem;
    }

    inline uint64_t SplashBuilder::computeSparseMem(const level_t start_level) const {
        uint64_t mem = 0;
        for (level_t level = start_level; level < getTreeHeight(); level++) {
            position_t num_items = labels_[level].size();
            mem += (num_items + 2 * num_items / 8 + 1);
            mem += (suffix_counts_[level] * getSuffixLen() / 8);
        }
        return mem;
    }

    void SplashBuilder::buildDense() {
        for (level_t level = 0; level < sparse_start_level_; level++) {
            initDenseVectors(level);
            if (getNumItems(level) == 0) continue;

            position_t node_num = 0;
            if (isTerminator(level, 0))
                setBit(prefixkey_indicator_bits_[level], 0);
            else
                setLabelAndChildIndicatorBitmap(level, node_num, 0);
            for (position_t pos = 1; pos < getNumItems(level); pos++) {
                if (isStartOfNode(level, pos)) {
                    node_num++;
                    if (isTerminator(level, pos)) {
                        setBit(prefixkey_indicator_bits_[level], node_num);
                        continue;
                    }
                }
                setLabelAndChildIndicatorBitmap(level, node_num, pos);
            }
        }
    }

    void SplashBuilder::initDenseVectors(const level_t level) {
        bitmap_labels_.push_back(std::vector<word_t>());
        bitmap_child_indicator_bits_.push_back(std::vector<word_t>());
        prefixkey_indicator_bits_.push_back(std::vector<word_t>());

        for (position_t nc = 0; nc < node_counts_[level]; nc++) {
            for (int i = 0; i < (int)kFanout; i += kWordSize) {
                bitmap_labels_[level].push_back(0);
                bitmap_child_indicator_bits_[level].push_back(0);
            }
            if (nc % kWordSize == 0)
                prefixkey_indicator_bits_[level].push_back(0);
        }
    }

    void SplashBuilder::setLabelAndChildIndicatorBitmap(const level_t level,
                                                      const position_t node_num, const position_t pos) {
        label_t label = labels_[level][pos];
        setBit(bitmap_labels_[level], node_num * kFanout + label);
        if (readBit(child_indicator_bits_[level], pos))
            setBit(bitmap_child_indicator_bits_[level], node_num * kFanout + label);
    }

    void SplashBuilder::addLevel() {
        labels_.push_back(std::vector<label_t>());
        child_indicator_bits_.push_back(std::vector<word_t>());
        louds_bits_.push_back(std::vector<word_t>());
        suffixes_.push_back(std::vector<word_t>());
        suffix_counts_.push_back(0);

        node_counts_.push_back(0);
        is_last_item_terminator_.push_back(false);

        child_indicator_bits_[getTreeHeight() - 1].push_back(0);
        louds_bits_[getTreeHeight() - 1].push_back(0);
    }

    position_t SplashBuilder::getNumItems(const level_t level) const {
        return labels_[level].size();
    }

    bool SplashBuilder::isStartOfNode(const level_t level, const position_t pos) const {
        return readBit(louds_bits_[level], pos);
    }

    bool SplashBuilder::isTerminator(const level_t level, const position_t pos) const {
        label_t label = labels_[level][pos];
        return ((label == kTerminator) && !readBit(child_indicator_bits_[level], pos));
    }

    bool SplashBuilder::cutOffNode(range_filtering::Trie::TrieNode* node) {
        if (restraintType_ == SplashRestraintType::none) return false;
        double cutoffLevel = (double) node->height_ / (double) node->children_count_;
        return cutoffLevel >= cutOffCoefficient_ && node->children.size() == 1 && !isRestrained(node);
    }

    bool SplashBuilder::isRestrained(range_filtering::Trie::TrieNode* current_node) const {
        // node is restrained === we cannot cut off the node
        if (restraintType_ == SplashRestraintType::absolute) {
            return current_node->height_ > absoluteRestraintValue_;
        } else if (restraintType_ == SplashRestraintType::relative) {
            return current_node->level_ + 1 <
                   (1. - relativeRestraintValue_) * (current_node->level_ + current_node->height_ + 0.);
        }
        return false;
    }
};

#endif // FST_BUILDER_H_