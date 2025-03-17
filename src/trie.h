#ifndef _TRIE_H_
#define _TRIE_H_

#include <memory>
#include <unordered_map>

class TrieNode
{
public:
    std::unordered_map<char, std::shared_ptr<TrieNode>> children;
    bool                                                is_end = false;

    TrieNode() {}
    ~TrieNode() {}
};

class Trie
{
private:
    std::shared_ptr<TrieNode> root;

public:
    Trie() : root(std::make_shared<TrieNode>()) {}

    void Insert(const std::string & word);
};

#endif // !_TRIE_H_