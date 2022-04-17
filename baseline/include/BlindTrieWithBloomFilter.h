#ifndef RANGE_FILTERING_BLINDTRIEWITHBLOOMFILTER_H
#define RANGE_FILTERING_BLINDTRIEWITHBLOOMFILTER_H

#include <map>
#include <vector>
#include <CompactTrie.hpp>
#include <BloomFilter.h>
#include <PrefixFilter.h>

namespace range_filtering {

class BlindTrieWithBloomFilter : public PrefixFilter {

public:
    // Keys must be sorted
    explicit BlindTrieWithBloomFilter(std::vector<std::string> &keys, uint32_t bloom_filter_size);
    bool lookupPrefix(const std::string &prefix) override;
    uint64_t getMemoryUsage() const override;

private:
    class Node {
    protected:
        Node();

        std::map<char, Node *> children_;
        bool end_of_word_;

        virtual bool lookupNode(const std::string &key, uint64_t position, bloom_filter::BloomFilter* bloomFilter);
        void insertKeys(uint64_t position, std::vector<std::string> &keys,
                        std::vector<std::string> &hashed_substrings);
        void insertChildNode(char current, uint64_t position,
                             std::vector<std::string> &keys,
                             std::vector<std::string> &hashed_substrings);
        virtual uint64_t getMemoryUsage() const;
        unsigned long long getChildrenMemoryUsage() const;
        static void generatePrefixes(std::string &word, uint64_t start_position, std::vector<std::string> &hashed_prefixes);

        friend class BlindTrieWithBloomFilter;

    };
    class BlindNode : public Node {
    protected:
        BlindNode(uint64_t fingerprint, uint64_t length);
        uint64_t getMemoryUsage() const override;

        uint8_t fingerprint_;
        uint64_t length_;

        bool lookupNode(const std::string &key, uint64_t position, bloom_filter::BloomFilter* bloomFilter) override;

        friend class BlindTrieWithBloomFilter;
    };

private:
    Node *root_;
    bloom_filter::BloomFilter *bloomFilter_;
};

} // namespace range_filtering

#endif //RANGE_FILTERING_BLINDTRIEWITHBLOOMFILTER_H
