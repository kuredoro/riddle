#include <vector>
#include "fmt/format.h"
#include "catch2/catch.hpp"
#include "catch_helpers.hpp"
#include "trie.hpp"

namespace testing {


template <typename T>
void AssertTrieContains(const common::Trie<T>& trie, const common::TriePayload<T>& payload) {
    auto result = trie.Find(payload.key);

    CHECK_MESSAGE((bool)result, 
            "expected to find key \"{}\", but didn't", payload.key);

    if (result) {
        CHECK_MESSAGE(*result == payload.value,
            "got payload {}, want {}", *result, payload.value);
    }
}

template <typename T>
void AssertTrieDoesNotContain(const common::Trie<T>& trie, const std::string& key) {
    auto result = trie.Find(key);

    CHECK_MESSAGE(!(bool)result, 
            "found key \"{}\", but didn't expect to", key);
}


} // namespace testing

SCENARIO("Basic word lookup via trie") {

    // TODO: ask on catch2 discord server where the hell is HAS_FAILED() macro
    // And then add printing of the source dictionary and the trie structure
    // if test fails.
    
    std::vector<std::string> foreignWords{
        "zzzz", "世界", "لوحة", "this is not in trie at all"
    };

    GIVEN("A set of words<->ints") {
        std::vector<common::TriePayload<int>> words{
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

        THEN("Words not in the trie are reported as such") {
            for (auto& w : foreignWords) {
                testing::AssertTrieDoesNotContain(trie, w);
            }
        }
    }
    
    GIVEN("A set of words<->strings") {
        std::vector<common::TriePayload<std::string>> words{
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

        THEN("Words not in the trie are reported as such") {
            for (auto& w : foreignWords) {
                testing::AssertTrieDoesNotContain(trie, w);
            }
        }
    }
}

SCENARIO("Initalizer-list initialization of a trie") {

    GIVEN("A set of words with integer payload") {

        common::Trie<int> trie{
            {"abc", 5},
            {"bcd", 6},
            {"xyz", 0},
            {"acb", -5},
            {"xxx", 18},
            {"ans", 42},
        };

        std::vector<common::TriePayload<int>> words{
            {"abc", 5},
            {"bcd", 6},
            {"xyz", 0},
            {"acb", -5},
            {"xxx", 18},
            {"ans", 42},
        };

        THEN("These words exist and report correct payloads") {
            for (auto& pl : words) {
                testing::AssertTrieContains(trie, pl);
            }
        }
    }
}
