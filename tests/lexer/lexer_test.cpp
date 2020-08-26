#include "catch2/catch.hpp"
#include "lexer.hpp"
#include "token.hpp"
#include <string>
#include "fmt/format.h"
#include "trie.hpp"
#include "catch_helpers.hpp"
#include <vector>
#include <iostream>

namespace testing
{

    void checkInTrie(std::string key, common::Trie<TokenType>
                                          trie)
    {
        auto result = trie.Find(key);
        CHECK_MESSAGE((bool)result,
                      "expected to find key \"{}\", but didn't", key);
    }

    void checkVectorTypeEquality(std::vector<TokenType> expected,
                                 std::vector<Token> real)
    {
        bool res = expected.size() == real.size();
        CHECK_MESSAGE(res,
                      "expected to get vectors of equal size");

        for (size_t i = 0; i < expected.size(); i++)
        {
            bool result = real[i].type == expected[i];
            CHECK_MESSAGE(result,
                          "expected to find type {}, but find {} (\"{}\")", expected[i], real[i].type, real[i].image);
        }
    }

} // namespace testing

void checkTokens(std::vector<std::string> tokens, common::Trie<TokenType> trie)
{
    for (std::string token : tokens)
    {
        testing::checkInTrie(token, trie);
    }
}

int countInvalidTokens(std::vector<Token> vector)
{
    int counter = 0;
    for (Token tk : vector)
    {
        if (tk.type == TokenType::InvalidToken)
        {
            counter++;
        }
    }
    return counter;
}

SCENARIO("Trie initialization")
{

    GIVEN("The file name")
    {
        THEN("it's readen without errors")
        {
            for (int i = 1; i < 12; i++)
            {
                std::string fileName = fmt::format("examples/ex{}.rdd", i);
                std::vector<Token> res = lexer::read(fileName);
                REQUIRE(countInvalidTokens(res) == 0);
            }
        }
    }
    GIVEN("A line of riddle code")
    {
        std::vector<std::string> code{
            "if true loop end is near ",
            "for x in 5.3..26.789",
            "var an_int: integer",
            "if (x+y)<=z then",
            "TokenType::Colon;",
            "var ans:  integer is\t42",
            "if 0.0 /= 42.424 then   good := true\tend",
            "for i in reverse 1..42 loop",
            "type MyRecord record\tvar x : integer\tvar ok : boolean;\tend",
        };
        std::vector<std::vector<TokenType>> result{
            {TokenType::If, TokenType::True, TokenType::LoopBegin,
             TokenType::End, TokenType::Is, TokenType::Identifier, TokenType::NewLine},
            {TokenType::ForLoop, TokenType::Identifier, TokenType::InRange,
             TokenType::RealLiteral, TokenType::TwoDots, TokenType::RealLiteral, TokenType::NewLine},
            {TokenType::VarDec, TokenType::Identifier, TokenType::Colon, TokenType::IntegerType, TokenType::NewLine},
            {TokenType::If, TokenType::BracketOpen, TokenType::Identifier, TokenType::PlusOp, TokenType::Identifier,
             TokenType::BracketClose, TokenType::SeqComp, TokenType::Identifier, TokenType::Then, TokenType::NewLine},
            {TokenType::Identifier, TokenType::Colon, TokenType::Colon, TokenType::Identifier,
             TokenType::Semicolon, TokenType::NewLine},
            {TokenType::VarDec, TokenType::Identifier, TokenType::Colon, TokenType::IntegerType,
             TokenType::Is, TokenType::IntegerLiteral, TokenType::NewLine},
            {TokenType::If, TokenType::RealLiteral, TokenType::NeqComp, TokenType::RealLiteral, TokenType::Then,
             TokenType::Identifier, TokenType::Assignment, TokenType::True, TokenType::End, TokenType::NewLine},
            {TokenType::ForLoop, TokenType::Identifier, TokenType::InRange, TokenType::ReverseRange, TokenType::IntegerLiteral,
             TokenType::TwoDots, TokenType::IntegerLiteral, TokenType::LoopBegin, TokenType::NewLine},
            {TokenType::TypeDec, TokenType::Identifier, TokenType::RecordType, TokenType::VarDec, TokenType::Identifier,
             TokenType::Colon, TokenType::IntegerType, TokenType::VarDec, TokenType::Identifier, TokenType::Colon,
             TokenType::BooleanType, TokenType::Semicolon, TokenType::End, TokenType::NewLine}};
        for (size_t i = 0; i < code.size(); i++)
        {
            std::vector<Token> res = lexer::splitLine(code[i]);
            testing::checkVectorTypeEquality(result[i], res);
            lexer::freeResult();
        }
    }
    GIVEN("A command to initialize the trie")
    {
        common::Trie<TokenType> trie = lexer::getTrie();

        THEN("All the keywords are in the trie")
        {
            std::vector<std::string> keywords{"var", "type", "routine", "is", "integer",
                                              "real", "boolean", "record", "array", "true",
                                              "false", "while", "for", "loop", "end",
                                              "reverse", "in", "if", "else", "and",
                                              "or", "xor", "then", "return"};
            checkTokens(keywords, trie);
        }
        THEN("Boolean ops are in the trie")
        {
            std::vector<std::string> boolOps{">", "<", ">=", "<=", "=", "/="};
            checkTokens(boolOps, trie);
        }
        THEN("Math ops are in the tire")
        {
            std::vector<std::string> mathOps{"+", "-", "*", "/", "%"};
            checkTokens(mathOps, trie);
        }
        THEN("Special chars are in the tire")
        {
            std::vector<std::string> specChars{"+", "-", "*", "/", "%", "\n"};
            checkTokens(specChars, trie);
        }
    }
}
