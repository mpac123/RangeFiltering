#include "gtest/gtest.h"

#include "IntegerSuRF.hpp"

namespace range_filtering {
    namespace integer_surf_test {
    class IntegerSuRFTest : public ::testing::Test {};

        TEST_F(IntegerSuRFTest, simpleKeysSet) {
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
            auto filter = IntegerSuRF(keys);

            ASSERT_FALSE(filter.lookupRange(0,1));
            ASSERT_FALSE(filter.lookupRange(6, 8));
            ASSERT_FALSE(filter.lookupRange(37, 39));
            //ASSERT_FALSE(filter.lookupRange(47, 57));

            // false positives
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