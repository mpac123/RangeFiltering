#ifndef RANGE_FILTERING_COMPACTTRIE_HPP
#define RANGE_FILTERING_COMPACTTRIE_HPP

#include <map>
#include <vector>

namespace range_filtering {

class CompactTrie {

public:
    class TrieNode {
    public:
        TrieNode();

        virtual uint64_t getMemoryUsage() const;
    protected:
        bool end_of_word_;
        std::map<char, TrieNode *> children_;
        uint64_t getChildrenMemoryUsage() const;
        virtual bool lookupNode(std::string &key, uint64_t position);

        friend class CompactTrie;
    };

    class CompactNode : public TrieNode {
    public:
        CompactNode(uint64_t fingerprint, uint64_t length);
        uint64_t getMemoryUsage() const override;
    private:
        uint8_t fingerprint_;
        uint64_t length_;

        bool lookupNode(std::string &key, uint64_t position) override;
    };

public:
    // Currently, keys must be sorted
    explicit CompactTrie(std::vector<std::string> &keys);

    bool lookupPrefix(std::string prefix);
    static uint8_t getKarpRabinFingerprint(std::string& pattern);
    uint64_t getMemoryUsage() const;

private:
    TrieNode *root;
    void insert(TrieNode *node, uint64_t position, std::vector<std::string> &keys);
    void insertNode(char current, uint64_t position,
                    std::vector<std::string> &keys_with_common_prefix, TrieNode* node);
};

} // namespace range_filtering


#endif //RANGE_FILTERING_COMPACTTRIE_HPP
