#include "gtest/gtest.h"
#include "MultiPrefixBloomFilter.h"

namespace range_filtering {
    namespace multi_prefix_bloom_filter_test {

    class MultiPrefixBloomFilterUnitTest : public ::testing::Test {

    };

        TEST_F(MultiPrefixBloomFilterUnitTest, emptyTrie) {
            auto keys = std::vector<std::string>();
            auto trie = MultiPrefixBloomFilter(keys, 8000);

            ASSERT_FALSE(trie.lookupPrefix("anything"));
            ASSERT_TRUE(trie.lookupPrefix(""));
        }

        TEST_F(MultiPrefixBloomFilterUnitTest, simpleTrie) {
            std::vector<std::string> keys = {
                    "f",
                    "far",
                    "fast",
                    "s",
                    "top",
                    "toy",
                    "trie",
            };
            auto trie = MultiPrefixBloomFilter(keys, 8000);

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

        TEST_F(MultiPrefixBloomFilterUnitTest, compactTrie) {
            std::vector<std::string> keys = {
                    "ba",
                    "baba",
                    "babca",
                    "banana",
                    "bananas"
            };
            auto trie = MultiPrefixBloomFilter(keys, 8000);

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

        TEST_F(MultiPrefixBloomFilterUnitTest, simpleTrieWithDoubting) {
            std::vector<std::string> keys = {
                    "f",
                    "far",
                    "fast",
                    "s",
                    "top",
                    "toy",
                    "trie",
            };
            auto trieWithFPsNoDoubting = MultiPrefixBloomFilter(keys, 50, 0);

            ASSERT_TRUE(trieWithFPsNoDoubting.lookupPrefix("f"));
            ASSERT_TRUE(trieWithFPsNoDoubting.lookupPrefix("fa"));

            // false positives
            ASSERT_TRUE(trieWithFPsNoDoubting.lookupPrefix("fest"));
            ASSERT_TRUE(trieWithFPsNoDoubting.lookupPrefix("fase"));
            ASSERT_TRUE(trieWithFPsNoDoubting.lookupPrefix("trri"));

            ASSERT_FALSE(trieWithFPsNoDoubting.lookupPrefix("faster"));
            ASSERT_TRUE(trieWithFPsNoDoubting.lookupPrefix("fast"));
            ASSERT_TRUE(trieWithFPsNoDoubting.lookupPrefix("trie"));
            ASSERT_FALSE(trieWithFPsNoDoubting.lookupPrefix("tried"));
            ASSERT_TRUE(trieWithFPsNoDoubting.lookupPrefix(("fas")));
            ASSERT_TRUE(trieWithFPsNoDoubting.lookupPrefix(("tri")));


            auto trieWithDoubting = MultiPrefixBloomFilter(keys, 50, 3);

            ASSERT_TRUE(trieWithDoubting.lookupPrefix("f"));
            ASSERT_TRUE(trieWithDoubting.lookupPrefix("fa"));

            // not false positives this time
            ASSERT_FALSE(trieWithDoubting.lookupPrefix("fest"));
            ASSERT_FALSE(trieWithDoubting.lookupPrefix("trri"));

            // still false positive as doubting doesn't help in this case
            ASSERT_TRUE(trieWithDoubting.lookupPrefix("fase"));

            // these shouldn't change
            ASSERT_FALSE(trieWithDoubting.lookupPrefix("faster"));
            ASSERT_TRUE(trieWithDoubting.lookupPrefix("fast"));
            ASSERT_TRUE(trieWithDoubting.lookupPrefix("trie"));
            ASSERT_FALSE(trieWithDoubting.lookupPrefix("tried"));
            ASSERT_TRUE(trieWithDoubting.lookupPrefix(("fas")));
            ASSERT_TRUE(trieWithDoubting.lookupPrefix(("tri")));
        }
    } // namespace multi_prefix_bloom_filter_test
} // namespace range_filtering

int main (int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}