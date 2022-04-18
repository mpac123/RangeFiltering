#include "gtest/gtest.h"

#include "integer_rosetta.hpp"

namespace range_filtering_rosetta {
    namespace integer_rosetta_test {
    class IntegerRosettaUnitTest : public ::testing::Test {};

    TEST_F(IntegerRosettaUnitTest, countTrailingZeros) {
        ASSERT_EQ(3, IntegerRosetta::countTrailingZeroes(int32_t(8)));
        ASSERT_EQ(3, IntegerRosetta::countTrailingZeroes(int32_t (1) << 3));
        ASSERT_EQ(16, IntegerRosetta::countTrailingZeroes(int32_t(1) << 16));


        auto number = int32_t (int32_t (1) << 30);
        auto result = IntegerRosetta::countTrailingZeroes(number);

        ASSERT_EQ(result, 30);
    }

        TEST_F(IntegerRosettaUnitTest, simpleKeysSet) {
            std::vector<uint32_t> keys = std::vector<uint32_t> {
                    2,
                    4,
                    5,
                    12,
                    16,
                    20,
                    26,
                    32,
                    34,
                    42,
                    46,
                    58,
                    60,
                    61,
                    66,
                    72
            };
            auto filter = IntegerRosetta(keys, 50000);

            ASSERT_FALSE(filter.lookupRange(0,1));
            ASSERT_FALSE(filter.lookupRange(6, 8));
            ASSERT_FALSE(filter.lookupRange(37, 39));
            ASSERT_FALSE(filter.lookupRange(47, 57));
            ASSERT_FALSE(filter.lookupRange(43, 45));

            // real positives
            ASSERT_TRUE(filter.lookupRange(41, 43));
            ASSERT_TRUE(filter.lookupRange(41, 42));
            ASSERT_TRUE(filter.lookupRange(42, 43));
            ASSERT_TRUE(filter.lookupRange(45, 46));
        }
    }
}

int main (int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}