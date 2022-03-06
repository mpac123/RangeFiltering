#include "gtest/gtest.h"

#include "RestrainedSurfingTrie.h"

namespace range_filtering {
    namespace restrained_surfing_trie_test {

        class RestrainedSurfingTrieUnitTest : public ::testing::Test {

        };

        TEST_F(RestrainedSurfingTrieUnitTest, emptyTrie) {
            auto keys = std::vector<std::string>();
            auto trie = RestrainedSurfingTrie(keys, 0, RestraintType::absolute, 2, 0.0);

            ASSERT_FALSE(trie.lookupPrefix("anything"));
            ASSERT_TRUE(trie.lookupPrefix(""));
        }

        TEST_F(RestrainedSurfingTrieUnitTest, simpleTrie) {
            std::vector<std::string> keys = {
                    "f",
                    "far",
                    "fast",
                    "s",
                    "top",
                    "toy",
                    "trie",
            };
            auto trie = RestrainedSurfingTrie(keys, 0, RestraintType::absolute, 2, 0.0);

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
            ASSERT_TRUE(trie.lookupPrefix("trr"));
            ASSERT_TRUE(trie.lookupPrefix("trri"));

        }

        TEST_F(RestrainedSurfingTrieUnitTest, simpleTrieWithSuffix) {
            std::vector<std::string> keys = {
                    "f",
                    "far",
                    "fast",
                    "s",
                    "top",
                    "toy",
                    "trie",
            };
            auto trie = RestrainedSurfingTrie(keys, 4, RestraintType::absolute, 2, 0.0);

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

            // not false positives this time
            ASSERT_FALSE(trie.lookupPrefix("fase"));
            ASSERT_FALSE(trie.lookupPrefix("trr"));
            ASSERT_FALSE(trie.lookupPrefix("trri"));
        }

        TEST_F(RestrainedSurfingTrieUnitTest, absoluteConstraint) {
            std::vector<std::string> keys = {
                    "f",
                    "far",
                    "fast",
                    "s",
                    "top",
                    "toy",
                    "triangle",
                    "trickster",
                    "trie",
            };
            auto restrainedTrie = RestrainedSurfingTrie(keys, 0, RestraintType::absolute, 2, 0.0);
            auto relaxedTrie = RestrainedSurfingTrie(keys, 0, RestraintType::absolute, 4, 0.0);

            ASSERT_TRUE(restrainedTrie.lookupPrefix("triang"));
            ASSERT_FALSE(restrainedTrie.lookupPrefix("trianh"));
            ASSERT_TRUE(restrainedTrie.lookupPrefix("tricks"));
            ASSERT_FALSE(restrainedTrie.lookupPrefix("tricky"));

            ASSERT_TRUE(relaxedTrie.lookupPrefix("triang"));
            ASSERT_TRUE(relaxedTrie.lookupPrefix("trianh"));
            ASSERT_TRUE(relaxedTrie.lookupPrefix("tricks"));
            ASSERT_TRUE(relaxedTrie.lookupPrefix("tricky"));

            ASSERT_TRUE(relaxedTrie.getMemoryUsage() < restrainedTrie.getMemoryUsage());
        }

        TEST_F(RestrainedSurfingTrieUnitTest, relativeConstraint) {
            std::vector<std::string> keys = {
                    "f",
                    "far",
                    "fast",
                    "s",
                    "top",
                    "toy",
                    "triangle",
                    "trickster",
                    "trie",
            };
            auto restrainedTrie = RestrainedSurfingTrie(keys, 0, RestraintType::relative, 0, 0.2);
            auto relaxedTrie = RestrainedSurfingTrie(keys, 0, RestraintType::relative, 0, 0.5);

            ASSERT_TRUE(restrainedTrie.lookupPrefix("triang"));
            ASSERT_FALSE(restrainedTrie.lookupPrefix("trianh"));
            ASSERT_TRUE(restrainedTrie.lookupPrefix("tricks"));
            ASSERT_FALSE(restrainedTrie.lookupPrefix("tricky"));

            ASSERT_TRUE(relaxedTrie.lookupPrefix("triang"));
            ASSERT_TRUE(relaxedTrie.lookupPrefix("trianh"));
            ASSERT_TRUE(relaxedTrie.lookupPrefix("tricks"));
            ASSERT_TRUE(relaxedTrie.lookupPrefix("tricky"));

            ASSERT_TRUE(relaxedTrie.getMemoryUsage() < restrainedTrie.getMemoryUsage());
        }


    } // namespace restrained_surfing_trie_test
} // namespace range_filtering

int main (int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}