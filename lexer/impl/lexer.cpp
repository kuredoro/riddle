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
    {"var", TokenType::VarDecl},
    {"type", TokenType::TypeDecl},
    {"routine", TokenType::RoutineDecl},
    {"is", TokenType::Is},
    {"integer", TokenType::IntegerType},
    {"real", TokenType::RealType},
    {"boolean", TokenType::BooleanType},
    {"record", TokenType::RecordType},
    {"array", TokenType::ArrayType},
    {"true", TokenType::True},
    {"false", TokenType::False},
    {"while", TokenType::WhileLoop},
    {"for", TokenType::ForLoop},
    {"loop", TokenType::LoopBegin},
    {"end", TokenType::End},
    {"reverse", TokenType::ReverseRange},
    {"in", TokenType::InRange},
    {"if", TokenType::If},
    {"else", TokenType::Else},
    {"and", TokenType::AndLogic},
    {"or", TokenType::OrLogic},
    {"xor", TokenType::XorLogic},
};


} // namespace lexer
