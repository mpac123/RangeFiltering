#include "gtest/gtest.h"

#include "RangeBFKRNoEncoding.hpp"
#include <fstream>


namespace range_filtering {
    namespace range_bf_kr_no_encoding_test {
        static const std::string kFilePath = "/home/mapac/Coding/RangeFiltering/succinct_trie/test/words.txt";
        static std::vector<std::string> words;
        static const int kWordTestSize = 234369;
        class RangeBfKRNoEncodingUnitTest : public ::testing::Test {

        };

        TEST_F(RangeBfKRNoEncodingUnitTest, emptyKeysSet) {
            auto keys = std::vector<std::string>();
            auto filter = RangeBFKRNoEncoding(keys, 5000);

            ASSERT_FALSE(filter.lookupPrefix("anything"));
            ASSERT_TRUE(filter.lookupPrefix(""));
            ASSERT_FALSE(filter.lookupRange("anything", "anything2"));
            ASSERT_FALSE(filter.lookupRange("a", "abc"));
        }

        TEST_F(RangeBfKRNoEncodingUnitTest, simpleKeysSet) {
            std::vector<std::string> keys = {
                    "f",
                    "far",
                    "fast",
                    "s",
                    "top",
                    "toy",
                    "trie",
            };
            auto trie = RangeBFKRNoEncoding(keys, 50000);

            ASSERT_TRUE(trie.lookupPrefix("f"));
            ASSERT_TRUE(trie.lookupPrefix("fa"));
            ASSERT_FALSE(trie.lookupPrefix("fest"));
            ASSERT_TRUE(trie.lookupPrefix("fast"));
            ASSERT_TRUE(trie.lookupPrefix("trie"));

            ASSERT_TRUE(trie.lookupPrefix(("fas")));
            ASSERT_TRUE(trie.lookupPrefix(("tri")));

            ASSERT_TRUE(trie.lookupRange("fan", "fat"));
            ASSERT_TRUE(trie.lookupRange("fanas", "fatter"));
            ASSERT_TRUE(trie.lookupRange("fart", "faster"));

            ASSERT_FALSE(trie.lookupRange("abc", "ade"));
            ASSERT_FALSE(trie.lookupRange("triangle", "trick"));

            ASSERT_FALSE(trie.lookupRange("a", "a"));
            ASSERT_FALSE(trie.lookupRange("a", "b"));
            ASSERT_FALSE(trie.lookupRange("fan", "fans"));
            ASSERT_TRUE(trie.lookupRange("f", "f"));
            ASSERT_TRUE(trie.lookupRange("f", "fa"));
            ASSERT_TRUE(trie.lookupRange("f", "fast"));
            ASSERT_TRUE(trie.lookupRange("t", "top"));
            ASSERT_TRUE(trie.lookupRange("t", "tor"));
            ASSERT_TRUE(trie.lookupRange("e", "fa"));
            ASSERT_TRUE(trie.lookupRange("s", "sa"));
            ASSERT_TRUE(trie.lookupRange("r", "sa"));
            ASSERT_TRUE(trie.lookupRange("rabarb", "sa"));
            ASSERT_TRUE(trie.lookupRange("t", "tr"));
            ASSERT_TRUE(trie.lookupRange("t", "tram"));
            ASSERT_TRUE(trie.lookupRange("triangle", "tries"));
            ASSERT_FALSE(trie.lookupRange("toz", "tozs"));
            ASSERT_FALSE(trie.lookupRange("toys", "toysRS"));
            ASSERT_FALSE(trie.lookupRange("toz", "tozsRS"));
            ASSERT_FALSE(trie.lookupRange("triangle", "triangles"));
        }

        TEST_F (RangeBfKRNoEncodingUnitTest, lookupRangeWordTest) {
            auto rosetta = new RangeBFKRNoEncoding(words, 100000);
            bool exist = rosetta->lookupRange(std::string("\1"), words[0]);
            ASSERT_TRUE(exist);

            for (unsigned i = 0; i < words.size() - 1; i++) {
                exist = rosetta->lookupRange(words[i], words[i+1]);
                ASSERT_TRUE(exist);
            }

            exist = rosetta->lookupRange(words[words.size() - 1], std::string("zzzzzzzz"));
            ASSERT_TRUE(exist);
        }

        void loadWordList() {
            std::ifstream infile(kFilePath);
            std::string key;
            int count = 0;
            while (infile.good() && count < kWordTestSize) {
                infile >> key;
                words.push_back(key);
                count++;
            }
        }
    }
}


int main (int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    range_filtering::range_bf_kr_no_encoding_test::loadWordList();
    return RUN_ALL_TESTS();
}