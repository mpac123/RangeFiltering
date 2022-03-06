#include "gtest/gtest.h"
#include "BloomedSurfingTrie.h"
#include "SurfingTrie.h"

namespace range_filtering {
    namespace bloomed_surfing_trie_test {

    class BloomedSurfingTrieUnitTest : public ::testing::Test {

    };

        TEST_F(BloomedSurfingTrieUnitTest, emptyTrie) {
            auto keys = std::vector<std::string>();
            auto trie = BloomedSurfingTrie(keys, 0, 8000, 0.5);

            ASSERT_FALSE(trie.lookupPrefix("anything"));
            ASSERT_TRUE(trie.lookupPrefix(""));
        }

        TEST_F(BloomedSurfingTrieUnitTest, simpleTrie) {
            std::vector<std::string> keys = {
                    "f",
                    "far",
                    "fast",
                    "s",
                    "top",
                    "toy",
                    "trie",
            };
            auto trie = BloomedSurfingTrie(keys, 0, 8000, 0.5);
            auto surfingTrie = SurfingTrie(keys, 0);

            ASSERT_TRUE(trie.lookupPrefix("f"));
            ASSERT_TRUE(trie.lookupPrefix("fa"));
            ASSERT_FALSE(trie.lookupPrefix("fest"));
            ASSERT_TRUE(trie.lookupPrefix("fast"));
            ASSERT_TRUE(trie.lookupPrefix("trie"));

            ASSERT_TRUE(trie.lookupPrefix(("fas")));
            ASSERT_TRUE(trie.lookupPrefix(("tri")));

            // not false positives with BF!
            ASSERT_FALSE(trie.lookupPrefix("tried"));
            ASSERT_FALSE(trie.lookupPrefix("tries"));
            ASSERT_FALSE(trie.lookupPrefix("tria"));
            ASSERT_FALSE(trie.lookupPrefix("faster"));
            ASSERT_FALSE(trie.lookupPrefix("fase"));
            ASSERT_FALSE(trie.lookupPrefix("trr"));
            ASSERT_FALSE(trie.lookupPrefix("trri"));

            ASSERT_TRUE(surfingTrie.getMemoryUsage() < trie.getMemoryUsage());
        }

    } // namespace bloomed_surfing_trie_test
} // namespace range_filtering

int main (int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}