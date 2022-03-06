#ifndef RANGEFILTERING_TRIE_HPP
#define RANGEFILTERING_TRIE_HPP

#include <vector>
#include <iostream>
#include <map>

#include <PrefixFilter.h>

namespace range_filtering {

class Trie : public PrefixFilter {

public:
    class TrieNode {
    public:
        TrieNode *parent;
        std::map<char, TrieNode *> children;
        bool end_of_word;
        uint64_t height_;
        uint64_t children_count_;
        uint64_t level_;

        TrieNode();
        explicit TrieNode(TrieNode* parent);

        unsigned long long getMemoryUsage() const;
        uint64_t calculateHeight();
        uint64_t calculateChildrenCount();
        void calculateLevel(uint64_t level);
    };

    class Iter {
    public:
        explicit Iter(const Trie *trie);
        void clear();
        bool isValid() const { return is_valid_; };
        int compare(const std::string& key) const;
        std::string getKey() const;

        bool operator ++(int);

    private:
        void findLeftMostKey();
        void findLeftMostKeyOfParentWithChildOnRight();

        bool is_valid_;
        uint8_t key_len_;
        std::vector<char> key_;
        TrieNode *current_node_;
        TrieNode *trie_root_;

        friend class Trie;
    };

public:
    TrieNode *root;

    // Currently, keys must be sorted
    explicit Trie(std::vector<std::string> &keys);

    bool lookupKey(std::string key);
    bool lookupPrefix(const std::string &prefix) override;
    Trie::Iter moveToKeyGreaterThan(const std::string& key, const bool inclusive);
    bool lookupRange(const std::string& left_key, const bool left_inclusive,
                     const std::string& right_key, const bool right_inclusive);
    unsigned long long getMemoryUsage() const override;
    std::string getName() const override { return "Trie"; }

private:
    void insert(TrieNode *node, uint64_t position, std::vector<std::string> &keys);
    bool lookupNode(const std::string &key, uint64_t position, TrieNode *node, bool exact);

    void moveDownTheNodeToKeyGreaterThan(const std::string &key, uint64_t position, Iter& iter);
};

} // namespace range_filtering

#endif //RANGEFILTERING_TRIE_HPP
