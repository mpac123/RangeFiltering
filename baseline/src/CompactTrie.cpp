#include <CompactTrie.hpp>

namespace range_filtering {

    CompactTrie::TrieNode::TrieNode() {
        end_of_word_ = false;
        children_ = std::map<char, TrieNode *>();
    }

    unsigned long long CompactTrie::TrieNode::getMemoryUsage() const {
        return sizeof(CompactTrie::TrieNode) + getChildrenMemoryUsage();
    }

    unsigned long long CompactTrie::TrieNode::getChildrenMemoryUsage() const {
        uint64_t children_size = 0;
        for (auto child : children_) {
            children_size += child.second->getMemoryUsage();
        }
        return children_size;
    }

    unsigned long long CompactTrie::CompactNode::getMemoryUsage() const {
        return sizeof(CompactTrie::TrieNode) + getChildrenMemoryUsage();
    }

    CompactTrie::CompactNode::CompactNode(uint64_t fingerprint, uint64_t length) : TrieNode() {
        fingerprint_ = fingerprint;
        length_ = length;
    }

    CompactTrie::CompactTrie(std::vector<std::string> &keys) {
        root = new TrieNode();
        root->insertKeys(0, keys);
    }

    void CompactTrie::TrieNode::insertKeys(uint64_t position, std::vector<std::string> &keys) {
        if (keys.empty()) {
            return;
        }

        char current = '\0';

        std::vector<std::string> keys_with_common_prefix = std::vector<std::string>();

        for (auto key : keys) {
            if (position >= key.size()) {
                end_of_word_ = true;
                // Create a new node and call insert recursively
                insertChildNode(current, position, keys_with_common_prefix);
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

    void CompactTrie::TrieNode::insertChildNode(char current, uint64_t position,
                                 std::vector<std::string> &keys) {
        // If only one key provided, add a leaf
        if (keys.size() == 1) {
            TrieNode *new_node;
            if (position + 1 == keys.at(0).size()) {
                new_node = new TrieNode();
                end_of_word_ = true;
            } else {
                auto substring = keys.at(0).substr(position);
                new_node = new CompactNode(KarpRabinFingerprint::generate_8bit(substring), keys.at(0).size() - position);
                end_of_word_ = true;
            }
            children_[current] = new_node;
            return;
        }

        // Otherwise, find longest common prefix
        std::string substring;
        substring.push_back(current);

        for (size_t i = position + 1; i < keys.at(0).size(); i++) {
            char ch = keys.at(0)[i];
            bool difference = false;
            for (size_t j = 1; j < keys.size(); j++) {
                if (keys.at(j)[i] != ch) {
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
                if (keys.at(i).size() != keys.at(0).size()) {
                    new_keys.push_back(keys[i]);
                }
            }
            if (new_keys.empty()) {
                TrieNode *new_node;
                if (position + 1 == keys.at(0).size()) {
                    new_node = new TrieNode();
                    end_of_word_ = true;
                } else {
                    new_node = new CompactNode(KarpRabinFingerprint::generate_8bit(substring), keys.at(0).size() - position);
                    end_of_word_ = true;
                }
                children_[current] = new_node;
                return;
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
            new_node = new CompactNode(KarpRabinFingerprint::generate_8bit(substring), substring.size());
            new_node->end_of_word_ = end_of_word;
        }
        new_node->insertKeys(position + substring.size(), new_keys);
        children_[current] = new_node;
    }

    bool CompactTrie::lookupPrefix(const std::string &prefix) {
        return root->lookupNode(prefix, 0);
    }

    bool CompactTrie::TrieNode::lookupNode(const std::string &key, uint64_t position) {
        if (position >= key.size()) {
            return true;
        }
        auto child_node_it = children_.find(key.at(position));
        if (child_node_it == children_.end()) {
            return false;
        }
        return child_node_it->second->lookupNode(key, position + 1);
    }

    bool CompactTrie::CompactNode::lookupNode(const std::string &key, uint64_t position) {
        if (position == key.length()) {
            return true;
        }

        if (position + length_ - 1 > key.length()) {
            // The remaining part of key is shorter than the length_ of the substring corresponding to this node
            // Hence, we have no way of checking if it's a valid prefix, so we have to return false
            // (we can only allow false positives)
            return true;
        }

        auto substring = key.substr(position - 1, length_);
        uint16_t fingerprint = KarpRabinFingerprint::generate_8bit(substring);
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
