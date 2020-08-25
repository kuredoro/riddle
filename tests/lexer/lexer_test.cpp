#include "catch2/catch.hpp"
#include "catch_helpers.hpp"
#include "fmt/format.h"
#include "strings.hpp"
#include "lexer.hpp"
#include "token.hpp"

namespace testing {


void PrintTokenStream(const std::vector<lexer::Token>& toks) {
    std::string out;
    for (auto& tok : toks) {
        out += fmt::format("{{{}:{} {} ({})}} ", tok.line, tok.srcPos, tok.image, tok.type);
    }
    FMT_UINFO("{}", out);
}


} // namespace testing

SCENARIO("Lexer is fed source code") {

    using lexer::TokenType;
    
    std::vector<std::string> code{
        "var xxx__35: integer is  4.5\n",
    };

    std::vector<std::vector<lexer::Token>> result{
        { {TokenType::VarDecl, 1, 1, "var"}, {TokenType::Identifier, 1, 5, "xxx__35"},
          {TokenType::Colon, 1, 12, ":"}, {TokenType::IntegerType, 1, 14, "integer"}, 
          {TokenType::Is, 1, 22, "is"}, {TokenType::RealLiteral, 1, 26, "4.5"},
        },
    };

    for (size_t i = 0; i < code.size(); i++) {

        GIVEN(common::replaceAll(code[i], "\n", "\\n")) {

            lexer::Lexer lx{code[i]};

            std::vector<lexer::Token> tokStream;
            for (auto tok = lx.Next(); tok.type != TokenType::Nil; tok = lx.Next()) {
                tokStream.push_back(tok);
            }

            if (tokStream.size() != result.size()) {
                FMT_INFO("Got, ");
                testing::PrintTokenStream(tokStream);
                FMT_INFO("Want, ");
                testing::PrintTokenStream(result[i]);
                REQUIRE(tokStream.size() == result.size());
            }
        }
    }

}
