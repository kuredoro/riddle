#include "catch2/catch.hpp"
#include "lexer.hpp"
#include "token.hpp"
#include <string>
#include "fmt/format.h"
#include "trie.hpp"
#include "catch_helpers.hpp"
#include <vector>

namespace testing
{

    void checkInTrie(std::string key, common::Trie<TokenType>
                                          trie)
    {
        auto result = trie.Find(key);
        CHECK_MESSAGE((bool)result,
                      "expected to find key \"{}\", but didn't", key);
    }

} // namespace testing

void checkTokens(std::vector<std::string> tokens, common::Trie<TokenType> trie)
{
    for (std::string token : tokens)
    {
        testing::checkInTrie(token, trie);
    }
}

int coutInvalidTokens(std::vector<Token> vector)
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
        THEN("it s readen")
        {
            std::vector<Token> res = read("examples/ex1.rdd");
            REQUIRE(coutInvalidTokens(res) == 0);
        }
    }
    GIVEN("A command to initialize the trie")
    {
        common::Trie<TokenType> trie = initTrie();

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
