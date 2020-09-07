#include <iterator>
#include "catch2/catch.hpp"
#include "catch_helpers.hpp"
#include "parser.hpp"
#include "lexer.hpp"


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
            return { .type = lexer::TokenType::Eof };
        }
        lexer::Token t = *it;
        it++;
        return t;
    }

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
                    .position = {
                        .line = 1,
                        .column = 1,
                    },
                    .lit = "2",
                },
                lexer::Token{
                    .type = lexer::TokenType::Add,
                    .position = {
                        .line = 1,
                        .column = 2,
                    },
                    .lit = "+",
                },
                lexer::Token{
                    .type = lexer::TokenType::Int,
                    .position = {
                        .line = 1,
                        .column = 3,
                    },
                    .lit = "6",
                },
                lexer::Token{
                    .type = lexer::TokenType::Div,
                    .position = {
                        .line = 1,
                        .column = 4,
                    },
                    .lit = "/",
                },
                lexer::Token{
                    .type = lexer::TokenType::Int,
                    .position = {
                        .line = 1,
                        .column = 5,
                    },
                    .lit = "3",
                },
            };

            THEN("Order of operations is preserved") {
                AST::Parser parser(lx);
                auto tree = parser.parseProgram();
                // TODO: compare the tree to the expected one
            }
        }
    }
}
