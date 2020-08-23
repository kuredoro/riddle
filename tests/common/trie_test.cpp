#include <vector>
#include "fmt/format.h"
#include "catch2/catch.hpp"
#include "trie.hpp"

#define FMT_INFO(...) INFO(fmt::format(__VA_ARGS__))
#define FMT_UINFO(...) UNSCOPED_INFO(fmt::format(__VA_ARGS__))
#define CHECK_MESSAGE(cond, ...) do { FMT_INFO(__VA_ARGS__); CHECK(cond); } while((void)0, 0)
#define REQUIRE_MESSAGE(cond, ...) do { FMT_INFO(__VA_ARGS__); REQUIRE(cond); } while((void)0, 0)

namespace testing {


template <typename T>
struct TriePayload {
    std::string key;
    T value;
};

template <typename T>
void AssertTrieContains(const common::Trie<T>& trie, testing::TriePayload<T> payload) {
    auto result = trie.Find(payload.key);

    CHECK_MESSAGE((bool)result, 
            "expected to find key \"{}\", but didn't", payload.key);

    if (result) {
        CHECK_MESSAGE(*result == payload.value,
            "got payload {}, want {}", *result, payload.value);
    }
}


} // namespace testing

SCENARIO("Basic word lookup via trie") {

    // TODO: ask on catch2 discord server where the hell is HAS_FAILED() macro
    // And then add printing of the source dictionary and the trie structure
    // if test fails.

    GIVEN("A set of words<->ints") {
        std::vector<testing::TriePayload<int>> words{
            {"a", 1},
            {"b", 2},
            {"abc", 3},
            {"bcd", 4},
        };

        common::Trie<int> trie;
        for (auto& i : words) {
            trie.Add(i.key, i.value);
        }


        THEN("All words that belong to it are reported as found") {
            for (auto& i : words) {
                testing::AssertTrieContains(trie, i);
            }
        }
    }
    
    GIVEN("A set of words<->strings") {
        std::vector<testing::TriePayload<std::string>> words{
            {"first", "nyan"},
            {"fir", "cat"},
            {"secon", "tsukue"},
            {"ssecon", "kinou"},
            {"second", "ashita"},
            {"fff", "kyou"},
            {"seldom", "ima"},
        };

        common::Trie<std::string> trie;
        for (auto& i : words) {
            trie.Add(i.key, i.value);
        }

        THEN("All words that belong to it are reported as found") {
            for (auto& i : words) {
                testing::AssertTrieContains(trie, i);
            }
        }
    }
}

