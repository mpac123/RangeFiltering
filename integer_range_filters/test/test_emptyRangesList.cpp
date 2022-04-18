#include "gtest/gtest.h"

#include "EmptyRangesList.hpp"

namespace range_filtering {
    namespace empty_ranges_list_test {
    class EmptyRangesListUnitTest : public ::testing::Test {};

        TEST_F(EmptyRangesListUnitTest, emptyKeysSet) {
            std::vector<uint32_t> keys;
            auto filter = EmptyRangeList(keys, 0.5);

            ASSERT_FALSE(filter.lookupRange(53, 57));
            ASSERT_FALSE(filter.lookupRange(0, 0));
            ASSERT_FALSE(filter.lookupRange(0, -2));
            ASSERT_FALSE(filter.lookupRange(0, 5));
            ASSERT_FALSE(filter.lookupRange(-5, 5));
            ASSERT_FALSE(filter.lookupRange(1000, 2000));
            ASSERT_FALSE(filter.lookupRange(-10000, -542));
        }

        TEST_F(EmptyRangesListUnitTest, simpleKeysSet) {
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
            auto filter = EmptyRangeList(keys, 1.0);

            ASSERT_FALSE(filter.lookupRange(0,1));
            ASSERT_FALSE(filter.lookupRange(6, 8));
            ASSERT_FALSE(filter.lookupRange(37, 39));
            ASSERT_FALSE(filter.lookupRange(47, 57));

            // false positives
            ASSERT_TRUE(filter.lookupRange(43, 45));

            // real positives
            ASSERT_TRUE(filter.lookupRange(41, 43));
            ASSERT_TRUE(filter.lookupRange(41, 42));
            ASSERT_TRUE(filter.lookupRange(42, 43));
            ASSERT_TRUE(filter.lookupRange(45, 46));
            ASSERT_TRUE(filter.lookupRange(0, 2));
            ASSERT_TRUE(filter.lookupRange(71, 72));

            filter = EmptyRangeList(keys, 0.25);

            ASSERT_FALSE(filter.lookupRange(0,1));
            ASSERT_FALSE(filter.lookupRange(6, 8));
            ASSERT_FALSE(filter.lookupRange(37, 39));
            ASSERT_FALSE(filter.lookupRange(47, 57));

            // not a false positives anymore
            ASSERT_FALSE(filter.lookupRange(43, 45));

            // real positives
            ASSERT_TRUE(filter.lookupRange(41, 43));
            ASSERT_TRUE(filter.lookupRange(41, 42));
            ASSERT_TRUE(filter.lookupRange(42, 43));
            ASSERT_TRUE(filter.lookupRange(45, 46));
            ASSERT_TRUE(filter.lookupRange(0, 2));
            ASSERT_TRUE(filter.lookupRange(71, 72));
        }
    }
}

int main (int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}