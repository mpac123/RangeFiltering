#ifndef RANGE_FILTERING_RESTRAINEDSURFINGTRIE_H
#define RANGE_FILTERING_RESTRAINEDSURFINGTRIE_H

#include <PrefixFilter.h>
#include <vector>
#include <map>

namespace range_filtering {

    enum RestraintType { none, absolute, relative, base };

    class RestrainedSurfingTrie : public PrefixFilter {

    public:
        explicit RestrainedSurfingTrie(std::vector<std::string> &keys, uint64_t max_suffix_length,
                                       RestraintType restraint_type, uint64_t absolute_restraint_val,
                                       double relative_restraint_val);
        bool lookupPrefix(const std::string &prefix) override;
        uint64_t getMemoryUsage() const override;

    protected:
        class TrieNode {
        protected:
            RestrainedSurfingTrie const& trie_;

            TrieNode(RestrainedSurfingTrie const& trie);
            virtual bool lookupNode(const std::string &key, uint64_t position);
            virtual uint64_t getMemoryUsage() const;

        private:
            std::map<char, TrieNode *> children_;
            bool end_of_word_;
            void insertKeys(uint64_t position, std::vector<std::string> &keys);
            void insertChildNode(char current, uint64_t position, std::vector<std::string> &keys);

            uint64_t getChildrenMemoryUsage() const;

            friend class RestrainedSurfingTrie;
        };

        class LeafNode : public TrieNode {
        protected:
            LeafNode(RestrainedSurfingTrie const& trie, uint8_t suffix);
            bool lookupNode(const std::string &key, uint64_t position) override;
            uint64_t getMemoryUsage() const override;

            uint8_t suffix_;

            friend class RestrainedSurfingTrie;
        };

    protected:
        TrieNode *root;
        uint64_t max_suffix_length_;
        RestraintType restraintType_;
        uint64_t absoluteRestraintValue_;
        double relativeRestraintValue_;

        bool isRestrained(uint64_t position, std::string key) const;
    };
}

#endif //RANGE_FILTERING_RESTRAINEDSURFINGTRIE_H
