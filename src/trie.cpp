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

bool Trie::Search(const std::string & s)
{
    auto current = root.get();

    for (auto it : s)
    {
        if (!current->children.count(it))
            return false;

        current = current->children.at(it).get();
    }

    return current->is_end;
}

bool Trie::FindPrefix(const std::string & prefix)
{
    auto current = root.get();
    for (auto it : prefix)
    {
        if (!current->children.count(it))
            return false;

        current = current->children.at(it).get();
    }

    return true;
}
