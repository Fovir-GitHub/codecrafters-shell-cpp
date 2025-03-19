#ifndef _TRIE_H_
#define _TRIE_H_

#include <memory>
#include <unordered_map>
#include <vector>

class Trie
{
private:
    class TrieNode
    {
    public:
        std::unordered_map<char, std::shared_ptr<TrieNode>> children;
        bool                                                is_end = false;

        TrieNode() {}
        ~TrieNode() {}
    };

    std::shared_ptr<TrieNode> root;

    /**
     *@brief Help to find the possible string by prefix by dfs method
     *
     * @param current_node the current TrieNode
     * @param result the array of result
     * @param prefix the prefix
     */
    void
    FindPossibleStringByPrefixHelper(std::shared_ptr<TrieNode> & current_node,
                                     std::vector<std::string> &  result,
                                     std::string &               prefix);

public:
    Trie() : root(std::make_shared<TrieNode>()) {}
    ~Trie() {}

    void Insert(const std::string & word);

    /**
     *@brief Find the possible string with the prefix
     *
     * @param prefix
     * @return std::vector<std::string> all possible strings
     */
    std::vector<std::string> FindPossibleStringByPrefix(std::string & prefix);
};

#endif // !_TRIE_H_