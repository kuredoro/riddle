#pragma once

#include "trie.hpp"
#include "token.hpp"
#include <cctype>

namespace lexer {

extern common::Trie<TokenType> g_keywordTrie;

class Lexer {
public:
    Lexer(std::string src) : m_buf(src) {}

    Token Next() {

        // Ignore spaces
        for (; m_pos < m_buf.size() && Lexer::isspace(m_buf[m_pos]); m_pos++) {}

        if (m_pos == m_buf.size()) {
            return { .type = TokenType::Eof };
        }

        // If alpha -> either a keyword or identifier -> read until space
        if (Lexer::isidstart(m_buf[m_pos])) {
            int len = 1;
            for (; m_pos + len < m_buf.size() && Lexer::isidsuf(m_buf[m_pos + len]); len++) {}

            Token tok;

            tok.image = m_buf.substr(m_pos, len);
            tok.line = m_lineNum;
            tok.srcPos = m_pos - m_lastAfterNewLine;
            tok.type = g_keywordTrie.Find(tok.image).value_or(TokenType::Identifier);
 
            m_pos += len;

            return tok;
        }

        if (m_buf[m_pos] == '\n') {
            Token tok;
            tok.type = TokenType::NewLine;
            tok.line = m_lineNum;
            tok.srcPos = m_pos - m_lastAfterNewLine;
            tok.image = "\n";

            m_pos++;
            m_lastAfterNewLine = m_pos;
            m_lineNum++;

            return tok;
        }

        // If not -> some operator, do maximal munch
        
        // Last failed? -> Error Token
        
        return {};
    }

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
