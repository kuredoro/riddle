#include "catch2/catch.hpp"
#include "lexer.hpp"
#include <string>
#include "fmt/format.h"
#include "trie.hpp"
#include "catch_helpers.hpp"

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
            string keywords[]{"var", "type", "routine", "is", "integer",
                              "real", "boolean", "record", "array", "true",
                              "false", "while", "for", "loop", "end",
                              "reverse", "in", "if", "else", "and",
                              "or", "xor", "then", "return"};
            for (string kw : keywords)
            {
                testing::checkInTrie(kw, trie);
            }
        }
        THEN("Boolean ops are in the tire")
        {
            string boolOps[]{">", "<", ">=", "<=", "=", "/="};
            for (string op : boolOps)
            {
                testing::checkInTrie(op, trie);
            }
        }
        THEN("Math ops are in the tire")
        {
            string mathOps[]{"+", "-", "*", "/", "%"};
            for (string op : mathOps)
            {
                testing::checkInTrie(op, trie);
            }
        }
        THEN("Special chars are in the tire")
        {
            string specChars[]{"+", "-", "*", "/", "%", "\n"};
            for (string spec : specChars)
            {
                testing::checkInTrie(spec, trie);
            }
        }
    }
}
