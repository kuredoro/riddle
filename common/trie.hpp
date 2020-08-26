#pragma once
#include <initializer_list>
#include <optional>
#include <string_view>
#include <string>
#include <map>
#include <vector>
#include "fmt/format.h"

namespace common {


template <typename T>
struct TriePayload {
    std::string key;
    T value;
};

template <typename T>
class TrieConstIterator;

template <typename T>
class Trie {
public:
    Trie() = default;
    Trie(std::initializer_list<TriePayload<T>> initList);

    void Add(const std::string_view key, T value);
    std::optional<T> Find(const std::string_view key) const;

    TrieConstIterator<T> Head() const {
        return {&m_tree, 0};
    }

    std::vector<std::string> PrintContents() const;

private:

    friend struct TrieConstIterator<T>;

    struct Node {
        std::optional<T> value;
        std::map<char, size_t> next;
    };

    std::vector<Node> m_tree;
};


template <typename T>
class TrieConstIterator {
public:

    using Node = typename Trie<T>::Node;

    TrieConstIterator(const std::vector<Node>* const ref, size_t id)
        : m_tree_ref(ref), m_id(id)
    {}

    void Next(char ch) {
        if (m_id == -1) {
            return;
        }

        if (m_tree_ref->at(m_id).next.count(ch) == 0) {
            m_id = -1;
            return;
        }

        m_id = m_tree_ref->at(m_id).next.at(ch);
    }

    bool Valid() {
        return m_id != -1;
    }

    bool Terminal() {
        if (!Valid()) {
            return false;
        }       

        return *m_tree_ref->at(m_id).value;
    }

private:
    const std::vector<Node>* const m_tree_ref;
    int m_id = -1;
};


template <typename T>
Trie<T>::Trie(std::initializer_list<TriePayload<T>> initList) {
    for (auto it = initList.begin(); it != initList.end(); it++) {
        Add(it->key, it->value);
    }
}

template <typename T>
void Trie<T>::Add(std::string_view key, T value) {
    if (m_tree.empty()) {
        m_tree.emplace_back();
    }

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
    if (m_tree.empty()) {
        return std::nullopt;
    }

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
