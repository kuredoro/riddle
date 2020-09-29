#pragma once

#include "token.hpp"
#include "trie.hpp"
#include <cctype>
#include <functional>
#include <string_view>

namespace lexer {

extern common::Trie<TokenType> g_keywordTrie;
extern common::Trie<TokenType> g_operatorTrie;

class Lexer {
public:
    Lexer(std::string_view src) : m_buf(src) { currentToken = scanNext(); }

    Token Next();

    Token Peek();

private:
    std::string_view m_buf;
    size_t m_pos = 0;
    size_t m_lineStartPos = 0;
    size_t m_lineNum = 1;
    Token currentToken;
    bool consumedEof = false;

    static bool isSpace(char ch) { return ch != '\n' && std::isspace(ch); }

    static bool isDigit(char ch) { return std::isdigit(ch); }

    static bool isIdentStart(char ch) { return ch == '_' || std::isalpha(ch); }

    static bool isIdentSuf(char ch) { return ch == '_' || std::isalnum(ch); }

    size_t skipWhile(size_t bufPos, std::function<bool(char)> pred);

    Token scanNext();

    char peek(size_t offset = 1);
};

} // namespace lexer
