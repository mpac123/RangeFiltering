#include "gtest/gtest.h"

#include "rosetta.hpp"

namespace range_filtering_rosetta {
    namespace rosettatest {
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
    }
}

int main (int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
