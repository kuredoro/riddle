#pragma once

#include "trie.hpp"
#include "token.hpp"

namespace lexer {

//extern common::Trie<TokenType> trie;

class Lexer {
public:
    Lexer(std::string src) : buf(src) {

    }

    Token Next() {
        return {};
    }

private:
    std::string buf;
    
};

} // namespace lexer
