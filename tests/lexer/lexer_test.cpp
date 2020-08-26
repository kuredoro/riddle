#include "catch2/catch.hpp"
#include "catch_helpers.hpp"
#include "fmt/format.h"
#include "strings.hpp"
#include "lexer.hpp"
#include "token.hpp"

namespace testing {

std::string TokenToString(const lexer::Token& tok) {
    auto str = common::replaceAll(tok.image, "\n", "\\n");
    return fmt::format("{{{}:{} {} ({})}} ", tok.line, tok.srcPos, str, tok.type);
}

void PrintTokenStream(const std::vector<lexer::Token>& toks) {
    std::string out;
    for (auto& tok : toks) {
        out += TokenToString(tok);
    }
    FMT_UINFO("{}", out);
}


} // namespace testing

bool operator!=(const lexer::Token& a, const lexer::Token& b) {
    return a.type != b.type || a.line != b.line || a.srcPos != b.srcPos || a.image != b.image;
}

bool operator==(const lexer::Token& a, const lexer::Token& b) {
    return !(a != b);
}

SCENARIO("Lexer is fed source code") {

    using lexer::TokenType;
    
    std::vector<std::string> code{
        "if sausage loop end is near",
        "if sausage loop\n    var end is near\nend\n",
        "var an_int: integer\n",
        //"var xxx__35: integer is  4.5\n",
    };

    std::vector<std::vector<lexer::Token>> result{
        {
            {TokenType::If, 1, 0, "if"}, {TokenType::Identifier, 1, 3, "sausage"}, {TokenType::LoopBegin, 1, 11, "loop"},
            {TokenType::End, 1, 16, "end"}, {TokenType::Is, 1, 20, "is"}, {TokenType::Identifier, 1, 23, "near"},
        },
        {
            {TokenType::If, 1, 0, "if"}, {TokenType::Identifier, 1, 3, "sausage"}, {TokenType::LoopBegin, 1, 11, "loop"},
            {TokenType::NewLine, 1, 15, "\n"},
            {TokenType::VarDecl, 2, 4, "var"}, {TokenType::End, 2, 8, "end"}, {TokenType::Is, 2, 12, "is"},
            {TokenType::Identifier, 2, 15, "near"}, {TokenType::NewLine, 2, 19, "\n"},
            {TokenType::End, 3, 0, "end"}, {TokenType::NewLine, 3, 3, "\n"},
        },
        { 
            {TokenType::VarDecl, 1, 1, "var"}, {TokenType::Identifier, 1, 5, "an_int"},
            {TokenType::Colon, 1, 11, ":"}, {TokenType::IntegerType, 1, 13, "integer"}, 
            {TokenType::NewLine, 1, 20, "\n"},
        },
        { 
            {TokenType::VarDecl, 1, 1, "var"}, {TokenType::Identifier, 1, 5, "xxx__35"},
            {TokenType::Colon, 1, 12, ":"}, {TokenType::IntegerType, 1, 14, "integer"}, 
            {TokenType::Is, 1, 22, "is"}, {TokenType::RealLiteral, 1, 26, "4.5"},
        },
    };

    for (size_t i = 0; i < code.size(); i++) {

        GIVEN(common::replaceAll(code[i], "\n", "\\n")) {

            lexer::Lexer lx{code[i]};

            std::vector<lexer::Token> tokStream;
            for (auto tok = lx.Next(); tok.type != TokenType::Eof; tok = lx.Next()) {
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
                    FMT_INFO("got {}", testing::TokenToString(tokStream[ti]))
                    FMT_INFO("want {}", testing::TokenToString(result[i][ti]));
                }
                CHECK(tokStream[ti] == result[i][ti]);
            }
        }
    }

}
