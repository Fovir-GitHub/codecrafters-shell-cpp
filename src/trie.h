#ifndef _TRIE_H_
#define _TRIE_H_

#include <memory>
#include <string>
#include <unordered_map>

class Trie
{
private:
    struct TrieNode
    {
        std::unordered_map<char, std::shared_ptr<TrieNode>> children;
        bool                                                is_end;

        TrieNode() : is_end(false) {}
        TrieNode(bool ie) : is_end(ie) {}
    };

    std::shared_ptr<TrieNode> root;

public:
    Trie() : root(std::make_shared<TrieNode>()) {}
    Trie(bool ie) : root(std::make_shared<TrieNode>(ie)) {}

    void Insert(const std::string & s);
};

#endif // !_TRIE_H_