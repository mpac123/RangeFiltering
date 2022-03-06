#ifndef RANGE_FILTERING_RESTRAINEDSPLASHYTRIE_H
#define RANGE_FILTERING_RESTRAINEDSPLASHYTRIE_H

#include <PrefixFilter.h>
#include <Trie.hpp>
#include <vector>
#include <map>
#include "RestrainedSurfingTrie.h"

namespace range_filtering {

    class RestrainedSplashyTrie : public PrefixFilter {

    public:
        explicit RestrainedSplashyTrie(std::vector<std::string> &keys, uint64_t max_suffix_length,
                                       double splashiness_coefficient, RestraintType restraintType,
                                       uint64_t absolute_restraint_val, double relative_restraint_val);
        bool lookupPrefix(const std::string &prefix) override;
        unsigned long long getMemoryUsage() const override;
        std::string getName() const override { return "RestrainedSplashyTrie"; }

    protected:
        class TrieNode {
        protected:
            RestrainedSplashyTrie const& trie_;

            TrieNode(RestrainedSplashyTrie const& trie);
            virtual bool lookupNode(const std::string &key, uint64_t position);
            virtual unsigned long long getMemoryUsage() const;

        private:
            std::map<char, TrieNode *> children_;
            bool end_of_word_;
            void insertKeys(Trie::TrieNode* current_node);
            void insertChildNode(char current, uint64_t position, std::vector<std::string> &keys);

            unsigned long long getChildrenMemoryUsage() const;

            friend class RestrainedSplashyTrie;
        };

        class LeafNode : public TrieNode {
        protected:
            LeafNode(RestrainedSplashyTrie const& trie, uint8_t suffix);
            bool lookupNode(const std::string &key, uint64_t position) override;
            unsigned long long getMemoryUsage() const override;

            uint8_t suffix_;

            friend class RestrainedSplashyTrie;
        };

    protected:
        TrieNode *root;
        uint64_t max_suffix_length_;
        RestraintType restraintType_;
        uint64_t absoluteRestraintValue_;
        double relativeRestraintValue_;
        double splashiness_coefficient_;

        bool isRestrained(Trie::TrieNode* current_node) const;
    };

}

#endif //RANGE_FILTERING_RESTRAINEDSPLASHYTRIE_H
