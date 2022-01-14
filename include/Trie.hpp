#ifndef RANGEFILTERING_TRIE_HPP
#define RANGEFILTERING_TRIE_HPP

#include <vector>
#include <iostream>
#include <unordered_map>

namespace range_filtering {

class Trie {

public:
    class TrieNode {
    public:
        std::unordered_map<char, TrieNode *> children;
        bool end_of_word;

        TrieNode();
    };

public:
    explicit Trie(std::vector<std::string> &keys);

    bool lookup(std::string key);

private:
    TrieNode *root;

    void insert(TrieNode *node, uint64_t position, std::vector<std::string> &keys);

    bool lookupNode(std::string &key, uint64_t position, TrieNode *node);
};

} // namespace range_filtering

#endif //RANGEFILTERING_TRIE_HPP
