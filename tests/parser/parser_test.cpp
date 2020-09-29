#include "catch2/catch.hpp"
#include "catch_helpers.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include <iterator>

/**
 * Since only the parser should be tested here, a dummy (stub) lexer
 *  is used to not be affected by bugs in the actual parser
 */
class DummyLexer : public lexer::Lexer {

public:
    DummyLexer(std::initializer_list<lexer::Token> tokens) : lexer::Lexer("") {
        for (auto it = tokens.begin(); it != tokens.end(); it++) {
            m_tokens.push_back(*it);
        }
        it = m_tokens.begin();
    }

    lexer::Token Next() {
        if (it == m_tokens.end()) {
            return {.type = lexer::TokenType::Eof};
        }
        lexer::Token t = *it;
        it++;
        return t;
    }

    lexer::Token operator[](size_t idx) { return m_tokens.at(idx); }

private:
    std::vector<lexer::Token> m_tokens;
    std::vector<lexer::Token>::iterator it;
};

SCENARIO("Parser builds a tree from tokens") {
    GIVEN("A lexer that gets tokens") {
        WHEN("Tokens represent the expression '2+6/3'") {
            DummyLexer lx{
                lexer::Token{
                    .type = lexer::TokenType::Int,
                    .pos =
                        {
                            .line = 1,
                            .column = 1,
                        },
                    .lit = "2",
                },
                lexer::Token{
                    .type = lexer::TokenType::Add,
                    .pos =
                        {
                            .line = 1,
                            .column = 2,
                        },
                    .lit = "+",
                },
                lexer::Token{
                    .type = lexer::TokenType::Int,
                    .pos =
                        {
                            .line = 1,
                            .column = 3,
                        },
                    .lit = "6",
                },
                lexer::Token{
                    .type = lexer::TokenType::Div,
                    .pos =
                        {
                            .line = 1,
                            .column = 4,
                        },
                    .lit = "/",
                },
                lexer::Token{
                    .type = lexer::TokenType::Int,
                    .pos =
                        {
                            .line = 1,
                            .column = 5,
                        },
                    .lit = "3",
                },
            };

            THEN("Order of operations is preserved") {
                // parser::Parser parser(lx);
                // auto tree = parser.parseExpression();
                // TODO: compare the tree to the expected one
            }
        }

        WHEN("Tokens represent a routine") {
            DummyLexer lx{
                lexer::Token{
                    .type = lexer::TokenType::Routine,
                    .pos =
                        {
                            .line = 1,
                            .column = 1,
                        },
                    .lit = "routine",
                },
                lexer::Token{
                    .type = lexer::TokenType::Identifier,
                    .pos =
                        {
                            .line = 1,
                            .column = 9,
                        },
                    .lit = "main",
                },
                // TODO
            };

            THEN("It is parsed correctly") {}
        }

        WHEN("The routine is not named") {
            // TODO
            THEN("An error is reported") {}
        }

        WHEN("Tokens represent a parameter \"x: array integer\"") {
            DummyLexer lx{
                lexer::Token{
                    .type = lexer::TokenType::Identifier,
                    .pos =
                        {
                            .line = 1,
                            .column = 1,
                        },
                    .lit = "x",
                },
                lexer::Token{
                    .type = lexer::TokenType::Colon,
                    .pos =
                        {
                            .line = 1,
                            .column = 2,
                        },
                    .lit = ":",
                },
                lexer::Token{
                    .type = lexer::TokenType::Array,
                    .pos =
                        {
                            .line = 1,
                            .column = 4,
                        },
                    .lit = "array",
                },
                lexer::Token{
                    .type = lexer::TokenType::IntegerType,
                    .pos =
                        {
                            .line = 1,
                            .column = 10,
                        },
                    .lit = "integer",
                },
            };

            ast::Parameter expected;
            expected.name = lx[0].lit;
            ast::ArrayType expectedType;
            ast::IntegerType integer;
            expectedType.elementType =
                std::make_shared<ast::IntegerType>(integer);
            expected.type = std::make_shared<ast::ArrayType>(expectedType);

            THEN("It is parsed correctly") {
                // parser::Parser parser(lx);
                // auto tree = parser.parseParameter();
                // CHECK((bool)tree);
                // CHECK(*tree == expected);
            }
        }

        WHEN("Tokens represent an array type \"array [ 5+3 ] real\"") {
            DummyLexer lx{
                lexer::Token{
                    .type = lexer::TokenType::Array,
                    .pos =
                        {
                            .line = 1,
                            .column = 1,
                        },
                    .lit = "array",
                },
                lexer::Token{
                    .type = lexer::TokenType::OpenBrack,
                    .pos =
                        {
                            .line = 1,
                            .column = 7,
                        },
                    .lit = "[",
                },
                lexer::Token{
                    .type = lexer::TokenType::Int,
                    .pos =
                        {
                            .line = 1,
                            .column = 9,
                        },
                    .lit = "5",
                },
                lexer::Token{
                    .type = lexer::TokenType::Add,
                    .pos =
                        {
                            .line = 1,
                            .column = 10,
                        },
                    .lit = "+",
                },
                lexer::Token{
                    .type = lexer::TokenType::Int,
                    .pos =
                        {
                            .line = 1,
                            .column = 11,
                        },
                    .lit = "3",
                },
                lexer::Token{
                    .type = lexer::TokenType::CloseBrack,
                    .pos =
                        {
                            .line = 1,
                            .column = 13,
                        },
                    .lit = "]",
                },
                lexer::Token{
                    .type = lexer::TokenType::RealType,
                    .pos =
                        {
                            .line = 1,
                            .column = 15,
                        },
                    .lit = "real",
                },
            };

            ast::ArrayType expected;

            THEN("It is parsed correctly") {
                // parser::Parser parser(lx);
                // auto tree = parser.parseParameter();
                // CHECK((bool)tree);
                // CHECK(*tree == expected);
            }
        }

        WHEN("Tokens represent a for loop with reverse") {
            DummyLexer lx{
                lexer::Token{
                    .type = lexer::TokenType::For,
                    .pos =
                        {
                            .line = 1,
                            .column = 1,
                        },
                    .lit = "for",
                },
                lexer::Token{
                    .type = lexer::TokenType::NewLine,
                    .pos =
                        {
                            .line = 1,
                            .column = 5,
                        },
                    .lit = "\n",
                },
                lexer::Token{
                    .type = lexer::TokenType::Identifier,
                    .pos =
                        {
                            .line = 2,
                            .column = 1,
                        },
                    .lit = "i",
                },
                lexer::Token{
                    .type = lexer::TokenType::NewLine,
                    .pos =
                        {
                            .line = 2,
                            .column = 4,
                        },
                    .lit = "\n",
                },
                lexer::Token{
                    .type = lexer::TokenType::In,
                    .pos =
                        {
                            .line = 3,
                            .column = 1,
                        },
                    .lit = "in",
                },
                lexer::Token{
                    .type = lexer::TokenType::Reverse,
                    .pos =
                        {
                            .line = 4,
                            .column = 4,
                        },
                    .lit = "reverse",
                },
                lexer::Token{
                    .type = lexer::TokenType::Int,
                    .pos =
                        {
                            .line = 4,
                            .column = 4,
                        },
                    .lit = "5",
                },
                lexer::Token{
                    .type = lexer::TokenType::TwoDots,
                    .pos =
                        {
                            .line = 4,
                            .column = 5,
                        },
                    .lit = "..",
                },
                lexer::Token{
                    .type = lexer::TokenType::Int,
                    .pos =
                        {
                            .line = 4,
                            .column = 7,
                        },
                    .lit = "10",
                },
                lexer::Token{
                    .type = lexer::TokenType::NewLine,
                    .pos =
                        {
                            .line = 4,
                            .column = 9,
                        },
                    .lit = "\n",
                },
                lexer::Token{
                    .type = lexer::TokenType::Loop,
                    .pos =
                        {
                            .line = 5,
                            .column = 1,
                        },
                    .lit = "loop",
                },
                lexer::Token{
                    .type = lexer::TokenType::NewLine,
                    .pos =
                        {
                            .line = 6,
                            .column = 1,
                        },
                    .lit = "\n",
                },
                lexer::Token{
                    .type = lexer::TokenType::End,
                    .pos =
                        {
                            .line = 6,
                            .column = 1,
                        },
                    .lit = "end",
                },
            };

            THEN("It is parsed correctly") {}
        }

        WHEN("Tokens represent a while loop") {
            DummyLexer lx{
                lexer::Token{
                    .type = lexer::TokenType::While,
                    .pos =
                        {
                            .line = 1,
                            .column = 1,
                        },
                    .lit = "while",
                },
                lexer::Token{
                    .type = lexer::TokenType::NewLine,
                    .pos =
                        {
                            .line = 1,
                            .column = 7,
                        },
                    .lit = "\n",
                },
                lexer::Token{
                    .type = lexer::TokenType::Boolean,
                    .pos =
                        {
                            .line = 2,
                            .column = 1,
                        },
                    .lit = "true",
                },
                lexer::Token{
                    .type = lexer::TokenType::Loop,
                    .pos =
                        {
                            .line = 2,
                            .column = 6,
                        },
                    .lit = "loop",
                },
                lexer::Token{
                    .type = lexer::TokenType::NewLine,
                    .pos =
                        {
                            .line = 2,
                            .column = 11,
                        },
                    .lit = "\n",
                },
                lexer::Token{
                    .type = lexer::TokenType::End,
                    .pos =
                        {
                            .line = 3,
                            .column = 1,
                        },
                    .lit = "end",
                },
            };

            THEN("It is parsed correctly") {}
        }
        WHEN("Tokens represent an if-else statement") {
            DummyLexer lx{
                lexer::Token{
                    .type = lexer::TokenType::If,
                    .pos =
                        {
                            .line = 1,
                            .column = 1,
                        },
                    .lit = "if",
                },
                lexer::Token{
                    .type = lexer::TokenType::Boolean,
                    .pos =
                        {
                            .line = 1,
                            .column = 3,
                        },
                    .lit = "true",
                },
                lexer::Token{
                    .type = lexer::TokenType::Then,
                    .pos =
                        {
                            .line = 1,
                            .column = 8,
                        },
                    .lit = "then",
                },
                lexer::Token{
                    .type = lexer::TokenType::NewLine,
                    .pos =
                        {
                            .line = 1,
                            .column = 13,
                        },
                    .lit = "\n",
                },
                lexer::Token{
                    .type = lexer::TokenType::Else,
                    .pos =
                        {
                            .line = 2,
                            .column = 1,
                        },
                    .lit = "else",
                },
                lexer::Token{
                    .type = lexer::TokenType::End,
                    .pos =
                        {
                            .line = 2,
                            .column = 6,
                        },
                    .lit = "end",
                },
            };

            THEN("It is parsed correctly") {}
        }
        WHEN("Tokens represent an if seatement without else") {
            DummyLexer lx{
                lexer::Token{
                    .type = lexer::TokenType::If,
                    .pos =
                        {
                            .line = 1,
                            .column = 1,
                        },
                    .lit = "if",
                },
                lexer::Token{
                    .type = lexer::TokenType::Boolean,
                    .pos =
                        {
                            .line = 1,
                            .column = 3,
                        },
                    .lit = "true",
                },
                lexer::Token{
                    .type = lexer::TokenType::Then,
                    .pos =
                        {
                            .line = 1,
                            .column = 8,
                        },
                    .lit = "then",
                },
                lexer::Token{
                    .type = lexer::TokenType::NewLine,
                    .pos =
                        {
                            .line = 1,
                            .column = 13,
                        },
                    .lit = "\n",
                },
                lexer::Token{
                    .type = lexer::TokenType::NewLine,
                    .pos =
                        {
                            .line = 2,
                            .column = 1,
                        },
                    .lit = "\n",
                },
                lexer::Token{
                    .type = lexer::TokenType::End,
                    .pos =
                        {
                            .line = 3,
                            .column = 1,
                        },
                    .lit = "end",
                },
            };

            THEN("It is parsed correctly") {}
        }
    }
}
