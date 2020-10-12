#include "catch2/catch.hpp"
#include "catch_helpers.hpp"
#include "fmt/format.h"
#include "trie.hpp"
#include <vector>

namespace testing {

template <typename T>
void AssertTrieContains(const common::Trie<T>& trie,
                        const common::TriePayload<T>& payload) {
    auto result = trie.Find(payload.key);

    CHECK_MESSAGE((bool)result, "expected to find key \"{}\", but didn't",
                  payload.key);

    if (result) {
        CHECK_MESSAGE(*result == payload.value,
                      "for key '{}' got payload {}, want {}", payload.key,
                      *result, payload.value);
    }

    auto resultBracket = trie[payload.key];
    if (*result != *resultBracket) {
        CHECK_MESSAGE(*result == *resultBracket,
                      "expected Find() and operator[] to return the same "
                      "value, but for key '{}' got {} and {} respectively",
                      payload.key, *result, *resultBracket);
    }
}

template <typename T>
void AssertTrieDoesNotContain(const common::Trie<T>& trie,
                              const std::string& key) {
    auto result = trie.Find(key);

    CHECK_MESSAGE(!(bool)result, "found key \"{}\", but didn't expect to", key);
}

} // namespace testing

SCENARIO("Basic word lookup via trie") {

    std::vector<std::string> foreignWords{"zzzz", "世界", "لوحة",
                                          "this is not in trie at all"};

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

        CAPTURE(words);
        CAPTURE(trie);

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
            {"first", "nyan"},   {"fir", "cat"},       {"secon", "tsukue"},
            {"ssecon", "kinou"}, {"second", "ashita"}, {"fff", "kyou"},
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

        THEN("Prefixes of the words are not reported as present") {
            testing::AssertTrieDoesNotContain(trie, "sel");
            testing::AssertTrieDoesNotContain(trie, "f");
            testing::AssertTrieDoesNotContain(trie, "ss");
        }
    }
}

SCENARIO("Initalizer-list initialization of a trie") {

    GIVEN("A set of words with integer payload") {

        common::Trie<int> trie{
            {"abc", 5},  {"bcd", 6},  {"xyz", 0},
            {"acb", -5}, {"xxx", 18}, {"ans", 42},
        };

        std::vector<common::TriePayload<int>> words{
            {"abc", 5},  {"bcd", 6},  {"xyz", 0},
            {"acb", -5}, {"xxx", 18}, {"ans", 42},
        };

        THEN("These words exist and report correct payloads") {
            for (auto& pl : words) {
                testing::AssertTrieContains(trie, pl);
            }
        }
    }
}

SCENARIO("Trie traversal via iterators") {

    GIVEN("A set of words") {

        common::Trie<int> trie{
            {"abcd", 1},
            {"abdd", 1},
            {"bcde", 1},
            {"bccc", 1},
        };

        THEN("Feeding it a word from this set char by char, yields terminal "
             "node") {
            auto head = trie.Head();

            std::string word{"abcd"};
            for (size_t ci = 0; ci < word.size(); ci++) {
                REQUIRE_MESSAGE(!head.Terminal(),
                                "before feeding i={} in word {}, got terminal "
                                "node, but want otherwise",
                                ci, word);
                REQUIRE_MESSAGE(!(bool)head.Value(),
                                "before feeding i={} in word {}, got some "
                                "value in node, expected none",
                                ci, word);
                head.Next(word[ci]);
            }

            REQUIRE_MESSAGE(head.Terminal(),
                            "got head after {} non-terminal, want otherwise",
                            word);
            REQUIRE_MESSAGE(
                (bool)head.Value(),
                "got value in head after {} empty, expected something", word);
            REQUIRE_MESSAGE(*head.Value() == 1, "got value in head {}, want {}",
                            *head.Value(), 1);
        }

        THEN("Feeding it a foreign word char by char, yields non-terminal "
             "node") {
            auto head = trie.Head();

            std::string word{"sosiska"};
            for (size_t ci = 0; ci < word.size(); ci++) {
                REQUIRE_MESSAGE(!head.Terminal(),
                                "before feeding i={} in word {}, got terminal "
                                "node, but want otherwise",
                                ci, word);
                REQUIRE_MESSAGE(!(bool)head.Value(),
                                "before feeding i={} in word {}, got some "
                                "value in node, expected none",
                                ci, word);
                head.Next(word[ci]);
            }

            REQUIRE_MESSAGE(!head.Terminal(),
                            "got head after {} terminal, want otherwise", word);
        }
    }
}
