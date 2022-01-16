#ifndef RANGEFILTERING_TRIE_HPP
#define RANGEFILTERING_TRIE_HPP

#include <vector>
#include <iostream>
#include <map>

namespace range_filtering {

class Trie {

public:
    class TrieNode {
    public:
        TrieNode *parent;
        std::map<char, TrieNode *> children;
        bool end_of_word;

        TrieNode();
        explicit TrieNode(TrieNode* parent);

        uint64_t getMemoryUsage() const;
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
    explicit Trie(std::vector<std::string> &keys);

    bool lookupKey(std::string key);
    bool lookupPrefix(std::string prefix);
    Trie::Iter moveToKeyGreaterThan(const std::string& key, const bool inclusive);
    bool lookupRange(const std::string& left_key, const bool left_inclusive,
                     const std::string& right_key, const bool right_inclusive);
    uint64_t getMemoryUsage() const;

private:
    TrieNode *root;

    void insert(TrieNode *node, uint64_t position, std::vector<std::string> &keys);
    bool lookupNode(std::string &key, uint64_t position, TrieNode *node, bool exact);

    void moveDownTheNodeToKeyGreaterThan(const std::string &key, uint64_t position, Iter& iter);
};

} // namespace range_filtering

#endif //RANGEFILTERING_TRIE_HPP
