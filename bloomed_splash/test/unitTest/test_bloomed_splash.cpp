#include "gtest/gtest.h"

#include <assert.h>

#include <fstream>
#include <string>
#include <vector>

#include "config.hpp"
#include "bloomed_splash.hpp"

namespace range_filtering_bloomed_splash {

    namespace fsttest {

        static const std::string kFilePath = "/home/mapac/Coding/RangeFiltering/succinct_trie/test/words.txt";
        static const int kWordTestSize = 234369;
        static const uint64_t kIntTestStart = 10;
        static const int kIntTestBound = 1000001;
        static const uint64_t kIntTestSkip = 10;
        static std::vector<std::string> words;

        class BloomedSplashUnitTest : public ::testing::Test {
        public:
            virtual void SetUp () {
                truncateWordSuffixes();
                fillinInts();
                data_ = nullptr;
            }
            virtual void TearDown () {
                if (data_)
                    delete[] data_;
            }

            void newBloomedSplashWords();
            void newBloomedSplashInts();
            void truncateWordSuffixes();
            void fillinInts();
            void testSerialize();
            void testLookupWord();

            BloomedSplash* fst_;
            std::vector<std::string> words_trunc_;
            std::vector<std::string> ints_;
            char* data_;
        };

        static int getCommonPrefixLen(const std::string &a, const std::string &b) {
            int len = 0;
            while ((len < (int)a.length()) && (len < (int)b.length()) && (a[len] == b[len]))
                len++;
            return len;
        }

        static int getMax(int a, int b) {
            if (a < b)
                return b;
            return a;
        }

        static bool isEqual(const std::string& a, const std::string& b, const unsigned bitlen) {
            if (bitlen == 0) {
                return (a.compare(b) == 0);
            } else {
                std::string a_prefix = a.substr(0, a.length() - 1);
                std::string b_prefix = b.substr(0, b.length() - 1);
                if (a_prefix.compare(b_prefix) != 0) return false;
                char mask = 0xFF << (8 - bitlen);
                char a_suf = a[a.length() - 1] & mask;
                char b_suf = b[b.length() - 1] & mask;
                return (a_suf == b_suf);
            }
        }

        void BloomedSplashUnitTest::newBloomedSplashWords() {
            fst_ = new BloomedSplash(words, 7, 1000, 0.5);
        }

        void BloomedSplashUnitTest::newBloomedSplashInts() {
                fst_ = new BloomedSplash(ints_, 7, 1000, 0.5);
        }

        void BloomedSplashUnitTest::truncateWordSuffixes() {
            assert(words.size() > 1);
            int commonPrefixLen = 0;
            for (unsigned i = 0; i < words.size(); i++) {
                if (i == 0)
                    commonPrefixLen = getCommonPrefixLen(words[i], words[i+1]);
                else if (i == words.size() - 1)
                    commonPrefixLen = getCommonPrefixLen(words[i-1], words[i]);
                else
                    commonPrefixLen = getMax(getCommonPrefixLen(words[i-1], words[i]),
                                             getCommonPrefixLen(words[i], words[i+1]));

                if (commonPrefixLen < (int)words[i].length()) {
                    words_trunc_.push_back(words[i].substr(0, commonPrefixLen + 1));
                } else {
                    words_trunc_.push_back(words[i]);
                    words_trunc_[i] += (char)kTerminator;
                }
            }
        }

        void BloomedSplashUnitTest::fillinInts() {
            for (uint64_t i = 0; i < kIntTestBound; i += kIntTestSkip) {
                ints_.push_back(uint64ToString(i));
            }
        }

        void BloomedSplashUnitTest::testSerialize() {
            data_ = fst_->serialize();
            fst_->destroy();
            delete fst_;
            char* data = data_;
            fst_ = BloomedSplash::deSerialize(data);
        }

        void BloomedSplashUnitTest::testLookupWord() {
            for (unsigned i = 0; i < words.size(); i++) {
                bool key_exist = fst_->lookupKey(words[i]);
                ASSERT_TRUE(key_exist);
            }
        }

        TEST_F (BloomedSplashUnitTest, IntStringConvertTest) {
            for (uint64_t i = 0; i < kIntTestBound; i++) {
                ASSERT_EQ(i, stringToUint64(uint64ToString(i)));
            }
        }

        TEST_F (BloomedSplashUnitTest, lookupWordTest) {
            newBloomedSplashWords();
            testLookupWord();
            fst_->destroy();
            delete fst_;
        }

        TEST_F (BloomedSplashUnitTest, serializeTest) {
            newBloomedSplashWords();
            testSerialize();
            testLookupWord();
        }

        TEST_F (BloomedSplashUnitTest, lookupIntTest) {
            newBloomedSplashInts();
            for (uint64_t i = 0; i < kIntTestBound; i += kIntTestSkip) {
                bool key_exist = fst_->lookupKey(uint64ToString(i));
                if (i % kIntTestSkip == 0)
                    ASSERT_TRUE(key_exist);
                else
                    ASSERT_FALSE(key_exist);
            }
            fst_->destroy();
            delete fst_;
        }

        TEST_F (BloomedSplashUnitTest, moveToKeyGreaterThanWordTest) {
            newBloomedSplashWords();
            bool inclusive = true;
            for (int i = 0; i < 2; i++) {
                if (i == 1)
                    inclusive = false;
                for (int j = -1; j <= (int)words.size(); j++) {
                    BloomedSplash::Iter iter;
                    if (j < 0)
                        iter = fst_->moveToFirst();
                    else if (j >= (int)words.size())
                        iter = fst_->moveToLast();
                    else
                        iter = fst_->moveToKeyGreaterThan(words[j], inclusive);

                    unsigned bitlen;
                    bool is_prefix = false;
                    if (j < 0) {
                        ASSERT_TRUE(iter.isValid());
                        std::string iter_key = iter.getKeyWithSuffix(&bitlen);
                        std::string word_prefix = words[0].substr(0, iter_key.length());
                        is_prefix = isEqual(word_prefix, iter_key, bitlen);
                        ASSERT_TRUE(is_prefix);
                    } else if (j >= (int)words.size()) {
                        ASSERT_TRUE(iter.isValid());
                        std::string iter_key = iter.getKeyWithSuffix(&bitlen);
                        std::string word_prefix = words[words.size() - 1].substr(0, iter_key.length());
                        is_prefix = isEqual(word_prefix, iter_key, bitlen);
                        ASSERT_TRUE(is_prefix);
                    } else if (j == (int)words.size() - 1) {
                        if (iter.getFpFlag()) {
                            ASSERT_TRUE(iter.isValid());
                            std::string iter_key = iter.getKeyWithSuffix(&bitlen);
                            std::string word_prefix = words[words.size() - 1].substr(0, iter_key.length());
                            is_prefix = isEqual(word_prefix, iter_key, bitlen);
                            ASSERT_TRUE(is_prefix);
                        } else {
                            ASSERT_FALSE(iter.isValid());
                        }
                    } else {
                        ASSERT_TRUE(iter.isValid());
                        std::string iter_key = iter.getKeyWithSuffix(&bitlen);
                        std::string word_prefix_fp = words[j].substr(0, iter_key.length());
                        std::string word_prefix_true = words[j+1].substr(0, iter_key.length());
                        if (iter.getFpFlag())
                            is_prefix = isEqual(word_prefix_fp, iter_key, bitlen);
                        else
                            is_prefix = isEqual(word_prefix_true, iter_key, bitlen);
                        ASSERT_TRUE(is_prefix);

                        // test getKey()
                        std::string iter_get_key = iter.getKey();
                        std::string iter_key_prefix = iter_key.substr(0, iter_get_key.length());
                        is_prefix = (iter_key_prefix.compare(iter_get_key) == 0);
                        ASSERT_TRUE(is_prefix);
                    }
                }
            }
            fst_->destroy();
            delete fst_;
        }

        TEST_F (BloomedSplashUnitTest, moveToKeyLessThanWordTest) {
            newBloomedSplashWords();
            bool inclusive = true;
            for (int i = 0; i < 2; i++) {
                if (i == 1)
                    inclusive = false;
                for (unsigned j = 1; j < words.size(); j++) {
                    BloomedSplash::Iter iter = fst_->moveToKeyLessThan(words[j], inclusive);

                    ASSERT_TRUE(iter.isValid());
                    unsigned bitlen;
                    std::string iter_key = iter.getKeyWithSuffix(&bitlen);
                    std::string word_prefix_fp = words[j].substr(0, iter_key.length());
                    std::string word_prefix_true = words[j-1].substr(0, iter_key.length());
                    bool is_prefix = false;
                    if (iter.getFpFlag())
                        is_prefix = isEqual(word_prefix_fp, iter_key, bitlen);
                    else
                        is_prefix = isEqual(word_prefix_true, iter_key, bitlen);
                    ASSERT_TRUE(is_prefix);
                }

                BloomedSplash::Iter iter = fst_->moveToKeyLessThan(words[0], inclusive);
                if (iter.getFpFlag()) {
                    ASSERT_TRUE(iter.isValid());
                    unsigned bitlen;
                    std::string iter_key = iter.getKeyWithSuffix(&bitlen);
                    std::string word_prefix_fp = words[0].substr(0, iter_key.length());
                    bool is_prefix = isEqual(word_prefix_fp, iter_key, bitlen);
                    ASSERT_TRUE(is_prefix);
                } else {
                    ASSERT_FALSE(iter.isValid());
                }
            }
            fst_->destroy();
            delete fst_;
        }



        TEST_F(BloomedSplashUnitTest, words) {
            auto trie = BloomedSplash(words, 7, 1000, 0.5);

            for (const auto& word : words) {
                for (size_t i = 0; i < word.size() - 1; i++) {
                    auto result = trie.lookupPrefix(word.substr(0, i + 1));
                    ASSERT_TRUE(result);
                }
            }
        }

        TEST_F(BloomedSplashUnitTest, bigTrie) {
            std::vector<std::string> keys = {
                    "f",
                    "far",
                    "fast",
                    "s",
                    "seseseseseseseesesese",
                    "top",
                    "toy",
                    "toyyyyyyyyyy",
                    "trie",
            };
            auto trie = BloomedSplash(keys, 7, 1000, 0.5);

            ASSERT_FALSE(trie.lookupPrefix("a"));
            ASSERT_FALSE(trie.lookupPrefix("c"));
            ASSERT_FALSE(trie.lookupPrefix("d"));
            ASSERT_FALSE(trie.lookupPrefix("ca"));
            ASSERT_FALSE(trie.lookupPrefix("ea"));
            ASSERT_FALSE(trie.lookupPrefix("e"));
            ASSERT_FALSE(trie.lookupPrefix("fi"));

            ASSERT_FALSE(trie.lookupPrefix("e"));
            ASSERT_TRUE(trie.lookupPrefix("f"));
            ASSERT_TRUE(trie.lookupPrefix("fa"));
            ASSERT_FALSE(trie.lookupPrefix("fest"));
            ASSERT_FALSE(trie.lookupPrefix("faster"));
            ASSERT_TRUE(trie.lookupPrefix("fast"));
            ASSERT_TRUE(trie.lookupPrefix("trie"));
            ASSERT_FALSE(trie.lookupPrefix("tried"));
            ASSERT_TRUE(trie.lookupPrefix("sesesesesesese"));
            ASSERT_FALSE(trie.lookupPrefix("topless"));
        }

        TEST_F(BloomedSplashUnitTest, range) {
            std::vector<std::string> keys = {
                    "f",
                    "far",
                    "fast",
                    "s",
                    "seseseseseseseesesese",
                    "top",
                    "toy",
                    "toyyyyyyyyyy",
                    "trie",
            };
            auto trie = BloomedSplash(keys, 7, 1000, 0.5);

            //ASSERT_FALSE(trie.lookupRange("a", true, "a0", true));
            //ASSERT_FALSE(trie.lookupRange("e", true, "ea", false));

        }

        void loadWordList() {
            std::ifstream infile(kFilePath);
            std::string key;
            int count = 0;
            while (infile.good() && count < kWordTestSize) {
                infile >> key;
                words.push_back(key);
                count++;
            }
        }

    } // namespace surftest

} // namespace surf

int main (int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    range_filtering_bloomed_splash::fsttest::loadWordList();
    return RUN_ALL_TESTS();
}
