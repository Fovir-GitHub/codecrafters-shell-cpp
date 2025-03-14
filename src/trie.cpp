#include "trie.h"

void Trie::Insert(const std::string & s)
{
    auto current = root.get();

    for (auto it : s)
    {
        if (!current->children[it])
            current->children[it] = std::make_shared<TrieNode>();

        current = current->children[it].get();
    }

    current->is_end = true;
}
