add_library(TRIE SHARED Trie.cpp CompactTrie.cpp KarpRabinFingerprint.cpp
        BlindTrieWithBloomFilter.cpp PrefixBloomFilter.cpp PrefixQuotientFilter.cpp
        MultiPrefixBloomFilter.cpp MultiPrefixQuotientFilter.cpp SurfingTrie.cpp
        RestrainedSurfingTrie.cpp BloomFiltersEnsemble.cpp BloomedSurfingTrie.cpp
        SplashyTrie.cpp RestrainedSplashyTrie.cpp)
target_link_libraries(TRIE FILTER mmh qf)