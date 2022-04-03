#include "gtest/gtest.h"

#include "lil_rosetta.hpp"

#include <fstream>

namespace range_filtering_rosetta {
    namespace lilrosettatest {
        static const std::string kFilePath = "/home/mapac/Coding/RangeFiltering/succinct_trie/test/words.txt";
        static std::vector<std::string> words;
        static const int kWordTestSize = 234369;

    class LilRosettaUnitTest : public ::testing::Test {};

        TEST_F(LilRosettaUnitTest, parsingToUint256SimpleCase) {
            auto keys = std::vector<std::string> {
                    "12345" };

            auto rosetta = LilRosetta(keys, 50000);

            std::string key = "3";
            auto expected_value = boost::multiprecision::uint256_t(
                    (boost::multiprecision::uint256_t(3) << (15 - 1*3))
            ) << (256 - 15);

            auto zerosToOneMask = rosetta.calculateOneToZeroMask();

            auto result = rosetta.parseStringToUint256(key, zerosToOneMask);
            ASSERT_EQ(expected_value, result);
        };

        TEST_F(LilRosettaUnitTest, parsingToUint256) {
            auto keys = std::vector<std::string> {
            "1234567890@" };

            auto rosetta = LilRosetta(keys, 50000);

            std::string key = "936=@P";
            auto expected_value = boost::multiprecision::uint256_t(
                    (boost::multiprecision::uint256_t(10) << (44 - 4))
                    + (boost::multiprecision::uint256_t(4) << (44 - 8))
                    + (boost::multiprecision::uint256_t(7) << (44 - 12))
                    + (boost::multiprecision::uint256_t(11) << (44 - 16))
                      + (boost::multiprecision::uint256_t(12) << (44 - 20))
                        + (boost::multiprecision::uint256_t(13) << (44 - 24))
                    ) << (256 - 44);;

            auto zerosToOneMask = std::bitset<256>();
            zerosToOneMask[58] = true;
            zerosToOneMask[65] = true;

            auto result = rosetta.parseStringToUint256(key, zerosToOneMask);
            ASSERT_EQ(expected_value, result);
        };

        TEST_F(LilRosettaUnitTest, trivialCase) {
            std::vector<std::string> keys = {
                    "a",
                    "ab",
                    "abc",
                    "ad",
                    "ada",
                    "adab"
            };
            auto rosetta = LilRosetta(keys, 50000);

            ASSERT_TRUE(rosetta.lookupRange("a", "b"));
            ASSERT_TRUE(rosetta.lookupRange("ab", "abc"));
            ASSERT_TRUE(rosetta.lookupRange("a", "abc"));
        }

        TEST_F(LilRosettaUnitTest, simpleCase) {
            std::vector<std::string> keys = {
                    "f",
                    "far",
                    "fast",
                    "s",
                    "top",
                    "toy",
                    "trie",
            };
            auto rosetta = LilRosetta(keys, 2000);

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

        TEST_F (LilRosettaUnitTest, lookupRangeWordTest) {
            auto rosetta = new LilRosetta(words, 10000);
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
    range_filtering_rosetta::lilrosettatest::loadWordList();
    return RUN_ALL_TESTS();
}
