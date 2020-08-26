#include "lexer.hpp"
#include "token.hpp"
#include <vector>
#include <string>
#include "trie.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cctype>

/**
 * Paks values to the Token struct instance
 *  @param type - TokenType of the struct
 *  @param srcPos - int, position of the token (line number in source code)
 *  @param image - string, source string of token
 *  @return structure with all the fields
*/

/**
 * Parsing of a single char to String
 * 
 * @param c - char, which should be 
 * @return std::string with value, content equal to c
*/
std::string toStr(char c)
{
    std::string s(1, c);
    return s;
}
namespace lexer
{
    common::Trie<TokenType>
        trie;                  // trie to search for the keywords of the language
    std::vector<Token> result; // list of tokens to be returned
    int lineNum = 1;           // line counter, start with 1, because it is file-lines enumeration

    /**
     * Iniialization of trie with a list of tokens, eqiavalent
     * to TokenType enumeration (see token.hpp)
    */
    void initTrie()
    {
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
        trie.Add("/", TokenType::DivOp);
        trie.Add("%", TokenType::RemainderOp);
        trie.Add("+", TokenType::PlusOp);
        trie.Add("-", TokenType::MinusOp);
        // signs
        trie.Add(".", TokenType::Dot);
        trie.Add("..", TokenType::TwoDots);
        trie.Add(",", TokenType::Comma);
        // braces and parenthesis
        trie.Add("(", TokenType::BracketOpen);
        trie.Add(")", TokenType::BracketClose);
        trie.Add("[", TokenType::SquareBracketOpen);
        trie.Add("]", TokenType::SquareBracketClose);
        trie.Add("//", TokenType::SingleLineComment);
        trie.Add("\n", TokenType::NewLine);
        trie.Add(";", TokenType::Semicolon);
        trie.Add(":=", TokenType::Assignment);
        trie.Add(":", TokenType::Colon);
    }

    /**
     * Checks of the longToken is in the word from position *indexPoiter
     * UPDATES the index, in case of full equality
     * ADDS required tokens to the result vector
     * 
     * @param indexPointer - pointer to int index in the word
     * @param word - the word, we are searching at
     * @param longToken - string of length 2, e.g. ">=", "<="...
     * @param tt1 - TokenType if only the first chars are equal
     * @param tt2 - TokenType if all chars are equal
     * 
    */
    void checkTwoCharToken(size_t &index, std::string word, std::string longToken,
                           TokenType tt1, TokenType tt2)
    {
        char firstChar = longToken[0];
        if (index < word.length() && word[index] == longToken[1])
        {
            index++;
            result.push_back({tt2, lineNum, longToken});
        }
        else
        {
            result.push_back({tt1, lineNum, toStr(firstChar)});
        }
    } // namespace lexer

    /**
     * Iterates through word while it is a number ()
     * 
    */
    std::string readWhileNum(size_t &index, std::string word)
    {
        std::string num;
        while (index < word.length() && std::isdigit(word[index]))
        {
            char c = word[index++];
            num.push_back(c);
        }
        return num;
    }

    int processWord(std::string word)
    {
        auto res = trie.Find(word);
        if (res)
        {
            // if value is defenetly a keyword
            TokenType val = res.value_or(TokenType::InvalidToken);
            // don't read single line comments till the end
            if (val == TokenType::SingleLineComment)
            {
                return -1;
            }
            result.push_back({val, lineNum, word});
        }
        else
        {
            // check not tab sequence
            if (int(word[0]) == 0 || isspace(word[0]))
            {
                return 0; // take next word
            }
            // read char-by-char:
            size_t index = 0;
            std::string sign;
            TokenType val;
            while (index < word.length())
            {
                char c = word[index++];
                switch (c)
                {
                // single-char tokens
                case '+':
                case '-':
                case '%':
                case '*':
                case '(':
                case ')':
                case '[':
                case ']':
                case '=':
                case ';':
                case ',':
                    sign = toStr(c);
                    val = trie.Find(sign).value_or(TokenType::InvalidToken);
                    result.push_back({val, lineNum, sign});
                    break;
                case '.':
                    checkTwoCharToken(index, word, "..", TokenType::Dot, TokenType::TwoDots);
                    break;
                case '<':
                    checkTwoCharToken(index, word, "<=", TokenType::SmallerComp, TokenType::SeqComp);
                    break;
                case '>':
                    checkTwoCharToken(index, word, ">=", TokenType::BiggerComp, TokenType::BeqComp);
                    break;
                case '/':
                    checkTwoCharToken(index, word, "/=", TokenType::DivOp, TokenType::NeqComp);
                    break;
                case ':':
                    checkTwoCharToken(index, word, ":=", TokenType::Colon, TokenType::Assignment);
                    break;

                default:

                    if (std::isdigit(c))
                    { // if number constant
                        index--;
                        std::string num = readWhileNum(index, word);
                        TokenType type = TokenType::IntegerLiteral;
                        if (index < word.length() && word[index] == '.')
                        { // if real
                            int oldIndex = index;
                            index++;
                            std::string floatPart = readWhileNum(index, word);
                            if (floatPart.length() == 0)
                            { //eg ..
                                index = oldIndex;
                                result.push_back({type, lineNum, num});
                                break;
                            }
                            num.append(".");
                            num.append(floatPart);
                            type = TokenType::RealLiteral;
                        }
                        result.push_back({type, lineNum, num});
                    }
                    else if (std::isalpha(c))
                    { // if var name
                        std::string name = toStr(c);
                        while (index < word.length() && (std::isalpha(word[index]) || std::isdigit(word[index]) || word[index] == '_'))
                        {
                            name.push_back(word[index++]);
                        }
                        val = trie.Find(name).value_or(TokenType::InvalidToken);
                        if (val == TokenType::InvalidToken)
                        {
                            result.push_back({TokenType::Identifier, lineNum, name});
                        }
                        else
                        {
                            result.push_back({val, lineNum, sign});
                        }
                    }
                    else
                    { // non-valid construction or space
                        // std::cout << "INVALID CHAR" << c << "\n";
                        if (!std::isspace(c))
                        {
                            result.push_back({TokenType::InvalidToken, lineNum, toStr(c)});
                        }
                    }

                    break;
                }
            }
        }
        return 0;
    }

    /**
     * Processing a line of riddle code. It updates the 'result' vector of tokens
     * 
     * @param line, string with line
    */
    void processLine(std::string line)
    {
        lineNum++;
        std::istringstream iss(line);
        std::string word;
        while (std::getline(iss, word, ' '))
        {
            if (processWord(word) == -1)
            { //if get a single-line comment
                return;
            }
        }
        result.push_back({TokenType::NewLine, lineNum, "\n"});
    }
    // ==================================PUBLIC================================
    /**
 * Gets the trie from the lexer's namespace. 
 * Method should be used for testing and debugging only
 * 
 * @return trie, used during parsing (debugging purposes)
*/
    common::Trie<TokenType> getTrie()
    {
        initTrie();
        return trie;
    }

    std::vector<Token> splitLine(std::string line)
    {
        initTrie();
        processLine(line);
        return result;
    }

    void freeResult()
    {
        result = {};
    }

    /**
 * Reading of the file and it's separaion on the tokens
 * 
 * @param fileName - path to the file to be parsed
 * @return vector of Tokens, founded in the given file, if file doesn't exist, the empty
 * vector is returned
*/
    std::vector<Token> read(std::string fileName)
    {
        std::string line;
        std::ifstream InputFS(fileName);
        initTrie();
        while (std::getline(InputFS, line))
        {
            processLine(line);
        }
        std::vector<Token> res = result;
        freeResult();
        return res;
    }
} // namespace lexer