#include "gtest/gtest.h"
#include "Trie.hpp"

namespace range_filtering {
namespace trie_test {

class TrieUnitTest : public ::testing::Test {
public:
    virtual void SetUp() {

    }
};

    TEST_F(TrieUnitTest, emptyTrie) {
        auto keys = std::vector<std::string>();
        auto trie = Trie(keys);

        ASSERT_FALSE(trie.lookup("anything"));
        ASSERT_TRUE(trie.lookup(""));
    }

    TEST_F(TrieUnitTest, simpleTrie) {
        std::vector<std::string> keys = {
                "f",
                "far",
                "fast",
                "s",
                "top",
                "toy",
                "trie",
        };
        auto trie = Trie(keys);

        ASSERT_TRUE(trie.lookup("f"));
        ASSERT_TRUE(trie.lookup("fa"));
        ASSERT_FALSE(trie.lookup("fest"));
        ASSERT_FALSE(trie.lookup("faster"));
        ASSERT_TRUE(trie.lookup("fast"));
        ASSERT_TRUE(trie.lookup("trie"));
        ASSERT_FALSE(trie.lookup("tried"));
    }
}
}

int main (int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}