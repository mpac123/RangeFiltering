#include "SurfingTrie.h"

namespace range_filtering {

    SurfingTrie::TrieNode::TrieNode(SurfingTrie const& trie) : trie_(trie) {
        end_of_word_ = false;
        children_ = std::map<char, TrieNode*>();
    }

    uint64_t SurfingTrie::TrieNode::getMemoryUsage() const {
        return (sizeof(SurfingTrie::TrieNode) + getChildrenMemoryUsage()) * 8;
    }

    uint64_t SurfingTrie::TrieNode::getChildrenMemoryUsage() const {
        uint64_t children_size = 0;
        for (auto child : children_) {
            children_size += child.second->getMemoryUsage();
        }
        return children_size * 8;
    }

    uint64_t SurfingTrie::LeafNode::getMemoryUsage() const {
        // This is not entirely true in this implementation as no matter what's the size of suffix,
        // I keep 8bits per suffix, but for the sake of comparison let's assume they're more packed...
        return trie_.max_suffix_length_;
    }

    SurfingTrie::LeafNode::LeafNode(SurfingTrie const& trie, uint8_t suffix) : TrieNode(trie) {
        suffix_ = suffix;
    }

    SurfingTrie::SurfingTrie(std::vector<std::string> &keys, uint64_t max_suffix_length) {
        root = new TrieNode(*this);
        max_suffix_length_ = max_suffix_length;
        root->insertKeys(0, keys);
    }

    void SurfingTrie::TrieNode::insertKeys(uint64_t position, std::vector<std::string> &keys) {
        if (keys.empty()) {
            return;
        }

        char current = '\0';

        std::vector<std::string> keys_with_common_prefix = std::vector<std::string>();

        for (auto key : keys) {
            if (position >= key.size()) {
                end_of_word_ = true;
                continue;
            }

            // Otherwise, continue
            if (current == '\0') {
                current = key.at(position);
                keys_with_common_prefix.push_back(key);
            } else if (current != key.at(position)) {
                // Create new node and call insert recursively on it
                insertChildNode(current, position, keys_with_common_prefix);
                // Reset current and clean keys_with_common_prefix
                current = key.at(position);
                keys_with_common_prefix = std::vector<std::string>();
                keys_with_common_prefix.push_back(key);
            } else {
                current = key.at(position);
                keys_with_common_prefix.push_back(key);
            }
        }

        if (current != '\0') {
            insertChildNode(current, position, keys_with_common_prefix);
        }
    }

    void SurfingTrie::TrieNode::insertChildNode(char current, uint64_t position,
                                                std::vector<std::string> &keys) {

        // If only one key provided, add a leaf
        if (keys.size() == 1) {
            TrieNode *new_node;
            if (position + 1 == keys.at(0).size()) {
                new_node = new TrieNode(trie_);
                new_node->end_of_word_ = true;
            } else {
                new_node = new LeafNode(trie_, keys.at(0)[position + 1] >> (8 - trie_.max_suffix_length_));
                new_node->end_of_word_ = true;
            }
            children_[current] = new_node;
            return;
        }

        // Otherwise, add trie node
        auto new_node = new TrieNode(trie_);
        new_node->insertKeys(position + 1, keys);
        children_.emplace(current, new_node);
    }

    bool SurfingTrie::lookupPrefix(const std::string &prefix) {
        return root->lookupNode(prefix, 0);
    }

    bool SurfingTrie::TrieNode::lookupNode(const std::string &key, uint64_t position) {
        if (position >= key.size()) {
            return true;
        }
        auto child_node_it = children_.find(key.at(position));
        if (child_node_it == children_.end()) {
            return false;
        }
        return child_node_it->second->lookupNode(key, position + 1);
    }

    bool SurfingTrie::LeafNode::lookupNode(const std::string &key, uint64_t position) {
        if (position == key.length()) {
            return true;
        }

        if (trie_.max_suffix_length_ == 0) return true;
        auto key_suffix = key[position] >> (8 - trie_.max_suffix_length_);
        return key_suffix == suffix_;
    }

    uint64_t SurfingTrie::getMemoryUsage() const {
        return sizeof(SurfingTrie) + int(root->getMemoryUsage() / 8.);
    }
}