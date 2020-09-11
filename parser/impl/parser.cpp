#include "parser.hpp"
#include "token.hpp"

namespace parser
{


    // ---- @kureduro


    sPtr<ast::Program> Parser::parseProgram() {
        ast::Program programNode;
        lexer::Token currentToken;
        while ((currentToken = m_lexer.Peek()).type != lexer::TokenType::Eof) {
            switch(currentToken.type) {
            case lexer::TokenType::Routine:
                programNode.routines.push_back(parseRoutine());
                continue;
            case lexer::TokenType::Var:
                programNode.variables.push_back(parseVariable());
                continue;
            case lexer::TokenType::Type:
                programNode.types.push_back(parseType());
                continue;
            case lexer::TokenType::NewLine:
                m_lexer.Next();
                continue;
            default:
                m_errors.push_back(Error{
                    .pos = currentToken.pos,
                    .message = "Unexpected token",
                });
                while(m_lexer.Peek().type != lexer::TokenType::NewLine) m_lexer.Next();
            }
        }
        return std::make_shared<ast::Program>(programNode);
    }

    sPtr<ast::Routine> Parser::parseRoutine() {
        ast::Routine routineNode;
        lexer::Token currentToken = m_lexer.Next();
        if (currentToken.type != lexer::TokenType::Routine) {
            m_errors.push_back(Error{
                .pos = currentToken.pos,
                .message = "Expected 'routine' keyword but did not find one",
            });
            return nullptr;
        }
        currentToken = skipWhile(isNewLine);

        if (currentToken.type != lexer::TokenType::Identifier) {
            m_errors.push_back(Error{
                .pos = currentToken.pos,
                .message = "Expected an identifier but did not find one",
            });
            // skip till "end" keyword
            while(m_lexer.Next().type != lexer::TokenType::End);
            return nullptr;
        }
        routineNode.name = currentToken;

        currentToken = skipWhile(isNewLine);

        if (currentToken.type != lexer::TokenType::OpenParen) {
            m_errors.push_back(Error{
                .pos = currentToken.pos,
                .message = "Expected to find '('",
            });
            // skip till "end" keyword
            while(m_lexer.Next().type != lexer::TokenType::End);
            return nullptr;
        }

        while(currentToken.type != lexer::TokenType::CloseParen) {
            routineNode.parameters.push_back(parseParameter());
            currentToken = m_lexer.Next();
            if (currentToken.type == lexer::TokenType::Comma) {
                currentToken = m_lexer.Next();
            }
        }
        currentToken = skipWhile(isNewLine);
        if (currentToken.type != lexer::TokenType::Colon && currentToken.type != lexer::TokenType::Is) {
            m_errors.push_back(Error{
                .pos = currentToken.pos,
                .message = "Unexpected token",
            });
            // skip till "end" keyword
            while(m_lexer.Next().type != lexer::TokenType::End);
            return nullptr;
        }
        if (currentToken.type == lexer::TokenType::Colon) {
            currentToken = skipWhile(isNewLine);
            routineNode.returnType = parseType();
        }
        currentToken = skipWhile(isNewLine);

        if (currentToken.type != lexer::TokenType::Is) {
            m_errors.push_back(Error{
                .pos = currentToken.pos,
                .message = "Expected to find keyword 'is'",
            });
            // skip till "end" keyword
            while(m_lexer.Next().type != lexer::TokenType::End);
            return nullptr;
        }
        currentToken = skipWhile(isNewLine);
        routineNode.body = parseBody();
        if (currentToken.type != lexer::TokenType::End) {
            m_errors.push_back(Error{
                .pos = currentToken.pos,
                .message = "Expected to find keyword 'end'",
            });
            // skip till "end" keyword
            while(m_lexer.Next().type != lexer::TokenType::End);
            return nullptr;
        }
        return std::make_shared<ast::Routine>(routineNode);
    }

    sPtr<ast::Parameter> Parser::parseParameter() {
        ast::Parameter parameterNode;
        lexer::Token currentToken = m_lexer.Next();
        if (currentToken.type != lexer::TokenType::Identifier) {
            m_errors.push_back(Error{
                .pos = currentToken.pos,
                .message = "Expected to find an identifier",
            });
            // skip till ')' or ','
            while(m_lexer.Peek().type != lexer::TokenType::Comma && m_lexer.Peek().type != lexer::TokenType::CloseParen) {
                m_lexer.Next();
            }
            return nullptr;
        }
        parameterNode.name = currentToken;
        currentToken = skipWhile(isNewLine);
        if (currentToken.type != lexer::TokenType::Colon) {
            m_errors.push_back(Error{
                .pos = currentToken.pos,
                .message = "Expected to find a ':'",
            });
            // skip till ')' or ','
            while(m_lexer.Peek().type != lexer::TokenType::Comma && m_lexer.Peek().type != lexer::TokenType::CloseParen) {
                m_lexer.Next();
            }
            return nullptr;
        }
        parameterNode.type = parseType();
        return std::make_shared<ast::Parameter>(parameterNode);
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

    bool Parser::isNewLine(lexer::Token tok) {
        return tok.type == lexer::TokenType::NewLine;
    }

    /**
     * Skips all tokens that match the given predicate, returning the first token that doesn't
     */
    lexer::Token Parser::skipWhile(std::function<bool(lexer::Token)> pred) {
        while(pred(m_lexer.Peek())) {
            m_lexer.Next();
        }
        return m_lexer.Next();
    }


} // namespace parser
