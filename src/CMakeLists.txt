add_library(TRIE SHARED Trie.cpp CompactTrie.cpp KarpRabinFingerprint.cpp BlindTrieWithBloomFilter.cpp PrefixBloomFilter.cpp)
target_link_libraries(TRIE BLOOM_FILTER mmh)