#include "trie.h"

void Trie::FindPossibleStringByPrefixHelper(
    std::shared_ptr<TrieNode> & current_node, std::vector<std::string> & result,
    std::string & prefix)
{
    for (auto & [next_character, next_node] : current_node->children)
    {
        // There is no next node
        if (!next_node)
            continue;

        // Add the next character to the prefix string
        prefix.push_back(next_character);
        if (next_node->is_end) /* If the next node is one end of a word */
            result.push_back(prefix); /* Add it to the result */

        // Recrusively find the possible string
        FindPossibleStringByPrefixHelper(next_node, result, prefix);
        prefix.pop_back(); /* Step back */
    }

    return;
}

void Trie::Insert(const std::string & word)
{
    std::shared_ptr<TrieNode> node = root;

    for (const char & ch : word)
    {
        if (!node->children[ch])
            node->children[ch] = std::make_shared<TrieNode>();

        node = node->children[ch];
    }

    node->is_end = true;

    return;
}

std::vector<std::string> Trie::FindPossibleStringByPrefix(std::string & prefix)
{
    std::shared_ptr<TrieNode> node = root;
    std::vector<std::string>  result;

    // Find the prefix at first
    for (const char & ch : prefix)
    {
        // Not found
        if (!node->children[ch])
            return result; /* Return empty result */
        node = node->children[ch];
    }

    if (node->is_end) /* The prefix is a possible word */
        result.push_back(prefix);

    // Find the remaining possible strings
    FindPossibleStringByPrefixHelper(node, result, prefix);

    return result;
}
