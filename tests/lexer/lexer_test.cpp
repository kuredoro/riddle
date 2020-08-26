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

        for (int i = 0; i < int(expected.size()); i++)
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
                std::vector<Token> res = read(fileName);
                REQUIRE(countInvalidTokens(res) == 0);
            }
        }
    }
    GIVEN("A line of riddle code")
    {
        std::vector<std::string> code{
            "if true loop end is near ",
            "for x in 5.3..26.789",
        };
        std::vector<std::vector<TokenType>> result{
            {TokenType::If, TokenType::True, TokenType::LoopBegin,
             TokenType::End, TokenType::Is, TokenType::Identifier, TokenType::NewLine},
            {TokenType::ForLoop, TokenType::Identifier, TokenType::InRange,
             TokenType::RealLiteral, TokenType::TwoDots, TokenType::RealLiteral, TokenType::NewLine}};
        for (int i = 0; i < int(code.size()); i++)
        {
            std::vector<Token> res = splitLine(code[i]);
            testing::checkVectorTypeEquality(result[i], res);
            freeResult();
        }
    }
    GIVEN("A command to initialize the trie")
    {
        common::Trie<TokenType> trie = getTrie();

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
