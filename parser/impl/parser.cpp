#include "parser.hpp"
#include "token.hpp"

namespace AST
{


    // ---- @kureduro

    bool Node::operator==(const Node& other) const {
		return begin == other.begin && end == other.end;
	}

    bool ProgramNode::operator==(const ProgramNode& other) const {
		return Node::operator==(other) && routine_table == other.routine_table;
	}

    ProgramNode Parser::parseProgram() {
        return ProgramNode();
    }

    // ---- @CrazyDream1

    VariableNode Parser::parseVariable() {
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
