#ifndef RANGE_FILTERING_SURFINGTRIE_H
#define RANGE_FILTERING_SURFINGTRIE_H

#include <PrefixFilter.h>
#include <vector>
#include <map>

namespace range_filtering {

class SurfingTrie : public PrefixFilter {

public:
    explicit SurfingTrie(std::vector<std::string> &keys, uint64_t max_suffix_length);
    bool lookupPrefix(const std::string &prefix) override;
    unsigned long long getMemoryUsage() const override;
    std::string getName() const override { return "SurfingTrie"; }

protected:
    class TrieNode {
    protected:
        SurfingTrie const& trie_;

        TrieNode(SurfingTrie const& trie);
        virtual bool lookupNode(const std::string &key, uint64_t position);
        virtual unsigned long long getMemoryUsage() const;

    private:
        std::map<char, TrieNode *> children_;
        bool end_of_word_;
        void insertKeys(uint64_t position, std::vector<std::string> &keys);
        void insertChildNode(char current, uint64_t position, std::vector<std::string> &keys);

        unsigned long long getChildrenMemoryUsage() const;

        friend class SurfingTrie;
    };

    class LeafNode : public TrieNode {
    protected:
        LeafNode(SurfingTrie const& trie, uint8_t suffix);
        bool lookupNode(const std::string &key, uint64_t position) override;
        unsigned long long getMemoryUsage() const override;

        uint8_t suffix_;

        friend class SurfingTrie;
    };

protected:
    TrieNode *root;
    uint64_t max_suffix_length_;

};
}

#endif //RANGE_FILTERING_SURFINGTRIE_H
