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
        auto k = BF::calculateNumberOfHashes(keys.size(), 1000);
        auto bf = BF(keys, 1000, k);

        for (const auto& key : keys) {
            ASSERT_TRUE(bf.lookupKey(key, k));
        }
        ASSERT_FALSE(bf.lookupKey(boost::multiprecision::uint256_t(70), k));
        ASSERT_FALSE(bf.lookupKey(boost::multiprecision::uint256_t(71), k));
        ASSERT_FALSE(bf.lookupKey(boost::multiprecision::uint256_t(75), k));
        ASSERT_FALSE(bf.lookupKey(boost::multiprecision::uint256_t(1005370), k));

        }
    }
}

int main (int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
