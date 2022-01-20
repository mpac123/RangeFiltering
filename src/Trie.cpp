#include <Trie.hpp>

namespace range_filtering {

Trie::TrieNode::TrieNode() {
    end_of_word = false;
    parent = nullptr;
}

Trie::TrieNode::TrieNode(TrieNode* _parent) {
    end_of_word = false;
    parent = _parent;
}

Trie::Trie(std::vector<std::string> &keys) {
    root = new TrieNode();
    insert(root, 0, keys);
}

void Trie::insert(TrieNode *node, uint64_t position, std::vector<std::string> &keys) {
    char current = '\0';
    std::map<char, TrieNode*> children = std::map<char, TrieNode*>();

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
        } else if (current != key.at(position)) {
            // Create new node and call insert recursively on it
            auto new_node = new TrieNode(node);
            insert(new_node, position + 1, keys_with_common_prefix);
            children[current] = new_node;
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
        children[current] = new_node;
    }

    // Save children keys and children nodes
    node->childrenKeys = std::vector<char>(children.size());
    node->childrenNodes = std::vector<TrieNode*>(children.size());
    size_t i = 0;
    for (auto child : children) {
        node->childrenKeys[i] = child.first;
        node->childrenNodes[i] = child.second;
        i++;
    }
    children.clear();
    keys_with_common_prefix.clear();
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
    auto child_pos = node->findKeyPosition(key.at(position));
    if (child_pos == -1) {
        return false;
    }
    return (lookupNode(key, position + 1, node->childrenNodes[child_pos], exact));
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
    // If node has children_, find the left-most key
    if (current_node_->childrenKeys.size() > 0) {
        key_len_++;
        key_.push_back(current_node_->childrenKeys[0]);
        current_node_ = current_node_->childrenNodes[0];
        findLeftMostKey();
        return is_valid_;
    }

    // Otherwise, backtrack until parent node has children_ on right
    findLeftMostKeyOfParentWithChildOnRight();
    return is_valid_;
}

Trie::Iter Trie::moveToKeyGreaterThan(const std::string& key, const bool inclusive) {
    Trie::Iter iter(this);
    moveDownTheNodeToKeyGreaterThan(key, 0, iter);
    bool exact = iter.getKey() == key;
    if (exact && !inclusive) {
        iter++;
    }
    return iter;
}

void Trie::moveDownTheNodeToKeyGreaterThan(const std::string &key, uint64_t position, Trie::Iter& iter) {
    if (position >= key.size()) {
        iter.findLeftMostKey();
        return;
    }

    auto child_pos = iter.current_node_->findKeyPosition(key.at(position));
    if (child_pos == -1) {
        // Find child with key greater than current
        auto next_node_pos = iter.current_node_->findKeyUpperBoundPosition(key.at(position));
        //if (next_node_pos < (long int) iter.current_node_->childrenKeys.size()) {
        if (next_node_pos != -1) {
            iter.key_len_++;
            iter.key_.push_back(iter.current_node_->childrenKeys[next_node_pos]);
            iter.current_node_ = iter.current_node_->childrenNodes[next_node_pos];
            iter.findLeftMostKey();
            return;
        }

        // Otherwise, backtrack until node has child greater than the node visited before
        iter.findLeftMostKeyOfParentWithChildOnRight();
        return;
    }

    iter.key_len_++;
    iter.key_.push_back(iter.current_node_->childrenKeys[child_pos]);
    iter.current_node_ = iter.current_node_->childrenNodes[child_pos];
    moveDownTheNodeToKeyGreaterThan(key, position + 1, iter);
}

void Trie::Iter::findLeftMostKey() {
    if (current_node_->end_of_word) {
        is_valid_ = true;
        return;
    }

    key_len_++;
    key_.push_back(current_node_->childrenKeys[0]);
    current_node_ = current_node_->childrenNodes[0];
    findLeftMostKey();
}

void Trie::Iter::findLeftMostKeyOfParentWithChildOnRight() {
    auto parent = current_node_->parent;
    if (parent == nullptr) {
        is_valid_ = false;
        return;
    }

    auto previous_key = key_.at(key_len_ - 1);
    key_.pop_back();
    auto next_node_pos = parent->findKeyUpperBoundPosition(previous_key);
    if (next_node_pos == -1) {
        key_len_--;
        current_node_ = parent;
        findLeftMostKeyOfParentWithChildOnRight();
        return;
    }

    key_.push_back(parent->childrenKeys[next_node_pos]);
    current_node_ = parent->childrenNodes[next_node_pos];
    findLeftMostKey();
}

bool Trie::lookupRange(const std::string& left_key, const bool left_inclusive,
                 const std::string& right_key, const bool right_inclusive) {
    auto left_key_iter = moveToKeyGreaterThan(left_key, left_inclusive);
    auto compare_left = left_key_iter.compare(left_key);
    if (compare_left == 0) {
        return true;
    }
    auto compare_right = left_key_iter.compare(right_key);
    if (compare_right < 0) {
        return true;
    }
    if (right_inclusive && compare_right == 0) {
        return true;
    }
    return false;
}

int64_t Trie::TrieNode::findKeyPosition(char key) {
    return binarySearch(0, childrenKeys.size(), key, true);
}

int64_t Trie::TrieNode::findKeyUpperBoundPosition(char key) {
    size_t position = binarySearch(0, childrenKeys.size(), key, false);
    if (position >= childrenKeys.size()) return -1;
    if (childrenKeys[position] > key) return position;
    if (position + 1 >= childrenKeys.size()) return -1;
    return position + 1;
}

int64_t Trie::TrieNode::binarySearch(size_t left, size_t right, char key, bool exact) {
    if (left >= right) {
        if (exact) return -1;
        return left;
    }
    int middle = (int) (left + right) / 2;
    if (childrenKeys[middle] == key) return middle;
    if (childrenKeys[middle] > key) return binarySearch(left, middle, key, exact);
    return binarySearch(middle + 1, right, key, exact);
}

uint64_t Trie::TrieNode::getMemoryUsage() const {
    uint64_t children_size = 0;
    for (size_t i = 0; i < childrenKeys.size(); i++) {
        children_size += childrenNodes[i]->getMemoryUsage();
    }
    return sizeof(Trie::TrieNode) + children_size;
}

uint64_t Trie::getMemoryUsage() const {
    return sizeof(Trie) + root->getMemoryUsage();
}

} // namespace range_filtering

