#include "gtest/gtest.h"
#include "popcount.hpp"

namespace range_filtering {
    namespace popcount_test {
    class PopcountUnitTest : public ::testing::Test {};

        TEST_F(PopcountUnitTest, test) {
            uint64_t x = 4123175354784;
            uint64_t y = 7;
            uint64_t* x_pnt = (uint64_t*) calloc(2, 8);
            x_pnt[0] = x;
            x_pnt[1] = y;

            ASSERT_EQ(getpopcount(x_pnt, 0, 8), 2);
            ASSERT_EQ(getpopcount(x_pnt, 0, 39), 9);
            ASSERT_EQ(getpopcount(x_pnt, 0, 64), 12);
            ASSERT_EQ(getpopcount(x_pnt, 0, 65), 13);
            ASSERT_EQ(getpopcount(x_pnt, 0, 66), 14);
            ASSERT_EQ(getpopcount(x_pnt, 0, 67), 15);
            ASSERT_EQ(getpopcount(x_pnt, 0, 70), 15);
            ASSERT_EQ(getpopcount(x_pnt, 0, 100), 15);
        }
    }
}

int main (int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}