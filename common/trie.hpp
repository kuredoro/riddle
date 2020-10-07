#pragma once
#include "fmt/format.h"
#include <initializer_list>
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace common {

template <typename T> class Trie;

template <typename T> struct TriePayload {
    std::string key;
    T value;
};

template <typename T>
std::ostream& operator<<(std::ostream& out, const TriePayload<T>& payload) {
    out << "{'" << payload.key << "', " << payload.value << "}";
    return out;
}

template <typename T> class TrieCursor {
public:
    using Node = typename Trie<T>::Node;

    TrieCursor(const std::vector<Node>* const ref, size_t id)
        : m_tree_ref(ref), m_id(id) {}

    void Next(char ch);

    bool Valid() const;
    bool Terminal() const;
    std::optional<T> Value() const;

private:
    const std::vector<Node>* const m_tree_ref;
    size_t m_id = (size_t)-1;
};

template <typename T> void TrieCursor<T>::Next(char ch) {
    if (!Valid()) {
        return;
    }

    if (m_tree_ref->at(m_id).next.count(ch) == 0) {
        m_id = (size_t)-1;
        return;
    }

    m_id = m_tree_ref->at(m_id).next.at(ch);
}

template <typename T> bool TrieCursor<T>::Valid() const {
    return m_id != (size_t)-1;
}

template <typename T> bool TrieCursor<T>::Terminal() const {
    if (!Valid()) {
        return false;
    }

    return (bool)m_tree_ref->at(m_id).value;
}

template <typename T> std::optional<T> TrieCursor<T>::Value() const {
    if (!Valid()) {
        return std::nullopt;
    }

    return m_tree_ref->at(m_id).value;
}

/**
 * Trie class is a string dictionary implemented via trie data structure.
 * This enables for O(n) lookup times for the entries and also allows
 * to incrementally traverse the prefix tree via a cursor.
 */
template <typename T> class Trie {
public:
    Trie() = default;
    Trie(std::initializer_list<TriePayload<T>> initList);

    void Add(const std::string_view key, T value);

    std::optional<T> Find(const std::string_view key) const;
    std::optional<T> operator[](const std::string_view key) const {
        return Find(key);
    }

    TrieCursor<T> Head() const;

    friend class TrieCursor<T>;

    template <typename U>
    friend std::ostream& operator<<(std::ostream& out, const Trie<U>& trie);

private:
    struct Node {
        std::optional<T> value;
        std::map<char, size_t> next;
    };

    std::vector<Node> m_tree;
};

template <typename T>
Trie<T>::Trie(std::initializer_list<TriePayload<T>> initList) {
    for (auto it = initList.begin(); it != initList.end(); it++) {
        Add(it->key, it->value);
    }
}

template <typename T> void Trie<T>::Add(std::string_view key, T value) {
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

template <typename T> TrieCursor<T> Trie<T>::Head() const {
    return {&m_tree, 0};
}

template <typename T>
std::ostream& operator<<(std::ostream& out, const Trie<T>& trie) {

    out << "{";
    for (size_t head = 0; head < trie.m_tree.size(); head++) {
        auto& node = trie.m_tree[head];
        out << "\n  [" << head << "] ";

        if (node.value) {
            out << "Value : " << *node.value << "\n";
        } else {
            out << "Value : nil\n";
        }

        if (node.next.empty()) {
            out << "    empty\n";
        }

        for (auto& [k, v] : node.next) {
            out << "    " << k << " -> " << v << "\n";
        }
    }

    out << "}\n";

    return out;
}

} // namespace common
