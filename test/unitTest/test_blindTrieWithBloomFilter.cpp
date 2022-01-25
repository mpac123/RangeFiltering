#include "gtest/gtest.h"
#include "BlindTrieWithBloomFilter.h"

namespace range_filtering {
namespace bloom_blind_trie_test {

class BlindTrieWithBloomFilterUnitTest : public ::testing::Test {

};

    TEST_F(BlindTrieWithBloomFilterUnitTest, emptyTrie) {
        auto keys = std::vector<std::string>();
        auto trie = BlindTrieWithBloomFilter(keys);

        ASSERT_FALSE(trie.lookupPrefix("anything"));
        ASSERT_TRUE(trie.lookupPrefix(""));
    }

    TEST_F(BlindTrieWithBloomFilterUnitTest, simpleTrie) {
        std::vector<std::string> keys = {
                "f",
                "far",
                "fast",
                "s",
                "top",
                "toy",
                "trie",
        };
        auto trie = BlindTrieWithBloomFilter(keys);

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
        ASSERT_FALSE(trie.lookupPrefix("trr")); // not a false positive this time
        ASSERT_FALSE(trie.lookupPrefix("trri"));
    }

    TEST_F(BlindTrieWithBloomFilterUnitTest, compactTrie) {
        std::vector<std::string> keys = {
                "ba",
                "baba",
                "babca",
                "banana",
                "bananas"
        };
        auto trie = BlindTrieWithBloomFilter(keys);

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

        // can be false positives if there's a collision in Bloom Filter
        ASSERT_FALSE(trie.lookupPrefix("bann"));
        ASSERT_FALSE(trie.lookupPrefix("bannn"));

        // negatives
        ASSERT_FALSE(trie.lookupPrefix("c"));
        ASSERT_FALSE(trie.lookupPrefix("ca"));
        ASSERT_FALSE(trie.lookupPrefix("bb"));
        ASSERT_FALSE(trie.lookupPrefix("bannnn"));
        ASSERT_FALSE(trie.lookupPrefix("bannnns"));
    }
} // namespace bloom_blind_trie
} // namespace range_filtering

int main (int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}