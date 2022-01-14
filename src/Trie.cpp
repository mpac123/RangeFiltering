#include <Trie.hpp>

namespace range_filtering {
Trie::TrieNode::TrieNode() {
    end_of_word = false;
    children = std::unordered_map<char, TrieNode *>();
}

// Keys must be sorted
Trie::Trie(std::vector<std::string> &keys) {
    root = new TrieNode();
    insert(root, 0, keys);
}

void Trie::insert(TrieNode *node, uint64_t position, std::vector<std::string> &keys) {
    char current = '\0';

    if (keys.empty()) {
        return;
    }

    std::vector<std::string> keys_with_common_prefix = std::vector<std::string>();

    for (auto key: keys) {
        // Mark end of word if key is shorter than current position
        if (position >= key.size()) {
            node ->end_of_word = true;
            continue;
        }
        // Otherwise, continue
        if (current == '\0') {
            current = key.at(position);
            keys_with_common_prefix.push_back(key);
        } else if (current != '\0' && current != key.at(position)) {
            // Create new node and call insert recursively on it
            auto new_node = new TrieNode();
            insert(new_node, position + 1, keys_with_common_prefix);
            node->children.emplace(current, new_node);
            // Reset current and clean keys_with_common_prefix
            current = key.at(position);
            keys_with_common_prefix = std::vector<std::string>();
            keys_with_common_prefix.push_back(key);
        } else {
            keys_with_common_prefix.push_back(key);
        }
    }

    if (current != '\0') {
        auto new_node = new TrieNode();
        insert(new_node, position + 1, keys_with_common_prefix);
        node->children.emplace(current, new_node);
    }

}

bool Trie::lookup(std::string key) {
    return lookupNode(key, 0, root);
}

bool Trie::lookupNode(std::string &key, uint64_t position, TrieNode *node) {
    if (position >= key.size()) {
        return true;
    }
    auto child_node_it = node->children.find(key.at(position));
    if (child_node_it == node->children.end()) {
        return false;
    }
    return (lookupNode(key, position + 1, child_node_it->second));
}

} // namespace range_filtering

