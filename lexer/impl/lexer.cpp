#include "trie.hpp"
#include "token.hpp"
#include "lexer.hpp"


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
    {"then", TokenType::Then},
    {"else", TokenType::Else},
    {"and", TokenType::AndLogic},
    {"or", TokenType::OrLogic},
    {"xor", TokenType::XorLogic},
};

common::Trie<TokenType> g_operatorTrie{
    {"<", TokenType::LessComp},
    {"<=", TokenType::LeqComp},
    {">", TokenType::GreaterComp},
    {">=", TokenType::GeqComp},
    {"=", TokenType::EqComp},
    {"/=", TokenType::NeqComp},
    {":=", TokenType::AssignmentOp},
    {"*", TokenType::MultOp},
    {"/", TokenType::DivOp},
    {"%", TokenType::ModOp},
    {"+", TokenType::AddOp},
    {"-", TokenType::SubOp},
    {".", TokenType::Dot},
    {"..", TokenType::TwoDots},
    {",", TokenType::Comma},
    {"(", TokenType::ParenOpen},
    {")", TokenType::ParenClose},
    {"[", TokenType::BracketOpen},
    {"]", TokenType::BracketClose},
    {":", TokenType::Colon},
    {";", TokenType::Semicolon},
};

} // namespace lexer
