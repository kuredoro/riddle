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

common::Trie<int> initTrie()
{
    common::Trie<int> trie;
    // keywords
    trie.Add("var", int(TokenType::VarDec));
    trie.Add("type", int(TokenType::TypeDec));
    trie.Add("routine", int(TokenType::RoutineDec));
    trie.Add("return", int(TokenType::Return));
    trie.Add("is", int(TokenType::Is));
    trie.Add("integer", int(TokenType::IntegerType));
    trie.Add("real", int(TokenType::RealType));
    trie.Add("boolean", int(TokenType::BooleanType));
    trie.Add("record", int(TokenType::RecordType));
    trie.Add("array", int(TokenType::ArrayType));
    trie.Add("true", int(TokenType::True));
    trie.Add("false", int(TokenType::False));
    trie.Add("while", int(TokenType::WhileLoop));
    trie.Add("for", int(TokenType::ForLoop));
    trie.Add("loop", int(TokenType::LoopBegin));
    trie.Add("end", int(TokenType::End));
    trie.Add("reverse", int(TokenType::ReverseRange));
    trie.Add("in", int(TokenType::InRange));
    trie.Add("if", int(TokenType::If));
    trie.Add("then", int(TokenType::Then));
    trie.Add("else", int(TokenType::Else));
    trie.Add("and", int(TokenType::AndLogic));
    trie.Add("or", int(TokenType::OrLogic));
    trie.Add("xor", int(TokenType::XorLogic));
    // comparison
    trie.Add("<", int(TokenType::SmallerComp));
    trie.Add("<=", int(TokenType::SeqComp));
    trie.Add(">", int(TokenType::BiggerComp));
    trie.Add(">=", int(TokenType::BeqComp));
    trie.Add("=", int(TokenType::EqComp));
    trie.Add("/=", int(TokenType::NeqComp));
    // math ops
    trie.Add("*", int(TokenType::MultOp));
    trie.Add("/", int(TokenType::DevOp));
    trie.Add("%", int(TokenType::RemainderOp));
    trie.Add("+", int(TokenType::PlusOp));
    trie.Add("-", int(TokenType::MinusOp));
    /* WARN     IntegerLiteral, // int const
        RealLiteral, // real const
        Identifier, // name
        and NewLine
    */
    // signs
    trie.Add(".", int(TokenType::Dot));
    trie.Add("..", int(TokenType::TwoDots));
    trie.Add(",", int(TokenType::Comma));
    // braces and parenthesis
    trie.Add("(", int(TokenType::BracketOpen));
    trie.Add(").", int(TokenType::BracketClose));
    trie.Add("[", int(TokenType::SquareBracketOpen));
    trie.Add("]", int(TokenType::SquareBracketClose));
    trie.Add("//", int(TokenType::SingleLineComment));
    trie.Add("\n", int(TokenType::SingleLineComment));
    return trie;
}

int read()
{
    std::vector<int> result;
    // get file (input - string)
    std::string line, word;
    std::string fileName = "./examples/ex1.rdd";
    // Read from the text file
    std::ifstream InputFS(fileName);
    common::Trie<int> trie = initTrie();

    while (std::getline(InputFS, line))
    {
        std::istringstream iss(line);
        while (std::getline(iss, word, ' '))
        {
            auto res = trie.Find(word);
            if (res)
            {
                // cout << res.value_or(-1) << "\n";
                int val = res.value_or(-1);
                result.insert(result.end(), val);
                std::cout << result[result.size() - 1] << "\n";
                if (val == int(TokenType::SingleLineComment))
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
