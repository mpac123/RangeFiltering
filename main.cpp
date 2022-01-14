#include <iostream>
#include <vector>
#include "include/Trie.hpp"

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
    if (trie->lookup(key)) {
        std::cout << "Key found" << std::endl;
    } else {
        std::cout << "Key not found" << std::endl;
    }

    key = "toy";
    if (trie->lookup(key)) {
        std::cout << "Key found" << std::endl;
    } else {
        std::cout << "Key not found" << std::endl;
    }
    return 0;
}
