#ifndef RANGE_FILTERING_BLINDTRIEWITHBLOOMFILTER_H
#define RANGE_FILTERING_BLINDTRIEWITHBLOOMFILTER_H

#include <map>
#include <vector>
#include <CompactTrie.hpp>
#include <BloomFilter.h>

namespace range_filtering {

class BlindTrieWithBloomFilter {

public:
    // Keys must be sorted
    explicit BlindTrieWithBloomFilter(std::vector<std::string> &keys);
    bool lookupPrefix(std::string prefix);
    uint64_t getMemoryUsage() const;

private:
    class Node {
    protected:
        Node();

        std::map<char, Node *> children_;
        bool end_of_word_;

        virtual bool lookupNode(std::string &key, uint64_t position, bloom_filter::BloomFilter* bloomFilter);
        void insertKeys(uint64_t position, std::vector<std::string> &keys,
                        std::vector<std::string> &hashed_substrings);
        void insertChildNode(char current, uint64_t position,
                             std::vector<std::string> &keys,
                             std::vector<std::string> &hashed_substrings);
        virtual uint64_t getMemoryUsage() const;
        uint64_t getChildrenMemoryUsage() const;
        static void generatePrefixes(std::string &word, uint64_t start_position, std::vector<std::string> &hashed_prefixes);

        friend class BlindTrieWithBloomFilter;

    };
    class BlindNode : public Node {
    protected:
        BlindNode(uint64_t fingerprint, uint64_t length);
        uint64_t getMemoryUsage() const override;

        uint8_t fingerprint_;
        uint64_t length_;

        bool lookupNode(std::string &key, uint64_t position, bloom_filter::BloomFilter* bloomFilter) override;

        friend class BlindTrieWithBloomFilter;
    };

private:
    Node *root_;
    bloom_filter::BloomFilter *bloomFilter_;
};

} // namespace range_filtering

#endif //RANGE_FILTERING_BLINDTRIEWITHBLOOMFILTER_H
