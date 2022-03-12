#ifndef RANGE_FILTERING_BLOOMEDSURFINGTRIE_H
#define RANGE_FILTERING_BLOOMEDSURFINGTRIE_H

#include <PrefixFilter.h>
#include <BloomFiltersEnsemble.h>
#include <vector>
#include <map>
#include <unordered_set>

namespace range_filtering {

    class BloomedSurfingTrie : public PrefixFilter {

    public:
        explicit BloomedSurfingTrie(std::vector<std::string> &keys, uint64_t max_suffix_length,
                                    uint32_t BF_size, double max_penalty);
        bool lookupPrefix(const std::string &prefix) override;
        uint64_t getMemoryUsage() const override;
        std::string getName() const override { return "BloomedSurfingTrie"; }

    protected:
        class TrieNode {
        protected:
            BloomedSurfingTrie const& trie_;

            TrieNode(BloomedSurfingTrie const& trie);
            virtual bool lookupNode(const std::string &key, uint64_t position);
            virtual uint64_t getMemoryUsage() const;

        private:
            std::map<char, TrieNode *> children_;
            bool end_of_word_;
            void insertKeys(uint64_t position, std::vector<std::string> &keys);
            void insertChildNode(char current, uint64_t position, std::vector<std::string> &keys);

            uint64_t getChildrenMemoryUsage() const;

            friend class BloomedSurfingTrie;
        };

        class LeafNode : public TrieNode {
        protected:
            LeafNode(BloomedSurfingTrie const& trie, uint8_t suffix);
            bool lookupNode(const std::string &key, uint64_t position) override;
            uint64_t getMemoryUsage() const override;

            uint8_t suffix_;

            friend class BloomedSurfingTrie;
        };

    protected:
        TrieNode *root;
        uint64_t max_suffix_length_;
        BloomFiltersEnsemble bloomFilters_;

        std::vector<std::vector<std::string>> prefixes_;

    };
}

#endif //RANGE_FILTERING_BLOOMEDSURFINGTRIE_H
