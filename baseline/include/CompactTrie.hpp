#ifndef RANGE_FILTERING_COMPACTTRIE_HPP
#define RANGE_FILTERING_COMPACTTRIE_HPP

#include <map>
#include <vector>
#include <KarpRabinFingerprint.h>
#include <PrefixFilter.h>

namespace range_filtering {

class CompactTrie : public PrefixFilter {

public:
    // Currently, keys must be sorted
    explicit CompactTrie(std::vector<std::string> &keys);
    bool lookupPrefix(const std::string &prefix) override;
    unsigned long long getMemoryUsage() const override;
    std::string getName() const override { return "BlindTrie"; }

protected:
    class TrieNode {
    protected:
        TrieNode();

        std::map<char, TrieNode *> children_;
        bool end_of_word_;

        virtual bool lookupNode(const std::string &key, uint64_t position);
        void insertKeys(uint64_t position, std::vector<std::string> &keys);
        void insertChildNode(char current, uint64_t position, std::vector<std::string> &keys);
        virtual unsigned long long getMemoryUsage() const;
        unsigned long long getChildrenMemoryUsage() const;

        friend class CompactTrie;
    };

    class CompactNode : public TrieNode {
    protected:
        CompactNode(uint64_t fingerprint, uint64_t length);
        unsigned long long getMemoryUsage() const override;

        uint8_t fingerprint_;
        uint64_t length_;

        bool lookupNode(const std::string &key, uint64_t position) override;

        friend class CompactTrie;
    };

protected:
    TrieNode *root;

};

} // namespace range_filtering


#endif //RANGE_FILTERING_COMPACTTRIE_HPP
