#include "trie.hpp"
#include "token.hpp"
#include "trie.hpp"
#pragma once

#include "trie.hpp"
#include "token.hpp"
#include <cctype>

namespace lexer {


extern common::Trie<TokenType> g_keywordTrie;
extern common::Trie<TokenType> g_operatorTrie;

class Lexer {
public:
    Lexer(std::string src) : m_buf(src) {}

    Token Next();

private:
    std::string m_buf;
    size_t m_pos = 0, m_lastAfterNewLine = 0;
    size_t m_lineNum = 1;

    bool isspace(char ch) {
        return ch != '\n' && std::isspace(ch);
    }

    bool isidstart(char ch) {
        return ch == '_' || std::isalpha(ch);
    }

    bool isidsuf(char ch) {
        return ch == '_' || std::isalnum(ch);
    }
};


} // namespace lexer
