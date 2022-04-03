#include "gtest/gtest.h"

#include "bf.hpp"

namespace range_filtering_rosetta {
    namespace bftest {
    class BFUnitTest : public ::testing::Test {};

    TEST_F (BFUnitTest, test) {
        auto keys = std::vector<boost::multiprecision::uint256_t> {
            boost::multiprecision::uint256_t(50),
            boost::multiprecision::uint256_t(52),
            boost::multiprecision::uint256_t(53),
            boost::multiprecision::uint256_t(10563650),
        };
        auto bf = BF(keys, 1000);

        for (const auto& key : keys) {
            ASSERT_TRUE(bf.lookupKey(key));
        }
        ASSERT_FALSE(bf.lookupKey(boost::multiprecision::uint256_t(70)));
        ASSERT_FALSE(bf.lookupKey(boost::multiprecision::uint256_t(71)));
        ASSERT_FALSE(bf.lookupKey(boost::multiprecision::uint256_t(75)));
        ASSERT_FALSE(bf.lookupKey(boost::multiprecision::uint256_t(1005370)));

        }
    }
}

int main (int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
