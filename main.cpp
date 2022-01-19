#include <iostream>
#include <vector>
#include "include/Trie.hpp"
#include "include/CompactTrie.hpp"

using namespace range_filtering;

int main() {
    std::vector<std::string> keys = {
            "f",
            "far",
            "fast",
            "s",
            "top",
            "toy",
            "trie",
    };
    Trie* trie = new Trie(keys);

    std::string key = "fase";
    if (trie->lookupKey(key)) {
        std::cout << "Key found" << std::endl;
    } else {
        std::cout << "Key not found" << std::endl;
    }

    key = "toy";
    if (trie->lookupKey(key)) {
        std::cout << "Key found" << std::endl;
    } else {
        std::cout << "Key not found" << std::endl;
    }

    if (trie->lookupRange("fare", true, "fase", true)) {
        std::cout << "Range contains keys" << std::endl;
    } else {
        std::cout << "Range empty" << std::endl;
    }

    if (trie->lookupRange("fare", true, "faster", true)) {
        std::cout << "Range contains keys" << std::endl;
    } else {
        std::cout << "Range empty" << std::endl;
    }

    std::cout << "Memory usage: " << trie->getMemoryUsage() << std::endl;

    CompactTrie* compactTrie = new CompactTrie(keys);

    key = "fase";
    if (compactTrie->lookupPrefix(key)) {
        std::cout << "Key found" << std::endl;
    } else {
        std::cout << "Key not found" << std::endl;
    }

    key = "toy";
    if (compactTrie->lookupPrefix(key)) {
        std::cout << "Key found" << std::endl;
    } else {
        std::cout << "Key not found" << std::endl;
    }

    std::cout << "Memory usage: " << compactTrie->getMemoryUsage() << std::endl;

    return 0;
}