#include "parser.hpp"
#include "token.hpp"

namespace parser
{


    // ---- @kureduro


    sPtr<ast::Program> Parser::parseProgram() {
        return std::make_shared<ast::Program>();
    }

    // ---- @CrazyDream1

    sPtr<ast::Variable> Parser::parseVariable() {
        auto token = m_lexer.Next();
        if (token.type != lexer::TokenType::Var) {
            m_errors.push_back({
                .pos = token.pos,
                .message = "Expected \"var\" keyword but didn't find it.",
            });
        }
        token = m_lexer.Next();
        if (token.type != lexer::TokenType::Identifier) {
            m_errors.push_back({
                .pos = token.pos,
                .message = "Expected an identifier after \"var\" keyword.",
            });
        }
        // ...
    }

    // ---- @MefAldemisov

    sPtr<ast::WhileLoop> Parser::parseWhileLoop() {
        auto token = m_lexer.Next();
        if (token.type != lexer::TokenType::While) {
            m_errors.push_back({
                .pos = token.pos,
                .message = "Expected \"while\" keyword but didn't find it.",
            });
        }
        // ...
    }

    // ---- @aabounegm

    sPtr<ast::Expression> Parser::parseExpression() {
        // TODO: Concrete implementation
    }

    // ---- End separation


} // namespace parser
