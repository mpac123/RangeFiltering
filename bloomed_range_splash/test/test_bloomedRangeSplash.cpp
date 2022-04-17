#include "gtest/gtest.h"

#include <bloomed_range_splash.hpp>
#include <fstream>

namespace range_filtering_bloomed_range_splash {
    namespace bloomed_range_splash_test {
        static const std::string kFilePath = "/home/mapac/Coding/RangeFiltering/succinct_trie/test/words.txt";
        static std::vector<std::string> words;
        static const int kWordTestSize = 234369;
        class BloomedRangeSplashUnitTest : public ::testing::Test {

        };

        TEST_F(BloomedRangeSplashUnitTest, simpleKeysSet) {
            std::vector<std::string> keys = {
                    "f",
                    "far",
                    "fast",
                    "s",
                    "top",
                    "toy",
                    "trie",
            };
            auto trie = BloomedRangeSplash(keys, 2, 50000);

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

        TEST_F (BloomedRangeSplashUnitTest, lookupRangeWordTest) {
            auto rosetta = new BloomedRangeSplash(words, 5, 10000);
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
    range_filtering_bloomed_range_splash::bloomed_range_splash_test::loadWordList();
    return RUN_ALL_TESTS();
}