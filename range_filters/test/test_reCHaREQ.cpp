#include "gtest/gtest.h"
#include "ReCHaREQ.hpp"
#include <fstream>

namespace range_filters {
    namespace rechareq_test {

        static const std::string kFilePath = "/home/mapac/Coding/RangeFiltering/succinct_trie/test/words.txt";
        static std::vector<std::string> words;
        static const int kWordTestSize = 234369;

        class ReCHaREQUnitTest : public ::testing::Test {
        };

        TEST_F(ReCHaREQUnitTest, simpleKeysSet) {
            std::vector<std::string> keys = {
                    "f",
                    "far",
                    "fast",
                    "s",
                    "top",
                    "toy",
                    "trie",
            };
            auto trie = ReCHaREQ(keys, 1, 0.2, 7);

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
            ASSERT_TRUE(trie.lookupRange("far", "fare"));
            ASSERT_TRUE(trie.lookupRange("fara", "fare")); // FP
        }

        TEST_F (ReCHaREQUnitTest, lookupRangeWordTest) {
            auto filter = new ReCHaREQ(words, 1, 0.2, 7);
            bool exist = filter->lookupRange(std::string("\1"), words[0]);
            ASSERT_TRUE(exist);

            for (unsigned i = 0; i < words.size() - 1; i++) {
                exist = filter->lookupRange(words[i], words[i+1]);
                ASSERT_TRUE(exist);
            }

            exist = filter->lookupRange(words[words.size() - 1], std::string("zzzzzzzz"));
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
    range_filters::rechareq_test::loadWordList();
    return RUN_ALL_TESTS();
}