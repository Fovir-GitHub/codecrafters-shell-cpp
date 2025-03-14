#ifndef _TRIE_H_
#define _TRIE_H_

#include <memory>
#include <string>
#include <unordered_map>

class TrieNode
{
private:
    std::unordered_map<char, std::shared_ptr<TrieNode>> children;
    bool                                                is_end;

public:
    TrieNode() : is_end(false) {}
    TrieNode(bool ie) : is_end(ie) {}

    bool IsEnd() const { return is_end; }
};

class Trie
{
private:
    std::shared_ptr<TrieNode> root;

public:
    Trie() : root(std::make_shared<TrieNode>()) {}
};

#endif // !_TRIE_H_