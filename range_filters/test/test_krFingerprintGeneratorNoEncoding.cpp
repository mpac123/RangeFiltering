#include "gtest/gtest.h"

#include <KRFingerprintGeneratorNoEncoding.hpp>

namespace range_filtering {
    namespace kr_fingerprint_generator_no_encoding_test {
        class KRFingerprintGeneratorNoEncodingUnitTest : public ::testing::Test {

        };

        TEST_F(KRFingerprintGeneratorNoEncodingUnitTest, simple) {
            std::string key01 = "word";
            std::string key02 = "wore";
            uint32_t k = 16;

            auto generator01 = KRFingerprintGeneratorNoEncoding(key01, k);
            auto generator02 = KRFingerprintGeneratorNoEncoding(key02, k);

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