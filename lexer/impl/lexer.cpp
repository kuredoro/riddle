#include "lexer.hpp"
#include "token.hpp"
#include <vector>
#include <string>
#include "trie.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <ctype.h>

common::Trie<TokenType> initTrie()
{
    common::Trie<TokenType> trie;
    // keywords
    trie.Add("var", TokenType::VarDec);
    trie.Add("type", TokenType::TypeDec);
    trie.Add("routine", TokenType::RoutineDec);
    trie.Add("return", TokenType::Return);
    trie.Add("is", TokenType::Is);
    trie.Add("integer", TokenType::IntegerType);
    trie.Add("real", TokenType::RealType);
    trie.Add("boolean", TokenType::BooleanType);
    trie.Add("record", TokenType::RecordType);
    trie.Add("array", TokenType::ArrayType);
    trie.Add("true", TokenType::True);
    trie.Add("false", TokenType::False);
    trie.Add("while", TokenType::WhileLoop);
    trie.Add("for", TokenType::ForLoop);
    trie.Add("loop", TokenType::LoopBegin);
    trie.Add("end", TokenType::End);
    trie.Add("reverse", TokenType::ReverseRange);
    trie.Add("in", TokenType::InRange);
    trie.Add("if", TokenType::If);
    trie.Add("then", TokenType::Then);
    trie.Add("else", TokenType::Else);
    trie.Add("and", TokenType::AndLogic);
    trie.Add("or", TokenType::OrLogic);
    trie.Add("xor", TokenType::XorLogic);
    // comparison
    trie.Add("<", TokenType::SmallerComp);
    trie.Add("<=", TokenType::SeqComp);
    trie.Add(">", TokenType::BiggerComp);
    trie.Add(">=", TokenType::BeqComp);
    trie.Add("=", TokenType::EqComp);
    trie.Add("/=", TokenType::NeqComp);
    // math ops
    trie.Add("*", TokenType::MultOp);
    trie.Add("/", TokenType::DevOp);
    trie.Add("%", TokenType::RemainderOp);
    trie.Add("+", TokenType::PlusOp);
    trie.Add("-", TokenType::MinusOp);
    /* WARN     IntegerLiteral, // int const
        RealLiteral, // real const
        Identifier, // name
        and NewLine
    */
    // signs
    trie.Add(".", TokenType::Dot);
    trie.Add("..", TokenType::TwoDots);
    trie.Add(",", TokenType::Comma);
    // braces and parenthesis
    trie.Add("(", TokenType::BracketOpen);
    trie.Add(").", TokenType::BracketClose);
    trie.Add("[", TokenType::SquareBracketOpen);
    trie.Add("]", TokenType::SquareBracketClose);
    trie.Add("//", TokenType::SingleLineComment);
    trie.Add("\n", TokenType::NewLine);
    return trie;
}

int read()
{
    std::vector<TokenType> result;
    // get file (input - string)
    std::string line, word;
    std::string fileName = "./examples/ex1.rdd";
    // Read from the text file
    std::ifstream InputFS(fileName);
    common::Trie<TokenType> trie = initTrie();

    while (std::getline(InputFS, line))
    {
        std::istringstream iss(line);
        while (std::getline(iss, word, ' '))
        {
            auto res = trie.Find(word);
            if (res)
            {
                // cout << res.value_or(-1) << "\n";
                TokenType invalidToken = TokenType::InvalidToken;
                TokenType val = res.value_or(invalidToken);
                result.insert(result.end(), val);
                std::cout << int(result[result.size() - 1]) << "\n";
                if (val == TokenType::SingleLineComment)
                {
                    continue;
                }
            }
            else
            {
                // check not tab sequence
                if (int(word[0]) == 0 || isspace(word[0]))
                {
                    continue;
                }
                std::cout << word << "\n";
                // read char-by-char:
                // if first literal -> suggest it is a variable name
                // run throught all word if number/_/letter
                // else if num-> int or 1..4 or real
                // else
            }
        }
        // word = strtok(line, " "); an
        // cout << word << "\n";
    }

    // Close the file
    InputFS.close();
    return 0;
}
