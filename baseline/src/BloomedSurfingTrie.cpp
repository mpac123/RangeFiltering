#include "BloomedSurfingTrie.h"

namespace range_filtering {

    BloomedSurfingTrie::TrieNode::TrieNode(BloomedSurfingTrie const& trie) : trie_(trie) {
        end_of_word_ = false;
        children_ = std::map<char, TrieNode*>();
    }

    unsigned long long BloomedSurfingTrie::TrieNode::getMemoryUsage() const {
        // char + 64-bit pointer per each child + boolean flag
        uint64_t node_size_bits = (8 + 64) * children_.size() + 1;
        return node_size_bits + getChildrenMemoryUsage();
    }

    unsigned long long BloomedSurfingTrie::TrieNode::getChildrenMemoryUsage() const {
        uint64_t children_size = 0;
        for (auto child : children_) {
            children_size += child.second->getMemoryUsage();
        }
        return children_size;
    }

    unsigned long long BloomedSurfingTrie::LeafNode::getMemoryUsage() const {
        // This is not entirely true in this implementation as no matter what's the size of suffix,
        // I keep 8bits per suffix, but for the sake of comparison let's assume they're more packed...
        return trie_.max_suffix_length_;
    }

    BloomedSurfingTrie::LeafNode::LeafNode(BloomedSurfingTrie const& trie, uint8_t suffix) : TrieNode(trie) {
        suffix_ = suffix;
    }

    BloomedSurfingTrie::BloomedSurfingTrie(std::vector<std::string> &keys, uint64_t max_suffix_length,
                                           uint32_t BF_size, double max_penalty) {
        root = new TrieNode(*this);
        max_suffix_length_ = max_suffix_length;
        uint64_t max_key_length = 0;
        for (auto key : keys) {
            max_key_length = std::max(max_key_length, key.length());
        }
        prefixes_ = std::vector<std::vector<std::string>>(max_key_length);
//        for (size_t i = 0; i < max_key_length; i++) {
//            prefixes_.emplace_back();
//        }
        root->insertKeys(0, keys);
        bloomFilters_ = BloomFiltersEnsemble(prefixes_, BF_size, max_penalty);
    }

    void BloomedSurfingTrie::TrieNode::insertKeys(uint64_t position, std::vector<std::string> &keys) {
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

    void BloomedSurfingTrie::TrieNode::insertChildNode(char current, uint64_t position,
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

                // Additionally, insert the prefixes of length position+ to the BF
                for (size_t i = 1; i < keys.at(0).length() - position; i++) {
                    std::string substr = keys.at(0).substr(0, position + i + 1);
                    //trie_.prefixes_[substr.length() - 1].push_back(substr);
                    auto &ref = const_cast <std::vector<std::string>&> (trie_.prefixes_[substr.length() - 1]);
                    ref.push_back(substr);
                    //vec.push_back(substr);
                    //trie_.prefixes_[substr.length() - 1].;
                }
            }
            children_[current] = new_node;
            return;
        }

        // Otherwise, add trie node
        auto new_node = new TrieNode(trie_);
        new_node->insertKeys(position + 1, keys);
        children_.emplace(current, new_node);
    }

    bool BloomedSurfingTrie::lookupPrefix(const std::string &prefix) {
        return root->lookupNode(prefix, 0);
    }

    bool BloomedSurfingTrie::TrieNode::lookupNode(const std::string &key, uint64_t position) {
        if (position >= key.size()) {
            return true;
        }
        auto child_node_it = children_.find(key.at(position));
        if (child_node_it == children_.end()) {
            return false;
        }
        return child_node_it->second->lookupNode(key, position + 1);
    }

    bool BloomedSurfingTrie::LeafNode::lookupNode(const std::string &key, uint64_t position) {
        if (position == key.length()) {
            return true;
        }

        bool may_exist = false;
        if (trie_.max_suffix_length_ == 0) may_exist = true;
        else may_exist = key[position] >> (8 - trie_.max_suffix_length_) == suffix_;

        if (!may_exist) return false;
        // Otherwise, explore bloom filters

        for (size_t i = 1; i <= key.length() - position; i++) {
            auto prefix = key.substr(0, position + i);
            if (!trie_.bloomFilters_.lookupKey(prefix)) return false;
        }

        return true;
    }

    unsigned long long BloomedSurfingTrie::getMemoryUsage() const {
        auto bytes = (unsigned long long) ((64. + root->getMemoryUsage()) / 8.0);
        return bytes + 1 + bloomFilters_.getMemoryUsage();
    }

} // namespace range_filtering