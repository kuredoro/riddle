#include "trie.hpp"
#include "token.hpp"
#pragma once

std::vector<Token> read(std::string);
common::Trie<TokenType> initTrie();