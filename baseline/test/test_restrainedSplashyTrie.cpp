#include "gtest/gtest.h"

#include "RestrainedSplashyTrie.h"

namespace range_filtering {
    namespace restrained_splashy_trie_test {

        class RestrainedSplashyTrieUnitTest : public ::testing::Test {

        };

        TEST_F(RestrainedSplashyTrieUnitTest, emptyTrie) {
            auto keys = std::vector<std::string>();
            auto trie = RestrainedSplashyTrie(keys, 0, 0.75, RestraintType::absolute, 2, 0.0);

            ASSERT_FALSE(trie.lookupPrefix("anything"));
            ASSERT_TRUE(trie.lookupPrefix(""));
        }

        TEST_F(RestrainedSplashyTrieUnitTest, simpleTrie) {
            std::vector<std::string> keys = {
                    "f",
                    "far",
                    "fast",
                    "s",
                    "top",
                    "toy",
                    "trie",
            };
            auto trie = RestrainedSplashyTrie(keys, 0, 0.75, RestraintType::absolute, 2, 0.0);

            ASSERT_TRUE(trie.lookupPrefix("f"));
            ASSERT_TRUE(trie.lookupPrefix("fa"));
            ASSERT_TRUE(trie.lookupPrefix("fast"));
            ASSERT_TRUE(trie.lookupPrefix("trie"));

            ASSERT_TRUE(trie.lookupPrefix(("fas")));
            ASSERT_TRUE(trie.lookupPrefix(("tri")));

            // false positives
            ASSERT_TRUE(trie.lookupPrefix("tried"));
            ASSERT_TRUE(trie.lookupPrefix("tries"));
            ASSERT_TRUE(trie.lookupPrefix("tria"));
            ASSERT_TRUE(trie.lookupPrefix("faster"));
            ASSERT_TRUE(trie.lookupPrefix("fase"));
            ASSERT_TRUE(trie.lookupPrefix("trr"));
            ASSERT_TRUE(trie.lookupPrefix("trri"));
            ASSERT_TRUE(trie.lookupPrefix("fest"));
        }

        TEST_F(RestrainedSplashyTrieUnitTest, simpleTrieWithSuffix) {
            std::vector<std::string> keys = {
                    "f",
                    "far",
                    "fast",
                    "s",
                    "top",
                    "toy",
                    "trie",
            };
            auto trie = RestrainedSplashyTrie(keys, 6, 0.75, RestraintType::absolute, 2, 0.0);

            ASSERT_TRUE(trie.lookupPrefix("f"));
            ASSERT_TRUE(trie.lookupPrefix("fa"));
            ASSERT_FALSE(trie.lookupPrefix("fest"));
            ASSERT_TRUE(trie.lookupPrefix("fast"));
            ASSERT_TRUE(trie.lookupPrefix("trie"));

            ASSERT_TRUE(trie.lookupPrefix(("fas")));
            ASSERT_TRUE(trie.lookupPrefix(("tri")));

            // false positives
            ASSERT_TRUE(trie.lookupPrefix("tried"));
            ASSERT_TRUE(trie.lookupPrefix("tries"));
            ASSERT_TRUE(trie.lookupPrefix("tria"));
            ASSERT_TRUE(trie.lookupPrefix("faster"));
            ASSERT_TRUE(trie.lookupPrefix("fase"));

            // not false positives this time

            ASSERT_FALSE(trie.lookupPrefix("trr"));
            ASSERT_FALSE(trie.lookupPrefix("trri"));
        }


    } // namespace surfing_trie_test
} // namespace range_filtering

int main (int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}