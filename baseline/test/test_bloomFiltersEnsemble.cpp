#include "gtest/gtest.h"
#include "BloomFiltersEnsemble.h"

namespace range_filtering {
    namespace bloom_filters_ensemble_test {
    class BloomFiltersEnsembleUnitTest : public ::testing::Test {

    };

        TEST_F(BloomFiltersEnsembleUnitTest, emptyDataset) {
            auto keys = std::vector<std::vector<std::string>>();
            auto trie = BloomFiltersEnsemble(keys, 8000, 0.5);

            ASSERT_FALSE(trie.lookupKey("anything"));
            ASSERT_FALSE(trie.lookupKey(""));
        }

        TEST_F(BloomFiltersEnsembleUnitTest, simpleDataset) {
            std::vector<std::vector<std::string>> keys {
                std::vector<std::string>(),
                std::vector<std::string>(),
                std::vector<std::string>(),
                std::vector<std::string> {"aaaa", "bbbb", "cccc"},
                std::vector<std::string> {"aaaaa", "bbbbb", "ccccc"}
            };
            auto ensemble = BloomFiltersEnsemble(keys, 8000, 0.5);

            ASSERT_TRUE(ensemble.lookupKey("aaaa"));
            ASSERT_TRUE(ensemble.lookupKey("bbbb"));
            ASSERT_TRUE(ensemble.lookupKey("cccc"));
            ASSERT_TRUE(ensemble.lookupKey("aaaaa"));
            ASSERT_TRUE(ensemble.lookupKey("bbbbb"));
            ASSERT_TRUE(ensemble.lookupKey("ccccc"));

            ASSERT_FALSE(ensemble.lookupKey(""));
            ASSERT_FALSE(ensemble.lookupKey("a"));
            ASSERT_FALSE(ensemble.lookupKey("aaaaaa"));
            ASSERT_FALSE(ensemble.lookupKey("abcs"));
        }
    }
}

int main (int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}