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
    trie.Add(";", TokenType::Semicolumn);
    trie.Add(":=", TokenType::Assignment);
    trie.Add(";", TokenType::Column);
    return trie;
}

Token makeToken(TokenType tt, int ln, std::string str)
{
    Token res;
    res.type = tt;
    res.srcPos = ln;
    res.image = str;
    return res;
}
std::string toStr(char x)
{
    // string class has a constructor
    // that allows us to specify size of
    // string as first parameter and character
    // to be filled in given size as second
    // parameter.
    std::string s(1, x);

    return s;
}

int read()
{
    std::vector<Token> result;
    // get file (input - string)
    std::string line, word;
    std::string fileName = "./examples/ex2.rdd";
    // Read from the text file
    std::ifstream InputFS(fileName);
    common::Trie<TokenType> trie = initTrie();
    int lineNum = 0;
    while (std::getline(InputFS, line))
    {
        lineNum++;
        std::istringstream iss(line);
        while (std::getline(iss, word, ' '))
        {
            if (result.size() > 0 && result[result.size() - 1].type == TokenType::SingleLineComment)
            {
                continue;
            }
            auto res = trie.Find(word);
            if (res)
            {
                // cout << res.value_or(-1) << "\n";
                TokenType invalidToken = TokenType::InvalidToken;
                TokenType val = res.value_or(invalidToken);
                result.push_back(makeToken(val, lineNum, word));
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
                // read char-by-char:
                int index = 0;
                while (index < int(word.length()))
                {
                    char c = word[index++];
                    switch (c)
                    {
                    // math ops
                    case '+':
                        result.push_back(makeToken(TokenType::PlusOp, lineNum, toStr(c)));
                        break;
                    case '-':
                        result.push_back(makeToken(TokenType::MinusOp, lineNum, toStr(c)));
                        break;
                    case '%':
                        result.push_back(makeToken(TokenType::RemainderOp, lineNum, toStr(c)));
                        break;
                    case '*':
                        result.push_back(makeToken(TokenType::MultOp, lineNum, toStr(c)));
                        break;
                    // braces & parenthesis
                    case '(':
                        result.push_back(makeToken(TokenType::BracketOpen, lineNum, toStr(c)));
                        break;
                    case ')':
                        result.push_back(makeToken(TokenType::BracketClose, lineNum, toStr(c)));
                        break;
                    case '[':
                        result.push_back(makeToken(TokenType::SquareBracketOpen, lineNum, toStr(c)));
                        break;
                    case ']':
                        result.push_back(makeToken(TokenType::SquareBracketClose, lineNum, toStr(c)));
                        break;
                    case '=':
                        result.push_back(makeToken(TokenType::EqComp, lineNum, toStr(c)));
                        break;
                    case ';':
                        result.push_back(makeToken(TokenType::Semicolumn, lineNum, toStr(c)));
                        break;

                    // dots
                    case '.':
                        if (index < int(word.length()) && word[index] == '.')
                        {
                            index++;
                            result.push_back(makeToken(TokenType::TwoDots, lineNum, ".."));
                        }
                        else
                        {
                            result.push_back(makeToken(TokenType::Dot, lineNum, toStr(c)));
                        }
                        break;
                    // comparisons
                    case '<':
                        if (index < int(word.length()) && word[index] == '=')
                        {
                            index++;
                            result.push_back(makeToken(TokenType::SeqComp, lineNum, "<="));
                        }
                        else
                        {
                            result.push_back(makeToken(TokenType::SmallerComp, lineNum, toStr(c)));
                        }
                        break;
                    case '>':
                        if (index < int(word.length()) && word[index] == '=')
                        {
                            index++;
                            result.push_back(makeToken(TokenType::BeqComp, lineNum, ">="));
                        }
                        else
                        {
                            result.push_back(makeToken(TokenType::BiggerComp, lineNum, toStr(c)));
                        }
                        break;
                    case '/':
                        if (index < int(word.length()) && word[index] == '=')
                        {
                            index++;
                            result.push_back(makeToken(TokenType::NeqComp, lineNum, "/="));
                        }
                        else
                        {
                            result.push_back(makeToken(TokenType::DevOp, lineNum, toStr(c)));
                        }
                        break;
                    case ':':
                        if (index < int(word.length()) && word[index] == '=')
                        {
                            index++;
                            result.push_back(makeToken(TokenType::Assignment, lineNum, ":="));
                        }
                        else
                        {
                            result.push_back(makeToken(TokenType::Column, lineNum, toStr(c)));
                        }
                        break;

                    default:

                        if (isdigit(c))
                        {
                            // if int
                            std::string num = toStr(c);
                            while (index < int(word.length()) && isdigit(word[index]))
                            {
                                char c = word[index++];
                                num.append(toStr(c));
                            }
                            // if float
                            if (index < int(word.length()) && word[index] == '.')
                            {
                                // add dot
                                char c = word[index++];
                                num.append(toStr(c));
                                // get float part
                                while (index < int(word.length()) && isdigit(word[index]))
                                {
                                    char c = word[index++];
                                    num.append(toStr(c));
                                }
                                // save
                                result.push_back(makeToken(TokenType::RealLiteral, lineNum, num));
                            }
                            else
                            {
                                // save as int
                                result.push_back(makeToken(TokenType::IntegerLiteral, lineNum, num));
                            }
                        }
                        else if (iswalpha(c))
                        {
                            // get var name
                            std::string name = toStr(c);
                            while (index < int(word.length()) && (isdigit(word[index]) || isdigit(word[index]) || word[index] == '_'))
                            {
                                char c = word[index++];
                                name.append(toStr(c));
                            }
                            result.push_back(makeToken(TokenType::Identifier, lineNum, name));
                        }
                        else
                        {
                            std::cout << c << "\n";
                            result.push_back(makeToken(TokenType::InvalidToken, lineNum, toStr(c)));
                        }

                        break;
                    }
                }
            }
        }
        result.push_back(makeToken(TokenType::NewLine, lineNum, "\n"));
    }
    // Close the file
    InputFS.close();
    return 0;
}
