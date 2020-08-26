#include "trie.hpp"
#include "token.hpp"
#include "trie.hpp"
#pragma once

namespace lexer
{
    std::vector<Token> read(std::string);
    common::Trie<TokenType> getTrie();
    std::vector<Token> splitLine(std::string);
    void freeResult();
} // namespace lexer