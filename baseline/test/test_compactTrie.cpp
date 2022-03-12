#include "gtest/gtest.h"

#include "CompactTrie.hpp"

namespace range_filtering {
namespace compact_trie_test {

class CompactTrieUnitTest : public ::testing::Test {

};

    TEST_F(CompactTrieUnitTest, emptyTrie) {
        auto keys = std::vector<std::string>();
        auto trie = CompactTrie(keys);

        ASSERT_FALSE(trie.lookupPrefix("anything"));
        ASSERT_TRUE(trie.lookupPrefix(""));
    }

    TEST_F(CompactTrieUnitTest, simpleTrie) {
        std::vector<std::string> keys = {
                "f",
                "far",
                "fast",
                "s",
                "top",
                "toy",
                "trie",
        };
        auto trie = CompactTrie(keys);

        ASSERT_TRUE(trie.lookupPrefix("f"));
        ASSERT_TRUE(trie.lookupPrefix("fa"));
        ASSERT_FALSE(trie.lookupPrefix("fest"));
        ASSERT_FALSE(trie.lookupPrefix("faster"));
        ASSERT_TRUE(trie.lookupPrefix("fast"));
        ASSERT_TRUE(trie.lookupPrefix("trie"));
        ASSERT_FALSE(trie.lookupPrefix("tried"));
        ASSERT_FALSE(trie.lookupPrefix("fase")); // should be able to determine based on fingerprint
        ASSERT_TRUE(trie.lookupPrefix(("fas")));
        ASSERT_TRUE(trie.lookupPrefix(("tri")));
        ASSERT_TRUE(trie.lookupPrefix("trr")); // false positive
        ASSERT_FALSE(trie.lookupPrefix("trri"));
    }

    TEST_F(CompactTrieUnitTest, compactTrie) {
        std::vector<std::string> keys = {
                "ba",
                "baba",
                "babca",
                "banana",
                "bananas"
        };
        auto trie = CompactTrie(keys);

        // true positives
        ASSERT_TRUE(trie.lookupPrefix("b"));
        ASSERT_TRUE(trie.lookupPrefix("ba"));
        ASSERT_TRUE(trie.lookupPrefix("bab"));
        ASSERT_TRUE(trie.lookupPrefix("baba"));
        ASSERT_TRUE(trie.lookupPrefix("babc"));
        ASSERT_TRUE(trie.lookupPrefix("babca"));
        ASSERT_TRUE(trie.lookupPrefix("ban"));
        ASSERT_TRUE(trie.lookupPrefix("bana"));
        ASSERT_TRUE(trie.lookupPrefix("banan"));
        ASSERT_TRUE(trie.lookupPrefix("banana"));
        ASSERT_TRUE(trie.lookupPrefix("bananas"));

        // false positives
        ASSERT_TRUE(trie.lookupPrefix("bann"));
        ASSERT_TRUE(trie.lookupPrefix("bannn"));

        // negatives
        ASSERT_FALSE(trie.lookupPrefix("c"));
        ASSERT_FALSE(trie.lookupPrefix("ca"));
        ASSERT_FALSE(trie.lookupPrefix("bb"));
        ASSERT_FALSE(trie.lookupPrefix("bannnn"));
        ASSERT_FALSE(trie.lookupPrefix("bannnns"));
    }

    TEST_F(CompactTrieUnitTest, duplicateKey) {
        std::vector<std::string> keys = {
                "ba",
                "baba",
                "babca",
                "banana",
                "banana",
                "bananas"
        };
        auto trie = CompactTrie(keys);

        // true positives
        ASSERT_TRUE(trie.lookupPrefix("b"));
        ASSERT_TRUE(trie.lookupPrefix("ba"));
        ASSERT_TRUE(trie.lookupPrefix("bab"));
        ASSERT_TRUE(trie.lookupPrefix("baba"));
        ASSERT_TRUE(trie.lookupPrefix("babc"));
        ASSERT_TRUE(trie.lookupPrefix("babca"));
        ASSERT_TRUE(trie.lookupPrefix("ban"));
        ASSERT_TRUE(trie.lookupPrefix("bana"));
        ASSERT_TRUE(trie.lookupPrefix("banan"));
        ASSERT_TRUE(trie.lookupPrefix("banana"));
        ASSERT_TRUE(trie.lookupPrefix("bananas"));

        // false positives
        ASSERT_TRUE(trie.lookupPrefix("bann"));
        ASSERT_TRUE(trie.lookupPrefix("bannn"));

        // negatives
        ASSERT_FALSE(trie.lookupPrefix("c"));
        ASSERT_FALSE(trie.lookupPrefix("ca"));
        ASSERT_FALSE(trie.lookupPrefix("bb"));
        ASSERT_FALSE(trie.lookupPrefix("bannnn"));
        ASSERT_FALSE(trie.lookupPrefix("bannnns"));
    }

} // namespace compact_trie_test
} // namespace range_filtering

int main (int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}