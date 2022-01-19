#include <CompactTrie.hpp>

namespace range_filtering {

    CompactTrie::TrieNode::TrieNode() {
        end_of_word_ = false;
        children_ = std::map<char, TrieNode *>();
    }

    uint64_t CompactTrie::TrieNode::getMemoryUsage() const {
        return sizeof(CompactTrie::TrieNode) + getChildrenMemoryUsage();
    }

    uint64_t CompactTrie::TrieNode::getChildrenMemoryUsage() const {
        uint64_t children_size = 0;
        for (auto child : children_) {
            children_size += child.second->getMemoryUsage();
        }
        return children_size;
    }

    uint64_t CompactTrie::CompactNode::getMemoryUsage() const {
        return sizeof(CompactTrie::TrieNode) + getChildrenMemoryUsage();
    }

    CompactTrie::CompactNode::CompactNode(uint64_t fingerprint, uint64_t length) : TrieNode() {
        fingerprint_ = fingerprint;
        length_ = length;
    }

    CompactTrie::CompactTrie(std::vector<std::string> &keys) {
        root = new TrieNode();
        insert(root, 0, keys);
    }

    void CompactTrie::insert(TrieNode *node, uint64_t position, std::vector<std::string> &keys) {
        if (keys.empty()) {
            return;
        }

        char current = '\0';

        std::vector<std::string> keys_with_common_prefix = std::vector<std::string>();

        for (auto key : keys) {
            if (position >= key.size()) {
                node->end_of_word_ = true;
                // Create a new node and call insert recursively
                insertNode(current, position, keys_with_common_prefix, node);
                // Reset current and clean keys_with_common_prefix
                current = key.at(position);
                keys_with_common_prefix = std::vector<std::string>();
                keys_with_common_prefix.push_back(key);
                continue;
            }

            // Otherwise, continue
            if (current == '\0') {
                current = key.at(position);
                keys_with_common_prefix.push_back(key);
            } else if (current != key.at(position)) {
                // Create a new node and call insert recursively
                insertNode(current, position, keys_with_common_prefix, node);
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
            insertNode(current, position, keys_with_common_prefix, node);
        }
    }

    void CompactTrie::insertNode(char current, uint64_t position,
                                 std::vector<std::string> &keys, TrieNode* node) {
        // If only one key provided, add a leaf
        if (keys.size() == 1) {
            TrieNode *new_node;
            if (position + 1 == keys.at(0).size()) {
                new_node = new TrieNode();
                new_node->end_of_word_ = true;
            } else {
                auto substring = keys.at(0).substr(position);
                new_node = new CompactNode(getKarpRabinFingerprint(substring), keys.at(0).size() - position);
                new_node->end_of_word_ = true;
            }
            node->children_[current] = new_node;
            return;
        }

        // Otherwise, find longest common prefix
        std::string substring;
        substring.push_back(current);

        for (size_t i = 1; i < keys.at(0).size(); i++) {
            char ch = keys.at(0)[position + i];
            bool difference = false;
            for (size_t j = 1; j < keys.size(); j++) {
                if (keys.at(j)[position + i] != ch) {
                    difference = true;
                    break;
                }
            }
            if (!difference) {
                substring.push_back(ch);
            } else {
                break;
            }
        }

        // Check if the node marks the end of a word; if yes, exclude the word from keys
        bool end_of_word = false;
        auto new_keys = std::vector<std::string>();
        if (substring.size() + position == keys.at(0).size()) {
            end_of_word = true;
            for (size_t i = 1; i < keys.size(); i++) {
                new_keys.push_back(keys[i]);
            }
        } else {
            new_keys = keys;
        }

        // Create new node depending on the length of the LCP
        TrieNode *new_node;
        if (substring.size() == 1) {
            new_node = new TrieNode();
            new_node->end_of_word_ = end_of_word;
        } else {
            new_node = new CompactNode(getKarpRabinFingerprint(substring), substring.size());
            new_node->end_of_word_ = end_of_word;
        }
        insert(new_node, position + substring.size(), new_keys);
        node->children_[current] = new_node;
    }

    uint8_t CompactTrie::getKarpRabinFingerprint(std::string& pattern) {
        uint64_t m = pattern.size();
        const uint16_t q = 251; // a prime number
        const uint16_t d = 256; // number of characters in the input alphabet;
        uint16_t h = 1;
        uint16_t p = 0; // hash value for pattern

        // The value of h is pow(d, m-1) mod q
        for (size_t i = 0; i < m; i++) {
            h = (h * d) % q;
        }

        // Calculate the hash value of pattern
        for (size_t i = 0; i < m; i++) {
            p = (d * p + pattern[i]) % q;
        }

        return p;
    }

    bool CompactTrie::lookupPrefix(std::string prefix) {
        return root->lookupNode(prefix, 0);
    }

    bool CompactTrie::TrieNode::lookupNode(std::string &key, uint64_t position) {
        if (position >= key.size()) {
            return true;
        }
        auto child_node_it = children_.find(key.at(position));
        if (child_node_it == children_.end()) {
            return false;
        }
        return child_node_it->second->lookupNode(key, position + 1);
    }

    bool CompactTrie::CompactNode::lookupNode(std::string &key, uint64_t position) {
        if (key.size() == 1) {
            return true;
        }

        if (position + length_ - 1 > key.length()) {
            // The remaining part of key is shorter than the length_ of the substring corresponding to this node
            // Hence, we have no way of checking if it's a valid prefix, so we have to return false
            // (we can only allow false positives)
            return true;
        }

        auto substring = key.substr(position - 1, length_);
        uint16_t fingerprint = getKarpRabinFingerprint(substring);
        if (fingerprint == fingerprint_) {
            if (position + length_ - 1 == key.length()) {
                return true;
            }
            auto child_node_it = children_.find(key.at(position + length_ - 1));
            if (child_node_it == children_.end()) {
                return false;
            }
            return child_node_it->second->lookupNode(key, position + length_);
        }

        return false;
    }

    uint64_t CompactTrie::getMemoryUsage() const {
        return sizeof(CompactTrie) + root->getMemoryUsage();
    }
}
