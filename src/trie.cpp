#include "trie.h"

void Trie::Insert(const std::string & word)
{
    auto node = root;

    for (const auto & ch : word)
    {
        if (!node->children[ch])
            node->children[ch] = std::make_shared<TrieNode>();

        node = node->children[ch];
    }

    node->is_end = true;

    return;
}