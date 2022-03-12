#include "RestrainedSurfingTrie.h"

namespace range_filtering {

    RestrainedSurfingTrie::TrieNode::TrieNode(RestrainedSurfingTrie const& trie) : trie_(trie) {
        end_of_word_ = false;
        children_ = std::map<char, TrieNode*>();
    }

    uint64_t RestrainedSurfingTrie::TrieNode::getMemoryUsage() const {
        // char + 64-bit pointer per each child + boolean flag
        uint64_t node_size_bits = (8 + 64) * children_.size() + 1;
        return node_size_bits + getChildrenMemoryUsage();
    }

    uint64_t RestrainedSurfingTrie::TrieNode::getChildrenMemoryUsage() const {
        uint64_t children_size = 0;
        for (auto child : children_) {
            children_size += child.second->getMemoryUsage();
        }
        return children_size;
    }

    uint64_t RestrainedSurfingTrie::LeafNode::getMemoryUsage() const {
        // This is not entirely true in this implementation as no matter what's the size of suffix,
        // I keep 8bits per suffix, but for the sake of comparison let's assume they're more packed...
        return trie_.max_suffix_length_;
    }

    RestrainedSurfingTrie::LeafNode::LeafNode(RestrainedSurfingTrie const& trie, uint8_t suffix) : TrieNode(trie) {
        suffix_ = suffix;
    }

    RestrainedSurfingTrie::RestrainedSurfingTrie(std::vector<std::string> &keys, uint64_t max_suffix_length,
                                                 RestraintType restraintType, uint64_t absoluteMaxRestraintValue,
                                                 double relativeMaxRestraintValue) {
        root = new TrieNode(*this);
        max_suffix_length_ = max_suffix_length;
        restraintType_ = restraintType;
        absoluteRestraintValue_ = absoluteMaxRestraintValue;
        relativeRestraintValue_ = relativeMaxRestraintValue;
        root->insertKeys(0, keys);
    }

    void RestrainedSurfingTrie::TrieNode::insertKeys(uint64_t position, std::vector<std::string> &keys) {
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

    void RestrainedSurfingTrie::TrieNode::insertChildNode(char current, uint64_t position,
                                                std::vector<std::string> &keys) {

        // If only one key provided, check restraints and, if allowed, add a leaf
        if (keys.size() == 1) {
            TrieNode *new_node;
            if (position + 1 == keys.at(0).size()) {
                new_node = new TrieNode(trie_);
                new_node->end_of_word_ = true;
            } else {
                // Check restraints
                if (trie_.isRestrained(position, keys.at(0))) {
                    new_node = new TrieNode(trie_);
                    new_node->insertKeys(position + 1, keys);
                    children_[current] = new_node;
                    return;
                }
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

    bool RestrainedSurfingTrie::lookupPrefix(const std::string &prefix) {
        return root->lookupNode(prefix, 0);
    }

    bool RestrainedSurfingTrie::TrieNode::lookupNode(const std::string &key, uint64_t position) {
        if (position >= key.size()) {
            return true;
        }
        auto child_node_it = children_.find(key.at(position));
        if (child_node_it == children_.end()) {
            return false;
        }
        return child_node_it->second->lookupNode(key, position + 1);
    }

    bool RestrainedSurfingTrie::LeafNode::lookupNode(const std::string &key, uint64_t position) {
        if (position == key.length()) {
            return true;
        }

        if (trie_.max_suffix_length_ == 0) return true;
        auto key_suffix = key[position] >> (8 - trie_.max_suffix_length_);
        return key_suffix == suffix_;
    }

    uint64_t RestrainedSurfingTrie::getMemoryUsage() const {
        auto bytes = (unsigned long long) ((64. + root->getMemoryUsage()) / 8.0);
        return bytes + 1;
    }

    bool RestrainedSurfingTrie::isRestrained(uint64_t position, std::string key) const {
        if (restraintType_ == RestraintType::absolute) {
            return (int)position + 1 < (int)key.length() - (int)absoluteRestraintValue_;
        }
        return position + 1 < (1. - relativeRestraintValue_) * key.length();
    }
}