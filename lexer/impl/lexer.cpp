#include "trie.hpp"
#include "token.hpp"
#include "lexer.hpp"
#include "token.hpp"
#include <vector>
#include <string>
#include "trie.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cctype>


namespace lexer {


common::Trie<TokenType> g_keywordTrie{
    {"var", TokenType::Var},
    {"type", TokenType::Type},
    {"routine", TokenType::Routine},
    {"is", TokenType::Is},
    {"integer", TokenType::IntegerType},
    {"real", TokenType::RealType},
    {"boolean", TokenType::Boolean},
    {"record", TokenType::Record},
    {"array", TokenType::Array},
    {"true", TokenType::True},
    {"false", TokenType::False},
    {"while", TokenType::While},
    {"for", TokenType::For},
    {"loop", TokenType::Loop},
    {"end", TokenType::End},
    {"reverse", TokenType::Reverse},
    {"in", TokenType::In},
    {"if", TokenType::If},
    {"then", TokenType::Then},
    {"else", TokenType::Else},
    {"and", TokenType::And},
    {"or", TokenType::Or},
    {"xor", TokenType::Xor},
};

common::Trie<TokenType> g_operatorTrie{
    {"//", TokenType::Comment},
    {"<",  TokenType::Less},
    {">",  TokenType::Greater},
    {"=",  TokenType::Eq},
    {"<=", TokenType::Leq},
    {">=", TokenType::Geq},
    {"/=", TokenType::Neq},
    {":=", TokenType::Assign},
    {"+",  TokenType::Add},
    {"-",  TokenType::Sub},
    {"*",  TokenType::Mul},
    {"/",  TokenType::Div},
    {"%",  TokenType::Mod},
    {"(",  TokenType::OParen},
    {"[",  TokenType::OBrack},
    {",",  TokenType::Comma},
    {".",  TokenType::Dot},
    {"..", TokenType::TwoDots},
    {")",  TokenType::CParen},
    {"]",  TokenType::CBrack},
    {";",  TokenType::Semicolon},
    {":",  TokenType::Colon},
};


size_t Lexer::skipWhile(size_t bufPos, std::function<bool(char)> pred, size_t len) {
    while (bufPos + len < m_buf.size() && pred(m_buf[bufPos + len]))
        len++;

    return len;
}


Token Lexer::Next() {

    // Ignore spaces
    m_pos += skipWhile(m_pos, Lexer::isspace);

    if (m_pos == m_buf.size()) {
        return { .type = TokenType::Eof };
    }

    Token tok{
        .type = TokenType::Illegal,
        .line = m_lineNum,
        .column = m_pos - m_lastAfterNewLine,
    };

    // If alpha -> either a keyword or identifier -> read until space
    if (Lexer::isidstart(m_buf[m_pos])) {

        auto len = skipWhile(m_pos, Lexer::isidsuf, 1);

        tok.lit = m_buf.substr(m_pos, len);
        tok.type = g_keywordTrie[tok.lit].value_or(TokenType::Ident);

        m_pos += len;

        return tok;
    }

    if (std::isdigit(m_buf[m_pos])) {

        auto truncLen = skipWhile(m_pos, Lexer::isdigit, 1);

        size_t fracLen = 0;
        if (m_pos + truncLen < m_buf.size() && m_buf[m_pos + truncLen] == '.') {
            fracLen = skipWhile(m_pos + truncLen + 1, Lexer::isdigit);
        }

        if (fracLen != 0) {
            truncLen += 1 + fracLen;
            tok.type = TokenType::Real;
        } else {
            tok.type = TokenType::Int;
        }

        tok.lit = m_buf.substr(m_pos, truncLen);

        m_pos += truncLen;

        return tok;
    }

    if (m_buf[m_pos] == '\n') {
        tok.type = TokenType::NewLine;
        tok.lit = "\n";

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

    tok.lit = m_buf.substr(m_pos, lastTerminalLength);

    m_pos += lastTerminalLength;

    // I'll use the fact that // can be considered an operator
    // If we got it, then we'll just skip to the next newline and start over
    // as if nothing happened.
    if (tok.type == TokenType::Comment) {
        m_pos += skipWhile(m_pos, [](char c){ return c != '\n'; });

        return Next();
    }

    return tok;
}

} // namespace lexer
