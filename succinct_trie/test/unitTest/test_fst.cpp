#include "gtest/gtest.h"

#include <assert.h>

#include <fstream>
#include <string>
#include <vector>

#include "config.hpp"
#include "fst.hpp"

namespace range_filtering {

    namespace fsttest {

        static const std::string kFilePath = "/home/mapac/Coding/RangeFiltering/succinct_trie/test/words.txt";
        static const int kWordTestSize = 234369;
        static const uint64_t kIntTestStart = 10;
        static const int kIntTestBound = 1000001;
        static const uint64_t kIntTestSkip = 10;
        static std::vector<std::string> words;

        class FSTUnitTest : public ::testing::Test {
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

            void newFSTWords();
            void newFSTInts();
            void truncateWordSuffixes();
            void fillinInts();
            void testSerialize();
            void testLookupWord();

            FST* fst_;
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

        void FSTUnitTest::newFSTWords() {
            fst_ = new FST(words);
        }

        void FSTUnitTest::newFSTInts() {
                fst_ = new FST(ints_);
        }

        void FSTUnitTest::truncateWordSuffixes() {
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

        void FSTUnitTest::fillinInts() {
            for (uint64_t i = 0; i < kIntTestBound; i += kIntTestSkip) {
                ints_.push_back(uint64ToString(i));
            }
        }

        void FSTUnitTest::testSerialize() {
            data_ = fst_->serialize();
            fst_->destroy();
            delete fst_;
            char* data = data_;
            fst_ = FST::deSerialize(data);
        }

        void FSTUnitTest::testLookupWord() {
            for (unsigned i = 0; i < words.size(); i++) {
                bool key_exist = fst_->lookupKey(words[i]);
                ASSERT_TRUE(key_exist);
            }
        }

        TEST_F (FSTUnitTest, IntStringConvertTest) {
            for (uint64_t i = 0; i < kIntTestBound; i++) {
                ASSERT_EQ(i, stringToUint64(uint64ToString(i)));
            }
        }

        TEST_F (FSTUnitTest, lookupWordTest) {
            newFSTWords();
            testLookupWord();
            fst_->destroy();
            delete fst_;
        }

        TEST_F (FSTUnitTest, serializeTest) {
            newFSTWords();
            testSerialize();
            testLookupWord();
        }

        TEST_F (FSTUnitTest, lookupIntTest) {
            newFSTInts();
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

        TEST_F (FSTUnitTest, moveToKeyGreaterThanWordTest) {
            newFSTWords();
            bool inclusive = true;
            for (int i = 0; i < 2; i++) {
                if (i == 1)
                    inclusive = false;
                for (int j = -1; j <= (int)words.size(); j++) {
                    FST::Iter iter;
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


        TEST_F (FSTUnitTest, moveToKeyGreaterThanIntTest) {
            newFSTInts();
            bool inclusive = true;
            for (int i = 0; i < 2; i++) {
                if (i == 1)
                    inclusive = false;
                for (uint64_t j = 0; j < kIntTestBound - 1; j++) {
                    FST::Iter iter = fst_->moveToKeyGreaterThan(uint64ToString(j), inclusive);

                    ASSERT_TRUE(iter.isValid());
                    unsigned bitlen;
                    std::string iter_key = iter.getKeyWithSuffix(&bitlen);
                    std::string int_key_fp = uint64ToString(j - (j % kIntTestSkip));
                    std::string int_key_true = uint64ToString(j - (j % kIntTestSkip) + kIntTestSkip);
                    std::string int_prefix_fp = int_key_fp.substr(0, iter_key.length());
                    std::string int_prefix_true = int_key_true.substr(0, iter_key.length());
                    bool is_prefix = false;
                    if (iter.getFpFlag())
                        is_prefix = isEqual(int_prefix_fp, iter_key, bitlen);
                    else
                        is_prefix = isEqual(int_prefix_true, iter_key, bitlen);
                    ASSERT_TRUE(is_prefix);
                }

                FST::Iter iter = fst_->moveToKeyGreaterThan(uint64ToString(kIntTestBound - 1), inclusive);
                if (iter.getFpFlag()) {
                    ASSERT_TRUE(iter.isValid());
                    unsigned bitlen;
                    std::string iter_key = iter.getKeyWithSuffix(&bitlen);
                    std::string int_key_fp = uint64ToString(kIntTestBound - 1);
                    std::string int_prefix_fp = int_key_fp.substr(0, iter_key.length());
                    bool is_prefix = isEqual(int_prefix_fp, iter_key, bitlen);
                    ASSERT_TRUE(is_prefix);
                } else {
                    ASSERT_FALSE(iter.isValid());
                }
            }
            fst_->destroy();
            delete fst_;
        }

        TEST_F (FSTUnitTest, moveToKeyLessThanWordTest) {
            newFSTWords();
            bool inclusive = true;
            for (int i = 0; i < 2; i++) {
                if (i == 1)
                    inclusive = false;
                for (unsigned j = 1; j < words.size(); j++) {
                    FST::Iter iter = fst_->moveToKeyLessThan(words[j], inclusive);

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

                FST::Iter iter = fst_->moveToKeyLessThan(words[0], inclusive);
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

        TEST_F (FSTUnitTest, moveToKeyLessThanIntTest) {
            newFSTInts();
            bool inclusive = true;
            for (int i = 0; i < 2; i++) {
                if (i == 1)
                    inclusive = false;
                for (uint64_t j = kIntTestSkip; j < kIntTestBound; j++) {
                    FST::Iter iter = fst_->moveToKeyLessThan(uint64ToString(j), inclusive);

                    ASSERT_TRUE(iter.isValid());
                    unsigned bitlen;
                    std::string iter_key = iter.getKeyWithSuffix(&bitlen);
                    std::string int_key = uint64ToString(j - (j % kIntTestSkip));
                    std::string int_prefix = int_key.substr(0, iter_key.length());
                    bool is_prefix = isEqual(int_prefix, iter_key, bitlen);
                    ASSERT_TRUE(is_prefix);
                }
                FST::Iter iter = fst_->moveToKeyLessThan(uint64ToString(0), inclusive);
                if (iter.getFpFlag()) {
                    ASSERT_TRUE(iter.isValid());
                    unsigned bitlen;
                    std::string iter_key = iter.getKeyWithSuffix(&bitlen);
                    std::string int_key = uint64ToString(0);
                    std::string int_prefix = int_key.substr(0, iter_key.length());
                    bool is_prefix = isEqual(int_prefix, iter_key, bitlen);
                    ASSERT_TRUE(is_prefix);
                } else {
                    ASSERT_FALSE(iter.isValid());
                }
            }
            fst_->destroy();
            delete fst_;
        }


        TEST_F (FSTUnitTest, IteratorIncrementWordTest) {
            newFSTWords();
            bool inclusive = true;
            FST::Iter iter = fst_->moveToKeyGreaterThan(words[0], inclusive);
            for (unsigned i = 1; i < words.size(); i++) {
                iter++;
                ASSERT_TRUE(iter.isValid());
                std::string iter_key;
                unsigned bitlen;
                iter_key = iter.getKeyWithSuffix(&bitlen);
                std::string word_prefix = words[i].substr(0, iter_key.length());
                bool is_prefix = isEqual(word_prefix, iter_key, bitlen);
                ASSERT_TRUE(is_prefix);
            }
            iter++;
            ASSERT_FALSE(iter.isValid());
            fst_->destroy();
            delete fst_;
        }

        TEST_F (FSTUnitTest, IteratorIncrementIntTest) {
            newFSTInts();
            bool inclusive = true;
            FST::Iter iter = fst_->moveToKeyGreaterThan(uint64ToString(0), inclusive);
            for (uint64_t i = kIntTestSkip; i < kIntTestBound; i += kIntTestSkip) {
                iter++;
                ASSERT_TRUE(iter.isValid());
                std::string iter_key;
                unsigned bitlen;
                iter_key = iter.getKeyWithSuffix(&bitlen);
                std::string int_prefix = uint64ToString(i).substr(0, iter_key.length());
                bool is_prefix = isEqual(int_prefix, iter_key, bitlen);
                ASSERT_TRUE(is_prefix);
            }
            iter++;
            ASSERT_FALSE(iter.isValid());
            fst_->destroy();
            delete fst_;
        }

        TEST_F (FSTUnitTest, IteratorDecrementWordTest) {
            newFSTWords();
            bool inclusive = true;
            FST::Iter iter = fst_->moveToKeyGreaterThan(words[words.size() - 1], inclusive);
            for (int i = words.size() - 2; i >= 0; i--) {
                iter--;
                ASSERT_TRUE(iter.isValid());
                std::string iter_key;
                unsigned bitlen;
                iter_key = iter.getKeyWithSuffix(&bitlen);
                std::string word_prefix = words[i].substr(0, iter_key.length());
                bool is_prefix = isEqual(word_prefix, iter_key, bitlen);
                ASSERT_TRUE(is_prefix);
            }
            iter--;
            ASSERT_FALSE(iter.isValid());
            fst_->destroy();
            delete fst_;
        }

        TEST_F (FSTUnitTest, IteratorDecrementIntTest) {
            newFSTInts();
            bool inclusive = true;
            FST::Iter iter = fst_->moveToKeyGreaterThan(uint64ToString(kIntTestBound - kIntTestSkip), inclusive);
            for (uint64_t i = kIntTestBound - 1 - kIntTestSkip; i > 0; i -= kIntTestSkip) {
                iter--;
                ASSERT_TRUE(iter.isValid());
                std::string iter_key;
                unsigned bitlen;
                iter_key = iter.getKeyWithSuffix(&bitlen);
                std::string int_prefix = uint64ToString(i).substr(0, iter_key.length());
                bool is_prefix = isEqual(int_prefix, iter_key, bitlen);
                ASSERT_TRUE(is_prefix);
            }
            iter--;
            iter--;
            ASSERT_FALSE(iter.isValid());
            fst_->destroy();
            delete fst_;
        }

        TEST_F (FSTUnitTest, lookupRangeWordTest) {
            newFSTWords();
            bool exist = fst_->lookupRange(std::string("\1"), true, words[0], true);
            ASSERT_TRUE(exist);
            exist = fst_->lookupRange(std::string("\1"), true, words[0], false);
            ASSERT_TRUE(exist);

            for (unsigned i = 0; i < words.size() - 1; i++) {
                exist = fst_->lookupRange(words[i], true, words[i+1], true);
                ASSERT_TRUE(exist);
                exist = fst_->lookupRange(words[i], true, words[i+1], false);
                ASSERT_TRUE(exist);
                exist = fst_->lookupRange(words[i], false, words[i+1], true);
                ASSERT_TRUE(exist);
                exist = fst_->lookupRange(words[i], false, words[i+1], false);
                ASSERT_TRUE(exist);
            }

            exist = fst_->lookupRange(words[words.size() - 1], true, std::string("zzzzzzzz"), false);
            ASSERT_TRUE(exist);
            exist = fst_->lookupRange(words[words.size() - 1], false, std::string("zzzzzzzz"), false);
            ASSERT_TRUE(exist);
            fst_->destroy();
            delete fst_;
        }

        TEST_F (FSTUnitTest, lookupRangeIntTest) {
            newFSTInts();
            for (uint64_t i = 0; i < kIntTestBound; i++) {
                bool exist = fst_->lookupRange(uint64ToString(i), true,
                                                uint64ToString(i), true);
                if (i % kIntTestSkip == 0)
                    ASSERT_TRUE(exist);
                else
                    ASSERT_FALSE(exist);

                for (unsigned j = 1; j < kIntTestSkip + 2; j++) {
                    exist = fst_->lookupRange(uint64ToString(i), false,
                                               uint64ToString(i + j), true);
                    uint64_t left_bound_interval_id = i / kIntTestSkip;
                    uint64_t right_bound_interval_id = (i + j) / kIntTestSkip;
                    if ((i % kIntTestSkip == 0)
                        || ((i < kIntTestBound - 1)
                            && ((left_bound_interval_id < right_bound_interval_id)
                                || ((i + j) % kIntTestSkip == 0))))
                        ASSERT_TRUE(exist);
                    else
                        ASSERT_FALSE(exist);
                }
            }
            fst_->destroy();
            delete fst_;
        }

        TEST_F (FSTUnitTest, approxCountWordTest) {
            newFSTWords();
            const int num_start_indexes = 5;
            const int start_indexes[num_start_indexes] =
                    {0, kWordTestSize/4, kWordTestSize/2, 3*kWordTestSize/4, kWordTestSize-1};
            for (int i = 0; i < num_start_indexes; i++) {
                int s = start_indexes[i];
                for (int j = s; j < kWordTestSize; j++) {
                    FST::Iter iter = fst_->moveToKeyGreaterThan(words[s], true);
                    FST::Iter iter2 = fst_->moveToKeyGreaterThan(words[j], true);
                    uint64_t count = fst_->approxCount(&iter, &iter2);
                    int error = j - s - count;
                    if (j > s)
                        error--;
                    ASSERT_TRUE(error == 0);
                }
            }
            fst_->destroy();
            delete fst_;
        }

        TEST_F (FSTUnitTest, approxCountIntTest) {
            fst_ = new FST(ints_, kIncludeDense, 256);
            const int num_start_indexes = 5;
            const int start_indexes[num_start_indexes] =
                    {0, kIntTestBound/4, kIntTestBound/2, 3*kIntTestBound/4, kIntTestBound-1};
            for (int i = 0; i < num_start_indexes; i++) {
                int s = start_indexes[i];
                for (int j = s; j < kIntTestBound; j += kIntTestSkip) {
                    FST::Iter iter = fst_->moveToKeyGreaterThan(uint64ToString(s), true);
                    FST::Iter iter2 = fst_->moveToKeyGreaterThan(uint64ToString(j), true);
                    uint64_t count = fst_->approxCount(&iter, &iter2);
                    int error = (j - start_indexes[i]) / kIntTestSkip - count;
                    if (j > s)
                        error--;
                    ASSERT_TRUE(error == 0);
                }
            }
            fst_->destroy();
            delete fst_;
        }

        TEST_F(FSTUnitTest, words) {
            auto trie = FST(words);

            for (const auto& word : words) {
                for (size_t i = 0; i < word.size() - 1; i++) {
                    ASSERT_TRUE(trie.lookupPrefix(word.substr(0, i + 1)));
                }
            }
        }

        TEST_F(FSTUnitTest, bigTrie) {
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
            auto trie = FST(keys);

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

        TEST_F(FSTUnitTest, range) {
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
            auto trie = FST(keys);

            ASSERT_FALSE(trie.lookupRange("a", true, "a0", true));
            ASSERT_FALSE(trie.lookupRange("e", true, "ea", false));
            ASSERT_FALSE(trie.lookupRange("fasu", "fasz"));
            ASSERT_FALSE(trie.lookupRange("seseseseseseseesesesf", "seseseseseseseesesesg"));

        }

        TEST_F(FSTUnitTest, tricky) {
            std::vector<std::string> keys = {
                    //"f",
                    //"far",
                    //"fast",
                    "s",
                    //"top",
                    "toy",
                    "toyak",
                    "toybk",
                    "toyck",
                    //"toydk",
                    "toyota",
                    "trie",
            };
            auto trie = FST(keys);

            auto iter = trie.moveToKeyGreaterThan("toyek", true);

            ASSERT_EQ(iter.getKey(), "toyota");
        }

        TEST_F(FSTUnitTest, trickyCase) {
            std::vector<std::string> keys = {
                    "oocvgxryapxon",
                    "oocvgxrz",
                    "oocvgxrzicws",
//                    "oocvgxrzieayyd",
//                    "oocvgxrzirev",
//                    "oocvgxrziuvptd",
                    "oocvgxrzjfgqx",
                    "oocvgxrzknwhxkbq",
//                    "oocvgxrzkpmhlzb",
//                    "oocvgxrzkstldyn",
//                    "oocvgxrzkwdlurq",
                    "oocvgxrzxbfghcboeu",
//                    "oocvgxrzxbwec",
//                    "oocvgxrzxepkgl",
//                    "oocvgxrzxqfrq",
//                    "oocvgxrzxqjrh",
//                    "oocvgxrzxqjrxj",
//                    "oocvgxrzxzxcudv",
                    "oocvgxsddqjhw",
                    "oocvgxshafosrf"
            };
            auto trie = FST(keys);

            auto iter = trie.moveToKeyGreaterThan("oocvgxrzp", true);

            ASSERT_EQ(iter.getKey(), "oocvgxrzxbfghcboeu");
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
    range_filtering::fsttest::loadWordList();
    return RUN_ALL_TESTS();
}
