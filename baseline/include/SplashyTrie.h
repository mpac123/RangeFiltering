#ifndef RANGE_FILTERING_SPLASHYTRIE_H
#define RANGE_FILTERING_SPLASHYTRIE_H

#include <PrefixFilter.h>
#include <vector>
#include <map>
#include <Trie.hpp>

namespace range_filtering {

    class SplashyTrie : public PrefixFilter {

    public:
        explicit SplashyTrie(std::vector<std::string> &keys, uint64_t max_suffix_length,
                             double splashiness_coefficient);
        bool lookupPrefix(const std::string &prefix) override;
        uint64_t getMemoryUsage() const override;
        std::string getName() const override { return "SurfingTrie"; }

    protected:
        class TrieNode {
        protected:
            SplashyTrie const& trie_;

            TrieNode(SplashyTrie const& trie);
            virtual bool lookupNode(const std::string &key, uint64_t position);
            virtual uint64_t getMemoryUsage() const;

        private:
            std::map<char, TrieNode *> children_;
            bool end_of_word_;
            void insertKeys(Trie::TrieNode* current_node);

            uint64_t getChildrenMemoryUsage() const;

            friend class SplashyTrie;
        };

        class LeafNode : public TrieNode {
        protected:
            LeafNode(SplashyTrie const& trie, uint8_t suffix);
            bool lookupNode(const std::string &key, uint64_t position) override;
            uint64_t getMemoryUsage() const override;

            uint8_t suffix_;

            friend class SplashyTrie;
        };

    protected:
        TrieNode *root;
        uint64_t max_suffix_length_;
        double splashiness_coefficient_;
    };
}
#endif //RANGE_FILTERING_SPLASHYTRIE_H
