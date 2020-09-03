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
        "if not(y x+y)<=z then\n",
        "TokenType::Colon;",
        "var ans:  integer is\t42\n",
        "if 0.0 /= 42.424 then\n    g_od := true\nend\n",
        "for i in reverse 1..42 loop\n",
        "type MyRecord record\nvar x : integer\nvar ok : boolean;\nend\n",
        "type // is boolean\nwaffles",
        "var x is 4.\nvar y is .4\n",
    };

    std::vector<std::vector<lexer::Token>> result{
        {

            {  TokenType::If, 1,  1,   "if"}, {TokenType::Identifier, 1,  4, "sausage"}, 
            {TokenType::Loop, 1, 12, "loop"}, {       TokenType::End, 1, 17,     "end"},
            {  TokenType::Is, 1, 21,   "is"}, {TokenType::Identifier, 1, 24,    "near"},
        },
        {

            {     TokenType::If, 1,  1,   "if"}, {TokenType::Identifier, 1,  4, "sausage"}, 
            {   TokenType::Loop, 1, 12, "loop"}, {   TokenType::NewLine, 1, 16,      "\n"},
            {    TokenType::Var, 2,  5,  "var"}, {       TokenType::End, 2,  9,     "end"},
            {     TokenType::Is, 2, 13,   "is"}, {TokenType::Identifier, 2, 16,    "near"},
            {TokenType::NewLine, 2, 20,   "\n"}, {       TokenType::End, 3,  1,     "end"},
            {TokenType::NewLine, 3,  4,   "\n"},
        },
        { 
            {    TokenType::Var, 1,  1, "var"}, { TokenType::Identifier, 1,  5,  "an_int"},
            {  TokenType::Colon, 1, 11,   ":"}, {TokenType::IntegerType, 1, 13, "integer"}, 
            {TokenType::NewLine, 1, 20,  "\n"},
        },
        {
            {        TokenType::If, 1,  1,   "if"}, {       TokenType::Not, 1,  4, "not"},
            { TokenType::OpenParen, 1,  7,    "("}, {TokenType::Identifier, 1,  8,   "y"},
            {TokenType::Identifier, 1, 10,    "x"}, {       TokenType::Add, 1, 11,   "+"},
            {TokenType::Identifier, 1, 12,    "y"}, {TokenType::CloseParen, 1, 13,   ")"},
            {       TokenType::Leq, 1, 14,   "<="}, {TokenType::Identifier, 1, 16,   "z"},
            {      TokenType::Then, 1, 18, "then"}, {   TokenType::NewLine, 1, 22,  "\n"},
        },
        {
            {TokenType::Identifier, 1,  1, "TokenType"}, {     TokenType::Colon, 1, 10,     ":"},
            {     TokenType::Colon, 1, 11,         ":"}, {     TokenType::Identifier, 1, 12, "Colon"},
            { TokenType::Semicolon, 1, 17,         ";"},
        },
        { 
            {    TokenType::Var, 1,  1, "var"}, { TokenType::Identifier, 1,  5,     "ans"},
            {  TokenType::Colon, 1,  8,   ":"}, {TokenType::IntegerType, 1, 11, "integer"}, 
            {     TokenType::Is, 1, 19,  "is"}, {        TokenType::Int, 1, 22,      "42"},
            {TokenType::NewLine, 1, 24,  "\n"},
        },
        {
            {        TokenType::If, 1,  1,   "if"}, {   TokenType::Real, 1,  4,    "0.0"}, 
            {       TokenType::Neq, 1,  8,   "/="}, {   TokenType::Real, 1, 11, "42.424"},
            {      TokenType::Then, 1, 18, "then"}, {TokenType::NewLine, 1, 22,     "\n"},
            {TokenType::Identifier, 2,  5, "g_od"}, { TokenType::Assign, 2, 10,     ":="},
            {      TokenType::True, 2, 13, "true"}, {TokenType::NewLine, 2, 17,     "\n"},
            {       TokenType::End, 3,  1,  "end"}, {TokenType::NewLine, 3,  4,     "\n"},
        },
        {
            {    TokenType::For, 1,  1, "for"}, {TokenType::Identifier, 1,  5,       "i"},
            {     TokenType::In, 1,  7,  "in"}, {   TokenType::Reverse, 1, 10, "reverse"},
            {    TokenType::Int, 1, 18,   "1"}, {   TokenType::TwoDots, 1, 19,      ".."},
            {    TokenType::Int, 1, 21,  "42"}, {      TokenType::Loop, 1, 24,    "loop"},
            {TokenType::NewLine, 1, 28,  "\n"},
        },
        {
            {      TokenType::Type, 1,  1,    "type"}, { TokenType::Identifier, 1,  6, "MyRecord"},
            {    TokenType::Record, 1, 15,  "record"}, {    TokenType::NewLine, 1, 21,       "\n"},
            {       TokenType::Var, 2,  1,     "var"}, { TokenType::Identifier, 2,  5,        "x"},
            {     TokenType::Colon, 2,  7,       ":"}, {TokenType::IntegerType, 2,  9,  "integer"},
            {   TokenType::NewLine, 2, 16,      "\n"}, {        TokenType::Var, 3,  1,      "var"},
            {TokenType::Identifier, 3,  5,      "ok"}, {      TokenType::Colon, 3,  8,        ":"},
            {   TokenType::Boolean, 3, 10, "boolean"}, {  TokenType::Semicolon, 3, 17,        ";"},
            {   TokenType::NewLine, 3, 18,      "\n"}, {        TokenType::End, 4,  1,      "end"},
            {   TokenType::NewLine, 4,  4,      "\n"},
        },
        {
            {      TokenType::Type, 1, 1,    "type"}, {TokenType::NewLine, 1, 19, "\n"},
            {TokenType::Identifier, 2, 1, "waffles"},
        },
        {
            {       TokenType::Var, 1,  1, "var"}, {TokenType::Identifier, 1,  5,   "x"},
            {        TokenType::Is, 1,  7,  "is"}, {      TokenType::Real, 1, 10,  "4."},
            {   TokenType::NewLine, 1, 12,  "\n"}, {       TokenType::Var, 2,  1, "var"},
            {TokenType::Identifier, 2,  5,   "y"}, {        TokenType::Is, 2,  7,  "is"},
            {      TokenType::Real, 2, 10,  ".4"}, {   TokenType::NewLine, 2, 12,  "\n"},
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
                if (tokStream[ti] != result[i][ti]) {
                    FMT_UINFO("got {}", testing::TokenToString(tokStream[ti]));
                    FMT_UINFO("want {}", testing::TokenToString(result[i][ti]));
                }
                CHECK(tokStream[ti] == result[i][ti]);
            }
        }
    }
}
