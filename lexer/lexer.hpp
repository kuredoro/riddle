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

    Token Next() {

        // Ignore spaces
        for (; m_pos < m_buf.size() && Lexer::isspace(m_buf[m_pos]); m_pos++) {}

        if (m_pos == m_buf.size()) {
            return { .type = TokenType::Eof };
        }

        Token tok{
            .type = TokenType::Error,
            .line = m_lineNum,
            .srcPos = m_pos - m_lastAfterNewLine,
        };

        // If alpha -> either a keyword or identifier -> read until space
        if (Lexer::isidstart(m_buf[m_pos])) {
            size_t len = 1;
            for (; m_pos + len < m_buf.size() && Lexer::isidsuf(m_buf[m_pos + len]); len++) {}

            tok.image = m_buf.substr(m_pos, len);
            tok.type = g_keywordTrie.Find(tok.image).value_or(TokenType::Identifier);
 
            m_pos += len;

            return tok;
        }

        if (std::isdigit(m_buf[m_pos])) {
            size_t truncLen = 1;
            for(; m_pos + truncLen < m_buf.size() && std::isdigit(m_buf[m_pos + truncLen]); truncLen++) {}

            size_t fracLen = 0;
            if (m_pos + truncLen < m_buf.size() && m_buf[m_pos + truncLen] == '.') {
                size_t offset = m_pos + truncLen + 1;
                for (; offset + fracLen < m_buf.size() && std::isdigit(m_buf[offset + fracLen]); fracLen++) {}
            }

            if (fracLen != 0) {
                truncLen += 1 + fracLen;
                tok.type = TokenType::RealLiteral;
            } else {
                tok.type = TokenType::IntegerLiteral;
            }

            tok.image = m_buf.substr(m_pos, truncLen);

            m_pos += truncLen;

            return tok;
        }

        if (m_buf[m_pos] == '\n') {
            tok.type = TokenType::NewLine;
            tok.image = "\n";

            m_pos++;
            m_lastAfterNewLine = m_pos;
            m_lineNum++;

            return tok;
        }

        // If not -> some operator, do maximal munch
        size_t len = 0;
        size_t lastTerminalLength = 0;

        auto head = g_operatorTrie.Head();
        while (m_pos + len < m_buf.size() && head.Valid()) {
            head.Next(m_buf[m_pos + len]);
            len++;

            // Basically, if this will never execute, the tok.type will be error
            // and positional data already filled.
            if (head.Terminal()) {
                lastTerminalLength = len;
                tok.type = *head.Value();
            }
        }

        tok.image = m_buf.substr(m_pos, lastTerminalLength);

        m_pos += lastTerminalLength;

        return tok;
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
