#include <Trie.hpp>

namespace range_filtering {

Trie::TrieNode::TrieNode() {
    end_of_word = false;
    children = std::map<char, TrieNode *>();
    parent = nullptr;
}

Trie::TrieNode::TrieNode(TrieNode* _parent) {
    end_of_word = false;
    children = std::map<char, TrieNode *>();
    parent = _parent;
}

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
            auto new_node = new TrieNode(node);
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
        auto new_node = new TrieNode(node);
        insert(new_node, position + 1, keys_with_common_prefix);
        node->children.emplace(current, new_node);
    }

}

bool Trie::lookupKey(std::string key) {
    return lookupNode(key, 0, root, true);
}

bool Trie::lookupPrefix(std::string prefix) {
    return lookupNode(prefix, 0, root, false);
}

bool Trie::lookupNode(std::string &key, uint64_t position, TrieNode *node, bool exact) {
    if (position >= key.size()) {
        return !exact || node->end_of_word;
    }
    auto child_node_it = node->children.find(key.at(position));
    if (child_node_it == node->children.end()) {
        return false;
    }
    return (lookupNode(key, position + 1, child_node_it->second, exact));
}

Trie::Iter::Iter(const Trie *trie) {
    current_node_ = trie->root;
    trie_root_ = trie->root;
    key_len_ = 0;
}

void Trie::Iter::clear() {
    current_node_ = trie_root_;
    key_len_ = 0;
}

int Trie::Iter::compare(const std::string &key) const {
    auto iter_key = getKey();
    return iter_key.compare(key);
}

std::string Trie::Iter::getKey() const {
    if (!isValid()) {
        return std::string{};
    }
    return std::string{(const char*)key_.data(), (size_t)key_len_};
}

bool Trie::Iter::operator++(int) {
    // TODO
}

Trie::Iter Trie::moveToKeyGreaterThan(const std::string& key, const bool inclusive) {
    // TODO: if not inclusive but the answer is exact, move to the next word
    Trie::Iter iter(this);
    moveDownTheNodeToKeyGreaterThan(key, 0, iter);
    return iter;
}

void Trie::moveDownTheNodeToKeyGreaterThan(const std::string &key, uint64_t position, Trie::Iter& iter) {
    if (position >= key.size()) {
        traverseDownLeftmostNodes(iter);
        return;
    }

    auto child_node_it = iter.current_node_->children.find(key.at(position));
    if (child_node_it == iter.current_node_->children.end()) {
        // Find child with key greater than current
        auto next_node_it = iter.current_node_->children.upper_bound(key.at(position));
        if (next_node_it != iter.current_node_->children.end()) {
            iter.key_len_++;
            iter.key_.push_back(next_node_it->first);
            iter.current_node_ = next_node_it->second;
            traverseDownLeftmostNodes(iter);
            return;
        }

        // Otherwise, backtrack until node has child greater than the node visited before
        backtrackToFindNextValue(iter);
        return;
    }

    iter.key_len_++;
    iter.key_.push_back(child_node_it->first);
    iter.current_node_ = child_node_it->second;
    moveDownTheNodeToKeyGreaterThan(key, position + 1, iter);
}

void Trie::traverseDownLeftmostNodes(Trie::Iter& iter) {
    if (iter.current_node_->end_of_word) {
        iter.is_valid_ = true;
        return;
    }

    auto leftMostKeyIter = iter.current_node_->children.begin();
    iter.key_len_++;
    iter.key_.push_back(leftMostKeyIter->first);
    iter.current_node_ = leftMostKeyIter->second;
    traverseDownLeftmostNodes(iter);
}

void Trie::backtrackToFindNextValue(Trie::Iter& iter) {
    auto parent = iter.current_node_->parent;
    if (parent == nullptr) {
        iter.is_valid_ = false;
        return;
    }

    auto previous_key = iter.key_.at(iter.key_len_ - 1);
    iter.key_.pop_back();
    auto next_node_iter = parent->children.upper_bound(previous_key);
    if (next_node_iter == parent->children.end()) {
        iter.key_len_--;
        iter.current_node_ = parent;
        backtrackToFindNextValue(iter);
        return;
    }

    iter.key_.push_back(next_node_iter->first);
    iter.current_node_ = next_node_iter->second;
    traverseDownLeftmostNodes(iter);
}

bool Trie::lookupRange(const std::string& left_key, const bool left_inclusive,
                 const std::string& right_key, const bool right_inclusive) {
    // TODO
}

} // namespace range_filtering

