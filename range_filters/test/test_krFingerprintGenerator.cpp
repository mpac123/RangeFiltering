#include "gtest/gtest.h"

#include <KRFingerprintGenerator.hpp>

namespace range_filtering {
    namespace kr_fingerprint_generator_test {
    class KRFingerprintGeneratorUnitTest : public ::testing::Test {

    };

        TEST_F(KRFingerprintGeneratorUnitTest, simple) {
            std::string key01 = "word";
            std::string key02 = "wore";
            auto mask = std::bitset<256>();
            mask['w'] = true;
            mask['o'] = true;
            mask['r'] = true;
            mask['d'] = true;
            mask['e'] = true;
            uint32_t k = 16;

            auto generator01 = KRFingerprintGenerator(key01, mask, k);
            auto generator02 = KRFingerprintGenerator(key02, mask, k);

            for (size_t i = 0; i < 3; i++) {
                auto fingerprint1 = generator01.next();
                auto fingerprint2 = generator02.next();

                for (size_t j = 0; j <  k; j++) {
                    ASSERT_EQ(fingerprint1[j], fingerprint2[j]);
                }
            }

            auto fingerprint1 = generator01.next();
            auto fingerprint2 = generator02.next();
            for (size_t j = 0; j <  k; j++) {
                ASSERT_EQ(fingerprint1[j] + 1, fingerprint2[j]);
            }
        }
    }
}


int main (int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}