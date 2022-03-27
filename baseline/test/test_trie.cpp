#include <fstream>
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

    static const std::string kFilePath = "/home/mapac/Coding/RangeFiltering/bench/workload-gen/workloads/100k_new/uniform_input.txt";
    static std::vector<std::string> words;

    class TrieUnitTest : public ::testing::Test {
public:
    static void findKeyGreaterThan(Trie& trie, const std::string& key, bool inclusive, const std::string& expected_key);
    static void doNotFindKeyGreaterThan(Trie& trie, const std::string& key, bool inclusive);
    static void findNext(Trie& trie, const std::string& key, const std::string& expected_key);
    static void doNotFindNext(Trie& trie, const std::string& key);
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
        CHECK_FOR_FAILURES(findKeyGreaterThan(trie, "fact", true, "far"));
        CHECK_FOR_FAILURES(doNotFindKeyGreaterThan(trie, "tries", true));

        CHECK_FOR_FAILURES(findKeyGreaterThan(trie, "", false, "f"));
        CHECK_FOR_FAILURES(findKeyGreaterThan(trie, "fa", false, "far"));
        CHECK_FOR_FAILURES(findKeyGreaterThan(trie, "fast", false, "s"));
        CHECK_FOR_FAILURES(findKeyGreaterThan(trie, "fare", false, "fast"));
        CHECK_FOR_FAILURES(findKeyGreaterThan(trie, "faster", false, "s"));
        CHECK_FOR_FAILURES(doNotFindKeyGreaterThan(trie, "trie", false));
        CHECK_FOR_FAILURES(findKeyGreaterThan(trie, "tor", false, "toy"));
        CHECK_FOR_FAILURES(findKeyGreaterThan(trie, "grr", false, "s"));
        CHECK_FOR_FAILURES(findKeyGreaterThan(trie, "fact", false, "far"));
        CHECK_FOR_FAILURES(doNotFindKeyGreaterThan(trie, "tries", false));

        CHECK_FOR_FAILURES(findNext(trie, "f", "far"));
        CHECK_FOR_FAILURES(findNext(trie, "far", "fast"));
        CHECK_FOR_FAILURES(findNext(trie, "fast", "s"));
        CHECK_FOR_FAILURES(findNext(trie, "s", "top"));
        CHECK_FOR_FAILURES(findNext(trie, "top", "toy"));
        CHECK_FOR_FAILURES(findNext(trie, "toy", "trie"));
        CHECK_FOR_FAILURES(doNotFindNext(trie, "trie"));

        ASSERT_TRUE(trie.lookupRange("fact", true, "fare", true));
        ASSERT_FALSE(trie.lookupRange("fare", true, "fase", true));
        ASSERT_TRUE(trie.lookupRange("far", true, "fase", true));
        ASSERT_FALSE(trie.lookupRange("far", false, "fase", true));
        ASSERT_TRUE(trie.lookupRange("fare", true, "fast", true));
        ASSERT_FALSE(trie.lookupRange("fare", true, "fast", false));
        ASSERT_FALSE(trie.lookupRange("fat", true, "o", false));
        ASSERT_TRUE(trie.lookupRange("fat", true, "so", false));

        ASSERT_FALSE(trie.lookupRange("zaatjyag", true, "zaatjyao", true));
        ASSERT_FALSE(trie.lookupRange("sas", true, "tap", true));
        ASSERT_FALSE(trie.lookupRange("topor", true, "tors", true));
        ASSERT_FALSE(trie.lookupRange("toyor", true, "tram", true));
    }

    TEST_F(TrieUnitTest, otherTrie) {
        std::vector<std::string> keys = {
                "f",
                "far",
                "fast",
                "s",
                "seseseseseseseesesese",
                "top",
                "toy",
                "toyyyyyyyyyy",
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
        ASSERT_TRUE(trie.lookupPrefix("sesesesesesese"));
    }

    TEST_F(TrieUnitTest, bigTrie) {
        std::vector<std::string> keys = {
                "f",
                "far",
                "fast",
                "s",
                "seseseseseseseesesese",
                "top",
                "toy",
                "toyyyyyyyyyy",
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
        ASSERT_TRUE(trie.lookupPrefix("sesesesesesese"));
    }

    TEST_F(TrieUnitTest, words) {
        auto trie = Trie(words);

        for (const auto& word : words) {
            for (size_t i = 0; i < word.size() - 1; i++) {
                ASSERT_TRUE(trie.lookupPrefix(word.substr(0, i + 1)));
            }
        }
    }

    void TrieUnitTest::findKeyGreaterThan(Trie& trie, const std::string& key, bool inclusive, const std::string& expected_key) {
        auto found_key_iter = trie.moveToKeyGreaterThan(key, inclusive);
        ASSERT_TRUE(found_key_iter.isValid());
        ASSERT_EQ(found_key_iter.getKey(), expected_key);
    }

    void TrieUnitTest::doNotFindKeyGreaterThan(Trie& trie, const std::string& key, bool inclusive) {
        auto found_key_iter = trie.moveToKeyGreaterThan(key, inclusive);
        ASSERT_FALSE(found_key_iter.isValid());
        ASSERT_EQ(found_key_iter.getKey(), "");
    }

    void TrieUnitTest::findNext(Trie& trie, const std::string& key, const std::string& expected_key) {
        auto key_iter = trie.moveToKeyGreaterThan(key, true);
        key_iter++;
        ASSERT_TRUE(key_iter.isValid());
        ASSERT_EQ(key_iter.getKey(), expected_key);
    }

    void TrieUnitTest::doNotFindNext(Trie& trie, const std::string& key) {
        auto key_iter = trie.moveToKeyGreaterThan(key, true);
        key_iter++;
        ASSERT_FALSE(key_iter.isValid());
        ASSERT_EQ(key_iter.getKey(), "");
    }

    void loadWordList() {
        std::ifstream infile(kFilePath);
        std::string key;
        int count = 0;
        while (infile.good()) {
            infile >> key;
            words.push_back(key);
            count++;
        }
    }
}
}

int main (int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    range_filtering::trie_test::loadWordList();
    return RUN_ALL_TESTS();
}