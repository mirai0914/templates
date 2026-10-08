#pragma once
#include <vector>
#include <array>
#include <string>

/**
 * @brief 字典树 (Trie)
 * @details 默认支持小写字母 a-z (ALPHABET = 26)
 *          支持按需动态扩点，自带 clear()，多测绝对安全
 */
template <int ALPHABET = 26, char BASE = 'a'>
struct Trie {
    struct Node {
        std::array<int, ALPHABET> next{};
        int count = 0;   // 以当前节点为结尾的单词数
        int pass = 0;    // 经过当前节点的单词数
    };

    std::vector<Node> tree;

    Trie() {
        clear();
    }

    void clear() {
        tree.clear();
        tree.emplace_back(); // 0 号节点作为根
    }

    void insert(const std::string& s) {
        int u = 0;
        tree[u].pass++;
        for (char ch : s) {
            int c = ch - BASE;
            if (!tree[u].next[c]) {
                tree[u].next[c] = static_cast<int>(tree.size());
                tree.emplace_back();
            }
            u = tree[u].next[c];
            tree[u].pass++;
        }
        tree[u].count++;
    }

    // 查找单词是否存在
    bool contains(const std::string& s) const {
        return count_exact(s) > 0;
    }

    // 统计确切单词出现的次数
    int count_exact(const std::string& s) const {
        int u = 0;
        for (char ch : s) {
            int c = ch - BASE;
            if (!tree[u].next[c]) return 0;
            u = tree[u].next[c];
        }
        return tree[u].count;
    }

    // 统计前缀出现的次数
    int count_prefix(const std::string& s) const {
        int u = 0;
        for (char ch : s) {
            int c = ch - BASE;
            if (!tree[u].next[c]) return 0;
            u = tree[u].next[c];
        }
        return tree[u].pass;
    }
};
