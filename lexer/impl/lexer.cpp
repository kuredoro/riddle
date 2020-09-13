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
    {"not", TokenType::Not},
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
    {"(",  TokenType::OpenParen},
    {"[",  TokenType::OpenBrack},
    {",",  TokenType::Comma},
    {".",  TokenType::Dot},
    {"..", TokenType::TwoDots},
    {")",  TokenType::CloseParen},
    {"]",  TokenType::CloseBrack},
    {";",  TokenType::Semicolon},
    {":",  TokenType::Colon},
};


size_t Lexer::skipWhile(size_t bufPos, std::function<bool(char)> pred) {
    size_t len = 0;
    while (bufPos + len < m_buf.size() && pred(m_buf[bufPos + len]))
        len++;

    return len;
}

char Lexer::peek(size_t offset) {
    if (m_pos + offset >= m_buf.size()) {
        return 0;
    }

    return m_buf[m_pos + offset];
}

Token Lexer::Peek () {
    if (currentToken == nullptr) {
        scanNext();
    } else if (currentToken->type == TokenType::Eof) {
        throw "End-of-file already reached";
    }
    return *currentToken;
}

Token Lexer::Next () {
    Token ret = Peek();
    if (ret.type != TokenType::Eof) {
        currentToken = nullptr;
    }
    return ret;
}

void Lexer::scanNext() {
    // Ignore spaces
    m_pos += skipWhile(m_pos, isSpace);

    if (m_pos == m_buf.size()) {
        currentToken = std::make_shared<Token>(Token { .type = TokenType::Eof });
        return;
    }

    Token tok{
        .type = TokenType::Illegal,
        .pos = {
            .line = m_lineNum,
            .column = m_pos - m_lineStartPos + 1,
        }
    };

    // If alpha -> either a keyword or identifier -> read until not
    if (isIdentStart(m_buf[m_pos])) {

        auto len = 1 + skipWhile(m_pos + 1, isIdentSuf);

        tok.lit = m_buf.substr(m_pos, len);
        tok.type = g_keywordTrie[tok.lit].value_or(TokenType::Identifier);

        m_pos += len;

        currentToken = std::make_shared<Token>(tok);
        return;
    }

    if (isDigit(m_buf[m_pos]) || (m_buf[m_pos] == '.' && isDigit(peek()))) {

        auto truncLen = skipWhile(m_pos, isDigit);

        size_t fracLen = (size_t)-1;
        if (peek(truncLen) == '.') {
            fracLen = skipWhile(m_pos + truncLen + 1, isDigit);

            // Two dots case
            if (fracLen == 0 && peek(truncLen + 1) == '.') {
                fracLen = (size_t)-1;
            }
        }

        if (fracLen != (size_t)-1) {
            truncLen += 1 + fracLen;
            tok.type = TokenType::Real;
        } else {
            tok.type = TokenType::Int;
        }

        tok.lit = m_buf.substr(m_pos, truncLen);

        m_pos += truncLen;

        currentToken = std::make_shared<Token>(tok);
        return;
    }

    if (m_buf[m_pos] == '\n') {
        tok.type = TokenType::NewLine;
        tok.lit = "\n";

        m_lineStartPos = m_pos + 1;

        m_pos++;
        m_lineNum++;

        currentToken = std::make_shared<Token>(tok);
        return;
    }

    // If not -> some operator, do maximal munch
    size_t len = 0;
    size_t lastTerminalLength = 0;

    auto head = g_operatorTrie.Head();
    while (peek(len) && head.Valid()) {
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

        scanNext();
        return;
    }

    currentToken = std::make_shared<Token>(tok);
}

} // namespace lexer
