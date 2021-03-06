#include "gtest/gtest.h"

#include "rosetta.hpp"

#include <fstream>


namespace range_filtering_rosetta {
    namespace rosettatest {

    static const std::string kFilePath = "/home/mapac/Coding/RangeFiltering/succinct_trie/test/words.txt";
    static std::vector<std::string> words;
    static const int kWordTestSize = 234369;

    class RosettaUnitTest : public ::testing::Test {};

        TEST_F(RosettaUnitTest, parsingToUint256) {
            std::string key = "abc";
            auto expected_value = boost::multiprecision::uint256_t((boost::multiprecision::uint256_t(97) << (256 - 8))
                    + (boost::multiprecision::uint256_t(98) << (256 - 16))
                    + (boost::multiprecision::uint256_t(99) << (256 - 24)));

            auto result = Rosetta::parseStringToUint256(key);
            ASSERT_EQ(expected_value, result);
        }

        TEST_F(RosettaUnitTest, countTrailingZeros) {
            ASSERT_EQ(3, Rosetta::countTrailingZeroes(boost::multiprecision::uint256_t(8)));
            ASSERT_EQ(3, Rosetta::countTrailingZeroes(boost::multiprecision::uint256_t(1) << 3));
            ASSERT_EQ(16, Rosetta::countTrailingZeroes(boost::multiprecision::uint256_t(1) << 16));


            auto number = boost::multiprecision::uint256_t(boost::multiprecision::uint256_t(1) << 240);
            auto result = Rosetta::countTrailingZeroes(number);

            ASSERT_EQ(result, 240);
        }

        TEST_F(RosettaUnitTest, simpleCase) {
            std::vector<std::string> keys = {
                    "f",
                    "far",
                    "fast",
                    "s",
                    "top",
                    "toy",
                    "trie",
            };
            auto rosetta = Rosetta(keys, 2000);

            ASSERT_FALSE(rosetta.lookupRange("a", "a"));
            ASSERT_FALSE(rosetta.lookupRange("a", "b"));
            ASSERT_FALSE(rosetta.lookupRange("fan", "fans"));
            ASSERT_TRUE(rosetta.lookupRange("f", "f"));
            ASSERT_TRUE(rosetta.lookupRange("f", "fa"));
            ASSERT_TRUE(rosetta.lookupRange("f", "fast"));
            ASSERT_TRUE(rosetta.lookupRange("t", "top"));
            ASSERT_TRUE(rosetta.lookupRange("t", "tor"));
            ASSERT_TRUE(rosetta.lookupRange("e", "fa"));
            ASSERT_TRUE(rosetta.lookupRange("s", "sa"));
            ASSERT_TRUE(rosetta.lookupRange("r", "sa"));
            ASSERT_TRUE(rosetta.lookupRange("rabarb", "sa"));
            ASSERT_TRUE(rosetta.lookupRange("t", "tr"));
            ASSERT_TRUE(rosetta.lookupRange("t", "tram"));
            ASSERT_TRUE(rosetta.lookupRange("triangle", "tries"));
            ASSERT_FALSE(rosetta.lookupRange("toz", "tozs"));
            ASSERT_FALSE(rosetta.lookupRange("toys", "toysRS"));
            ASSERT_FALSE(rosetta.lookupRange("toz", "tozsRS"));
            ASSERT_FALSE(rosetta.lookupRange("triangle", "triangles"));
        }

        TEST_F (RosettaUnitTest, lookupRangeWordTest) {
            auto rosetta = new Rosetta(words, 10000);
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
    range_filtering_rosetta::rosettatest::loadWordList();
    return RUN_ALL_TESTS();
}
