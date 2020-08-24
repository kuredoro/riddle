#include "lexer.hpp"
#include "token.hpp"
#include <vector>
#include <string>
#include "trie.hpp"
#include <iostream>
#include <fstream>
using namespace std;

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
    // get file (input - string)
    string line;
    string fileName = "./README.md";
    cout << fileName;
    // Read from the text file
    ifstream InputFS(fileName);

    // while (getline(InputFS, line)) // get tokens by line, because we should save the 'coordinates'
    // {
    // split by spaces
    // for each splitted word:
    // check in tree
    // if there - add token
    // else
    // if string[0] literal - identifier (check if there are only literals and numbers or underscore)
    // else
    // if there is a dot - RealLiteral
    // else IntegerLiteral
    // add new-line token
    // Output the text from the file
    // }

    // Close the file
    InputFS.close();
    return 0;
}
