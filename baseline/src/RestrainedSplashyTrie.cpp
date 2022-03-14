#include "RestrainedSplashyTrie.h"

namespace range_filtering {

    RestrainedSplashyTrie::TrieNode::TrieNode(RestrainedSplashyTrie const& trie) : trie_(trie) {
        end_of_word_ = false;
        children_ = std::map<char, TrieNode*>();
    }

    uint64_t RestrainedSplashyTrie::TrieNode::getMemoryUsage() const {
        // char + 64-bit pointer per each child + boolean flag
        uint64_t node_size_bits = (8 + 64) * children_.size() + 1;
        return node_size_bits + getChildrenMemoryUsage();
    }

    uint64_t RestrainedSplashyTrie::TrieNode::getChildrenMemoryUsage() const {
        uint64_t children_size = 0;
        for (auto child : children_) {
            children_size += child.second->getMemoryUsage();
        }
        return children_size;
    }

    uint64_t RestrainedSplashyTrie::LeafNode::getMemoryUsage() const {
        // This is not entirely true in this implementation as no matter what's the size of suffix,
        // I keep 8bits per suffix, but for the sake of comparison let's assume they're more packed...
        return trie_.max_suffix_length_;
    }

    RestrainedSplashyTrie::LeafNode::LeafNode(RestrainedSplashyTrie const& trie, uint8_t suffix) : TrieNode(trie) {
        suffix_ = suffix;
    }

    RestrainedSplashyTrie::RestrainedSplashyTrie(const std::vector<std::string> &keys, uint64_t max_suffix_length,
                                                 double splashiness_coefficient, RestraintType restraintType,
                                                 uint64_t absolute_restraint_val, double relative_restraint_val) {
        root = new TrieNode(*this);
        max_suffix_length_ = max_suffix_length;
        splashiness_coefficient_ = splashiness_coefficient;
        restraintType_ = restraintType;
        absoluteRestraintValue_ = absolute_restraint_val;
        relativeRestraintValue_ = relative_restraint_val;

        auto trie = Trie(keys);
        Trie::TrieNode* current_node = trie.root;
        root->insertKeys(current_node);
    }

    void RestrainedSplashyTrie::TrieNode::insertKeys(Trie::TrieNode* current_node) {
        if (current_node->children_count_ == 0) {
            end_of_word_ = current_node->end_of_word;
            return;
        }

        for (auto child : current_node->children) {
            double splashiness = (double) child.second->height_ / (double) child.second->children_count_;
            if (splashiness >= trie_.splashiness_coefficient_
                && child.second->children.size() == 1
                && !trie_.isRestrained(child.second)) {
                auto new_node = new LeafNode(trie_, child.second->children.begin()->first >> (8 - trie_.max_suffix_length_));
                new_node->end_of_word_ = true;
                children_[child.first] = new_node;
            } else {
                auto new_node = new TrieNode(trie_);
                new_node->end_of_word_ = child.second->end_of_word;
                new_node->insertKeys(child.second);
                children_[child.first] = new_node;
            }
        }

    }

    bool RestrainedSplashyTrie::lookupPrefix(const std::string &prefix) {
        return root->lookupNode(prefix, 0);
    }

    bool RestrainedSplashyTrie::TrieNode::lookupNode(const std::string &key, uint64_t position) {
        if (position >= key.size()) {
            return true;
        }
        auto child_node_it = children_.find(key.at(position));
        if (child_node_it == children_.end()) {
            return false;
        }
        return child_node_it->second->lookupNode(key, position + 1);
    }

    bool RestrainedSplashyTrie::LeafNode::lookupNode(const std::string &key, uint64_t position) {
        if (position == key.length()) {
            return true;
        }

        if (trie_.max_suffix_length_ == 0) return true;
        auto key_suffix = key[position] >> (8 - trie_.max_suffix_length_);
        return key_suffix == suffix_;
    }

    uint64_t RestrainedSplashyTrie::getMemoryUsage() const {
        auto bytes = (unsigned long long) ((64. + root->getMemoryUsage()) / 8.0);
        return bytes + 1;
    }

    bool RestrainedSplashyTrie::isRestrained(Trie::TrieNode* current_node) const {
        if (restraintType_ == RestraintType::absolute) {
            return current_node->height_ < absoluteRestraintValue_;
        }
        return current_node->level_ + 1 < (1. - relativeRestraintValue_) * (current_node->level_ + current_node->height_ + 1);
    }
}