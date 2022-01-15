#include "gtest/gtest.h"
#include "Trie.hpp"

#define CHECK_FOR_FAILURES_MSG(statement, message)                                                 \
{                                                                                                  \
    SCOPED_TRACE(message);                                                                         \
    ASSERT_NO_FATAL_FAILURE((statement));                                                          \
}

#define CHECK_FOR_FAILURES(statement) CHECK_FOR_FAILURES_MSG(statement, " <--  line of failure\n")


namespace range_filtering {
namespace trie_test {

class TrieUnitTest : public ::testing::Test {
public:
    static void findKeyGreaterThan(Trie& trie, const std::string& key, bool inclusive, const std::string& expected_key);
};

    TEST_F(TrieUnitTest, emptyTrie) {
        auto keys = std::vector<std::string>();
        auto trie = Trie(keys);

        ASSERT_FALSE(trie.lookupPrefix("anything"));
        ASSERT_TRUE(trie.lookupPrefix(""));
        ASSERT_FALSE(trie.lookupKey("anything"));
        ASSERT_FALSE(trie.lookupKey(""));
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

        ASSERT_TRUE(trie.lookupPrefix("f"));
        ASSERT_TRUE(trie.lookupPrefix("fa"));
        ASSERT_FALSE(trie.lookupPrefix("fest"));
        ASSERT_FALSE(trie.lookupPrefix("faster"));
        ASSERT_TRUE(trie.lookupPrefix("fast"));
        ASSERT_TRUE(trie.lookupPrefix("trie"));
        ASSERT_FALSE(trie.lookupPrefix("tried"));

        ASSERT_TRUE(trie.lookupKey("f"));
        ASSERT_FALSE(trie.lookupKey("fa"));
        ASSERT_FALSE(trie.lookupKey("fest"));
        ASSERT_FALSE(trie.lookupKey("faster"));
        ASSERT_TRUE(trie.lookupKey("fast"));
        ASSERT_TRUE(trie.lookupKey("trie"));
        ASSERT_FALSE(trie.lookupKey("tried"));

        CHECK_FOR_FAILURES(findKeyGreaterThan(trie, "", true, "f"));
        CHECK_FOR_FAILURES(findKeyGreaterThan(trie, "fa", true, "far"));
        CHECK_FOR_FAILURES(findKeyGreaterThan(trie, "fast", true, "fast"));
        CHECK_FOR_FAILURES(findKeyGreaterThan(trie, "fare", true, "fast"));
        CHECK_FOR_FAILURES(findKeyGreaterThan(trie, "faster", true, "s"));
        CHECK_FOR_FAILURES(findKeyGreaterThan(trie, "trie", true, "trie"));
        CHECK_FOR_FAILURES(findKeyGreaterThan(trie, "tor", true, "toy"));
        CHECK_FOR_FAILURES(findKeyGreaterThan(trie, "grr", true, "s"));

        auto found_key_iter = trie.moveToKeyGreaterThan("tries", true);
        ASSERT_FALSE(found_key_iter.isValid());
        ASSERT_EQ(found_key_iter.getKey(), "");

    }

    void TrieUnitTest::findKeyGreaterThan(Trie& trie, const std::string& key, bool inclusive, const std::string& expected_key) {
        auto found_key_iter = trie.moveToKeyGreaterThan(key, inclusive);
        ASSERT_TRUE(found_key_iter.isValid());
        ASSERT_EQ(found_key_iter.getKey(), expected_key);
    }
}
}

int main (int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}