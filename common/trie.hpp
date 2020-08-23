#pragma once
#include <optional>
#include <string_view>
#include <string>
#include <map>
#include <vector>
#include "fmt/format.h"

namespace common {


template <typename T>
class Trie {
public:
    Trie() {
        m_tree.emplace_back();
    }

    void Add(const std::string_view key, T value);
    std::optional<T> Find(const std::string_view key) const;

    std::vector<std::string> PrintContents() const;

private:

    struct Node {
        std::optional<T> value;
        std::map<char, size_t> next;
    };

    std::vector<Node> m_tree;
};

template <typename T>
void Trie<T>::Add(std::string_view key, T value) {
    size_t head = 0;
    size_t chi = 0;
    for (; chi < key.size(); chi++) {
        char ch = key[chi];

        if (m_tree[head].next.count(ch) == 0) {
            break;
        }

        head = m_tree[head].next[ch];
    }

    size_t ni = m_tree.size();

    m_tree.resize(m_tree.size() + key.size() - chi);

    for (; ni < m_tree.size(); ni++, chi++) {
        char ch = key[chi];
        m_tree[head].next[ch] = ni;
        head = ni;
    }

    m_tree[head].value = {value};
}

template <typename T>
std::optional<T> Trie<T>::Find(const std::string_view key) const {
    size_t head = 0;
    for (auto ch : key) {
        if (m_tree[head].next.count(ch) == 0) {
            return std::nullopt;
        }

        head = m_tree[head].next.at(ch);
    }

    return m_tree[head].value;
}

template <typename T>
std::vector<std::string> Trie<T>::PrintContents() const {
    std::vector<std::string> out;
    for (size_t head = 0; head < m_tree.size(); head++) {
        auto& node = m_tree[head];
        std::string nodeInfo = fmt::format("[{:2}] ", head);

        if (node.value) {
            nodeInfo += fmt::format("Value : {}\n", *node.value);
        } else {
            nodeInfo += "Value : nil\n";
        }

        if (node.next.empty()) {
            nodeInfo += "    empty\n";
        }

        for (auto& [k, v] : node.next) {
            nodeInfo += fmt::format("    {} : {}\n", k, v);
        }

        nodeInfo += "\n";
        out.push_back(nodeInfo);
    }

    return out;
}


} // namespace common
