#include "catch2/catch.hpp"
#include "lexer.hpp"
#include <string>
#include "fmt/format.h"
#include "trie.hpp"
#include "catch_helpers.hpp"
#include <vector>

using namespace std;
namespace testing
{

    void checkInTrie(string key, common::Trie<int>
                                     trie)
    {
        auto result = trie.Find(key);
        CHECK_MESSAGE((bool)result,
                      "expected to find key \"{}\", but didn't", key);
    }

} // namespace testing

void checkTokens(vector<string> tokens, common::Trie<int> trie)
{
    for (string token : tokens)
    {
        testing::checkInTrie(token, trie);
    }
}

SCENARIO("Trie initialization")
{

    GIVEN("The file name")
    {
        THEN("it s readen")
        {
            REQUIRE(read() == 0);
        }
    }
    GIVEN("A command to initialize the trie")
    {
        common::Trie<int> trie = initTrie();
        THEN("All 24 keywords ar in the trie")
        {
            vector<string> keywords{"var", "type", "routine", "is", "integer",
                                    "real", "boolean", "record", "array", "true",
                                    "false", "while", "for", "loop", "end",
                                    "reverse", "in", "if", "else", "and",
                                    "or", "xor", "then", "return"};

            checkTokens(keywords, trie);
        }
        THEN("Boolean ops are in the trie")
        {
            vector<string> boolOps{">", "<", ">=", "<=", "=", "/="};
            checkTokens(boolOps, trie);
        }
        THEN("Math ops are in the tire")
        {
            vector<string> mathOps{"+", "-", "*", "/", "%"};
            checkTokens(mathOps, trie);
        }
        THEN("Special chars are in the tire")
        {
            vector<string> specChars{"+", "-", "*", "/", "%", "\n"};
            checkTokens(specChars, trie);
        }
    }
}
