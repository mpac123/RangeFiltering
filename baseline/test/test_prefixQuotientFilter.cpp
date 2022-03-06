#include "gtest/gtest.h"
#include "PrefixQuotientFilter.h"

namespace range_filtering {
namespace prefix_quotient_filter_test {

class PrefixQuotientFilterUnitTest : public ::testing::Test {};

    TEST_F(PrefixQuotientFilterUnitTest, emptyTrie) {
        auto keys = std::vector<std::string>();
        auto trie = PrefixQuotientFilter(keys, 5);

        ASSERT_FALSE(trie.lookupPrefix("anything"));
        ASSERT_TRUE(trie.lookupPrefix(""));
    }

    TEST_F(PrefixQuotientFilterUnitTest, simpleTrie) {
        std::vector<std::string> keys = {
                "f",
                "far",
                "fast",
                "s",
                "top",
                "toy",
                "trie",
        };
        auto trie = PrefixQuotientFilter(keys, 6);

        ASSERT_FALSE(trie.hasFailed());

        ASSERT_TRUE(trie.lookupPrefix("f"));
        ASSERT_TRUE(trie.lookupPrefix("fa"));
        ASSERT_FALSE(trie.lookupPrefix("fest"));
        ASSERT_FALSE(trie.lookupPrefix("faster"));
        ASSERT_TRUE(trie.lookupPrefix("fast"));
        ASSERT_TRUE(trie.lookupPrefix("trie"));
        ASSERT_FALSE(trie.lookupPrefix("tried"));
        ASSERT_FALSE(trie.lookupPrefix("fase"));
        ASSERT_TRUE(trie.lookupPrefix(("fas")));
        ASSERT_TRUE(trie.lookupPrefix(("tri")));
        ASSERT_FALSE(trie.lookupPrefix("trr")); // not a false positive this time
        ASSERT_FALSE(trie.lookupPrefix("trri")); // false positive
    }

    TEST_F(PrefixQuotientFilterUnitTest, compactTrie) {
        std::vector<std::string> keys = {
                "ba",
                "baba",
                "babca",
                "banana",
                "bananas",
                "bananass"
        };
        auto trie = PrefixQuotientFilter(keys, 5);

        ASSERT_FALSE(trie.hasFailed());

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

        ASSERT_FALSE(trie.lookupPrefix("bann"));
        ASSERT_FALSE(trie.lookupPrefix("bannn"));
        ASSERT_FALSE(trie.lookupPrefix("c"));
        ASSERT_FALSE(trie.lookupPrefix("ca"));
        ASSERT_FALSE(trie.lookupPrefix("bb"));
        ASSERT_FALSE(trie.lookupPrefix("bannnn"));
        ASSERT_FALSE(trie.lookupPrefix("bannnns"));
    }
} // namespace prefix_bloom_filter_test
} // namespace range_filtering

int main (int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}