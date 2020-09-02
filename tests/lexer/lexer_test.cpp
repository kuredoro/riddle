#include "catch2/catch.hpp"
#include "catch_helpers.hpp"
#include "fmt/format.h"
#include "strings.hpp"
#include "lexer.hpp"
#include "token.hpp"

namespace testing {

std::string TokenToString(const lexer::Token& tok) {
    auto str = common::replaceAll(tok.lit, "\n", "\\n");
    return fmt::format("{{{}:{} {} ({})}} ", tok.line, tok.column, str, tok.type);
}

void PrintTokenStream(const std::vector<lexer::Token>& toks) {
    std::string out;
    for (auto& tok : toks) {
        out += TokenToString(tok);
    }
    FMT_UINFO("{}", out);
}


} // namespace testing


SCENARIO("Lexer is fed source code") {

    using lexer::TokenType;
    
    std::vector<std::string> code{
        "if sausage loop end is near",
        "if sausage loop\n    var end is near\nend\n",
        "var an_int: integer\n",
        "if (x+y)<=z then\n",
        "TokenType::Colon;",
        "var ans:  integer is\t42\n",
        "if 0.0 /= 42.424 then\n    g_od := true\nend\n",
        "for i in reverse 1..42 loop\n",
        "type MyRecord record\nvar x : integer\nvar ok : boolean;\nend\n",
        "type // is boolean\nwaffles",
    };

    std::vector<std::vector<lexer::Token>> result{
        {
            {       TokenType::If, 1,  0,   "if"}, {TokenType::Ident, 1,  3, "sausage"}, 
            {TokenType::Loop, 1, 11, "loop"}, {       TokenType::End, 1, 16,     "end"},
            {       TokenType::Is, 1, 20,   "is"}, {TokenType::Ident, 1, 23,    "near"},
        },
        {
            {       TokenType::If, 1,  0,   "if"}, {TokenType::Ident, 1,  3, "sausage"}, 
            {TokenType::Loop, 1, 11, "loop"}, {   TokenType::NewLine, 1, 15,      "\n"},
            {  TokenType::Var, 2,  4,  "var"}, {       TokenType::End, 2,  8,     "end"},
            {       TokenType::Is, 2, 12,   "is"}, {TokenType::Ident, 2, 15,    "near"},
            {  TokenType::NewLine, 2, 19,   "\n"}, {       TokenType::End, 3,  0,     "end"},
            {  TokenType::NewLine, 3,  3,   "\n"},
        },
        { 
            {TokenType::Var, 1,  0, "var"}, { TokenType::Ident, 1,  4,  "an_int"},
            {  TokenType::Colon, 1, 10,   ":"}, {TokenType::IntegerType, 1, 12, "integer"}, 
            {TokenType::NewLine, 1, 19,  "\n"},
        },
        {
            {        TokenType::If, 1,  0,   "if"}, { TokenType::OParen, 1,  3,  "("}, 
            {TokenType::Ident, 1,  4,    "x"}, {     TokenType::Add, 1,  5,  "+"},
            {TokenType::Ident, 1,  6,    "y"}, {TokenType::CParen, 1,  7,  ")"},
            {   TokenType::Leq, 1,  8,   "<="}, {TokenType::Ident, 1, 10,  "z"},
            {      TokenType::Then, 1, 12, "then"}, {   TokenType::NewLine, 1, 16, "\n"},
        },
        {
            {TokenType::Ident, 1,  0, "TokenType"}, {     TokenType::Colon, 1,  9,     ":"},
            {     TokenType::Colon, 1, 10,         ":"}, {TokenType::Ident, 1, 11, "Colon"},
            { TokenType::Semicolon, 1, 16,         ";"},
        },
        { 
            {TokenType::Var, 1,  0, "var"}, {    TokenType::Ident, 1,  4,     "ans"},
            {  TokenType::Colon, 1,  7,   ":"}, {   TokenType::IntegerType, 1, 10, "integer"}, 
            {     TokenType::Is, 1, 18,  "is"}, {TokenType::Int, 1, 21,      "42"},
            {TokenType::NewLine, 1, 23,  "\n"},
        },
        {
            {        TokenType::If, 1,  0,   "if"}, { TokenType::Real, 1,  3,    "0.0"}, 
            {   TokenType::Neq, 1,  7,   "/="}, { TokenType::Real, 1, 10, "42.424"},
            {      TokenType::Then, 1, 17, "then"}, {     TokenType::NewLine, 1, 21,     "\n"},
            {TokenType::Ident, 2,  4, "g_od"}, {TokenType::Assign, 2,  9,     ":="},
            {      TokenType::True, 2, 12, "true"}, {     TokenType::NewLine, 2, 16,     "\n"},
            {       TokenType::End, 3,  0,  "end"}, {     TokenType::NewLine, 3,  3,     "\n"},
        },
        {
            {       TokenType::For, 1,  0, "for"}, {  TokenType::Ident, 1,  4,       "i"},
            {       TokenType::In, 1,  6,  "in"}, {TokenType::Reverse, 1,  9, "reverse"},
            {TokenType::Int, 1, 17,   "1"}, {     TokenType::TwoDots, 1, 18,      ".."},
            {TokenType::Int, 1, 20,  "42"}, {   TokenType::Loop, 1, 23,    "loop"},
            {       TokenType::NewLine, 1, 27,  "\n"},
        },
        {
            {   TokenType::Type, 1,  0,    "type"}, { TokenType::Ident, 1,  5, "MyRecord"},
            { TokenType::Record, 1, 14,  "record"}, {    TokenType::NewLine, 1, 20,       "\n"},
            {    TokenType::Var, 2,  0,     "var"}, { TokenType::Ident, 2,  4,        "x"},
            {      TokenType::Colon, 2,  6,       ":"}, {TokenType::IntegerType, 2,  8,  "integer"},
            {    TokenType::NewLine, 2, 15,      "\n"}, {    TokenType::Var, 3,  0,      "var"},
            { TokenType::Ident, 3,  4,      "ok"}, {      TokenType::Colon, 3,  7,        ":"},
            {TokenType::Boolean, 3,  9, "boolean"}, {  TokenType::Semicolon, 3, 16,        ";"},
            {    TokenType::NewLine, 3, 17,      "\n"}, {        TokenType::End, 4,  0,      "end"},
            {    TokenType::NewLine, 4,  3,      "\n"},
        },
        {
            {  TokenType::Type, 1, 0,    "type"}, {TokenType::NewLine, 1, 18, "\n"},
            {TokenType::Ident, 2, 0, "waffles"},
        },
    };

    for (size_t i = 0; i < code.size(); i++) {

        GIVEN(common::replaceAll(code[i], "\n", "\\n")) {

            lexer::Lexer lx{code[i]};

            std::vector<lexer::Token> tokStream;
            for (auto tok = lx.Next(); tok.type != TokenType::Eof && tok.type != TokenType::Illegal; tok = lx.Next()) {
                tokStream.push_back(tok);
            }

            if (tokStream.size() != result[i].size()) {
                FMT_UINFO("got ");
                testing::PrintTokenStream(tokStream);
                FMT_UINFO("want ");
                testing::PrintTokenStream(result[i]);
            }
            REQUIRE(tokStream.size() == result[i].size());

            for (size_t ti = 0; ti < result[i].size(); ti++) {
                if (tokStream[ti] == result[i][ti]) {
                    FMT_UINFO("got {}", testing::TokenToString(tokStream[ti]));
                    FMT_UINFO("want {}", testing::TokenToString(result[i][ti]));
                }
                CHECK(tokStream[ti] == result[i][ti]);
            }
        }
    }
}
