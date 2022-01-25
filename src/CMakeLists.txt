add_library(TRIE SHARED Trie.cpp CompactTrie.cpp KarpRabinFingerprint.cpp BlindTrieWithBloomFilter.cpp)
target_link_libraries(TRIE BLOOM_FILTER mmh)