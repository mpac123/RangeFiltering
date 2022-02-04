add_library(TRIE SHARED Trie.cpp CompactTrie.cpp KarpRabinFingerprint.cpp
        BlindTrieWithBloomFilter.cpp PrefixBloomFilter.cpp PrefixQuotientFilter.cpp)
target_link_libraries(TRIE FILTER mmh qf)