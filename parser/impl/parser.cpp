#include "parser.hpp"
#include "token.hpp"

namespace AST
{


    // ---- @kureduro



    // ---- @CrazyDream1

    VariableNode Parser::parseVariable() {
        // TODO: change to  .Peek() wherever checking for error ("expecting")
        auto token = m_lexer.Next();
        if (token.type != lexer::TokenType::Var) {
            m_errors.push_back({
                .position = token.position,
                .message = "Expected \"var\" keyword but didn't find it.",
            });
        }
        token = m_lexer.Next();
        if (token.type != lexer::TokenType::Identifier) {
            m_errors.push_back({
                .position = token.position,
                .message = "Expected an identifier after \"var\" keyword.",
            });
        }
        // ...
    }

    // ---- @MefAldemisov

    WhileLoopNode Parser::parseWhileLoop() {
        // TODO: change to  .Peek() wherever checking for error ("expecting")
        auto token = m_lexer.Next();
        if (token.type != lexer::TokenType::While) {
            m_errors.push_back({
                .position = token.position,
                .message = "Expected \"while\" keyword but didn't find it.",
            });
        }
        // ...
    }

    // ---- @aabounegm

    ExpressionNode Parser::parseExpression() {
        // TODO: Concrete implementation
    }

    // ---- End separation


} // namespace AST
