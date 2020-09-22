#include "parser.hpp"
#include "token.hpp"

namespace parser
{

    using lexer::TokenType;
    using lexer::Token;

    // ---- @kureduro

    static std::array<TokenType, 2> primitives{
        TokenType::IntegerType,
        TokenType::RealType,
    };

    sPtr<ast::Program> Parser::parseProgram() {
        ast::Program programNode;
        Token currentToken;
        while ((currentToken = m_lexer.Peek()).type != TokenType::Eof) {
            switch(currentToken.type) {
            case TokenType::Routine:
                programNode.routines.push_back(parseRoutine());
                continue;
            case TokenType::Var:
                programNode.variables.push_back(parseVariable());
                continue;
            case TokenType::Type:
                programNode.types.push_back(parseType());
                continue;
            case TokenType::NewLine:
                m_lexer.Next();
                continue;
            default:
                m_errors.push_back(Error{
                    .pos = currentToken.pos,
                    .message = "Unexpected token",
                });
                while(m_lexer.Peek().type != TokenType::NewLine) m_lexer.Next();
            }
        }
        return std::make_shared<ast::Program>(programNode);
    }

    sPtr<ast::Routine> Parser::parseRoutine() {
        ast::Routine routineNode;
        Token currentToken = skipWhile(isNewLine);
        if (currentToken.type != TokenType::Routine) {
            m_errors.push_back(Error{
                .pos = currentToken.pos,
                .message = "Expected 'routine' keyword but did not find one",
            });
            return nullptr;
        }
        currentToken = skipWhile(isNewLine);

        if (currentToken.type != TokenType::Identifier) {
            m_errors.push_back(Error{
                .pos = currentToken.pos,
                .message = "Expected an identifier but did not find one",
            });
            // skip till "end" keyword
            while(m_lexer.Next().type != TokenType::End);
            return nullptr;
        }
        routineNode.name = currentToken;

        currentToken = skipWhile(isNewLine);

        if (currentToken.type != TokenType::OpenParen) {
            m_errors.push_back(Error{
                .pos = currentToken.pos,
                .message = "Expected to find '('",
            });
            // skip till "end" keyword
            while(m_lexer.Next().type != TokenType::End);
            return nullptr;
        }

        while(currentToken.type != TokenType::CloseParen) {
            routineNode.parameters.push_back(parseParameter());
            currentToken = skipWhile(isNewLine);
            if (currentToken.type != TokenType::Comma && currentToken.type != TokenType::CloseParen) {
                m_errors.push_back(Error{
                    .pos = currentToken.pos,
                    .message = "Unexpected token",
                });
            }
        }
        currentToken = skipWhile(isNewLine);
        if (currentToken.type != TokenType::Colon && currentToken.type != TokenType::Is) {
            m_errors.push_back(Error{
                .pos = currentToken.pos,
                .message = "Unexpected token",
            });
            // skip till "end" keyword
            while(m_lexer.Next().type != TokenType::End);
            return nullptr;
        }
        if (currentToken.type == TokenType::Colon) {
            currentToken = skipWhile(isNewLine);
            routineNode.returnType = parseType();
        }
        currentToken = skipWhile(isNewLine);

        if (currentToken.type != TokenType::Is) {
            m_errors.push_back(Error{
                .pos = currentToken.pos,
                .message = "Expected to find keyword 'is'",
            });
            // skip till "end" keyword
            while(m_lexer.Next().type != TokenType::End);
            return nullptr;
        }
        currentToken = skipWhile(isNewLine);
        routineNode.body = parseBody();
        if (currentToken.type != TokenType::End) {
            m_errors.push_back(Error{
                .pos = currentToken.pos,
                .message = "Expected to find keyword 'end'",
            });
            // skip till "end" keyword
            while(m_lexer.Next().type != TokenType::End);
            return nullptr;
        }
        return std::make_shared<ast::Routine>(routineNode);
    }

    sPtr<ast::Parameter> Parser::parseParameter() {
        ast::Parameter parameterNode;
        Token currentToken = skipWhile(isNewLine);
        if (currentToken.type != TokenType::Identifier) {
            m_errors.push_back(Error{
                .pos = currentToken.pos,
                .message = "Expected to find an identifier",
            });
            // skip till ')' or ','
            while(m_lexer.Peek().type != TokenType::Comma && m_lexer.Peek().type != TokenType::CloseParen) {
                m_lexer.Next();
            }
            return nullptr;
        }
        parameterNode.name = currentToken;
        currentToken = skipWhile(isNewLine);
        if (currentToken.type != TokenType::Colon) {
            m_errors.push_back(Error{
                .pos = currentToken.pos,
                .message = "Expected to find a ':'",
            });
            // skip till ')' or ','
            while(m_lexer.Peek().type != TokenType::Comma && m_lexer.Peek().type != TokenType::CloseParen) {
                m_lexer.Next();
            }
            return nullptr;
        }
        parameterNode.type = parseType();
        return std::make_shared<ast::Parameter>(parameterNode);
    }

    sPtr<ast::Type> Parser::parseType() {
        Token currentToken = m_lexer.Peek();
        if (TokenType *type = std::find(std::begin(primitives), std::end(primitives), currentToken.type);
            type != std::end(primitives)) {
            ast::PrimitiveType typeNode;
            typeNode.type = currentToken;
            m_lexer.Next();
            return std::make_shared<ast::PrimitiveType>(typeNode);
        } else if (currentToken.type == TokenType::Array) {
            return parseArrayType();
        } else if (currentToken.type == TokenType::Record) {
            return parseRecordType();
        } else if (currentToken.type == TokenType::Identifier) {
            ast::AliasedType typeNode;
            typeNode.name = currentToken;
            m_lexer.Next();
            return std::make_shared<ast::AliasedType>(typeNode);
        } else {
            m_errors.push_back(Error{
                .pos = currentToken.pos,
                .message = "Unknown type",
            });
            m_lexer.Next();
            return nullptr;
        }
    }

    sPtr<ast::ArrayType> Parser::parseArrayType() {
        Token currentToken = m_lexer.Next();
        if (currentToken.type != TokenType::Array) {
            m_errors.push_back(Error{
                .pos = currentToken.pos,
                .message = "Expected 'array' keyword but did not find it"
            });
            return nullptr;
        }
        ast::ArrayType arrayNode;
        if (m_lexer.Peek().type == TokenType::OpenBrack) {
            m_lexer.Next();
            arrayNode.length = parseExpression();
            currentToken = m_lexer.Next();
            if (currentToken.type != TokenType::CloseBrack) {
                m_errors.push_back(Error{
                    .pos = currentToken.pos,
                    .message = "Unexpected token"
                });
                while(m_lexer.Next().type != TokenType::CloseBrack);
                return nullptr;
            }
        }
        arrayNode.elementType = parseType();
        return std::make_shared<ast::ArrayType>(arrayNode);
    }

    sPtr<ast::RecordType> Parser::parseRecordType() {
        Token currentToken = m_lexer.Next();
        if (currentToken.type != TokenType::Record) {
            m_errors.push_back(Error{
                .pos = currentToken.pos,
                .message = "Expected 'record' keyword but did not find it"
            });
            return nullptr;
        }
        ast::RecordType recordNode;
        while (m_lexer.Peek().type == TokenType::NewLine) m_lexer.Next();
        while (m_lexer.Peek().type != TokenType::End) {
            recordNode.fields.push_back(parseVariable());
            while (m_lexer.Peek().type == TokenType::NewLine) m_lexer.Next();
        }
        m_lexer.Next();
        return std::make_shared<ast::RecordType>(recordNode);
    }

    // ---- @CrazyDream1

    sPtr<ast::Variable> Parser::parseVariable() {
	    ast::Variable variable;
        auto token = skipWhile(isNewLine);
        if (token.type != TokenType::Var) {
            m_errors.push_back(Error{
                .pos = token.pos,
                .message = "Expected \"var\" keyword but didn't find it.",
            });
        }
        token = skipWhile(isNewLine);
        if (token.type != TokenType::Identifier) {
            m_errors.push_back(Error{
                .pos = token.pos,
                .message = "Expected an identifier after \"var\" keyword.",
            });
        }
	    variable.name = token;
        token = skipWhile(isNewLine);
        if (token.type != TokenType::Is && token.type != TokenType::Colon) {
            m_errors.push_back(Error{
                .pos = token.pos,
                .message = "Expected an \"is\" keyword or : after the identifier.",
            });
        }
        switch (token.type)
        {
	        case TokenType::Colon:
                variable.type = parseType();
                if ((token = m_lexer.Peek()).type != TokenType::Is) break;
		        else m_lexer.Next();
            case TokenType::Is:
                variable.expression = parseExpression();
                break;
            default:
                m_errors.push_back(Error{
                    .pos = currentToken.pos,
                    .message = "Expected an \"is\" keyword or : after the identifier.",
                });
        }
        return std::make_shared<ast::Variable>(variable);
    }

    sPtr<ast::Body> Parser::parseBody() {
	ast::Body body;
        auto currentToken;
        while ((currentToken = m_lexer.Peek()).type != TokenType::End)
        {
            switch (currentToken.type)
            {
            case TokenType::Var:
                body.variables.push_back(parseVariable());
                continue;
            case TokenType::Type:
                body.types.push_back(parseType());
                continue;
            case TokenType::NewLine:
                m_lexer.Next();
                continue;
            default:
                body.statements.push_back(parseStatement());
            }
        }
        return std::make_shared<ast::Body>(body);
    }

    sPtr<ast::Statement> Parser::parseStatement() {
        auto currentToken = skipWhile(isNewLine);
        switch (currentToken.type)
        {
        case TokenType::Identifier:
	        sPtr<Expression> expression = parseExpression();
	        if (dynamic_cast<sPtr<ast::RoutineCall>>(expression))
	            return expression;
            else 
                return parseAssignment(expression);
        case TokenType::While:
            return parseWhileLoop();
        case TokenType::For:
            return parseForLoop();
	    case TokenType::If:
            return parseIfStatement();
        default:
            m_errors.push_back(Error{
                    .pos = currentToken.pos,
                    .message = "Unexpected token",
                });
	    return nullptr;
            //while (m_lexer.Peek().type != TokenType::NewLine)
            //    m_lexer.Next();
        }
        ;
    }

    sPtr<ast::RoutineCall> parseRoutineCall() {
        return nullptr;
    }

    sPtr<ast::Assignment> Parser::parseAssignment(sPtr<ast::Expression> left) {
        auto currentToken = skipWhile(isNewLine);
        ast::Assignment assignment;
	    if (currentToken.type != TokenType::Assign)
        {
            m_errors.push_back(Error{
                .pos = currentToken.pos,
                .message = "Expected := but didn't find it.",
            });
	    assignment.LeftExpression = left;
	    assignment.RightExpression = parseExpression();
        return std::make_shared<ast::Assignment>(assignment);
    }

    // ---- @MefAldemisov

    sPtr<ast::WhileLoop> Parser::parseWhileLoop() {
        auto token = m_lexer.Next();
        if (token.type != TokenType::While) {
            m_errors.push_back(Error{
                .pos = token.pos,
                .message = "Expected \"while\" keyword but didn't find it.",
            });
        }
        // ...
        return nullptr;
    }

    // ---- @aabounegm

    sPtr<ast::Expression> Parser::parseExpression() {
        // TODO: Concrete implementation
        return nullptr;
    }

    // ---- End separation

    bool Parser::isNewLine(Token tok) {
        return tok.type == TokenType::NewLine;
    }

    /**
     * Skips all tokens that match the given predicate, returning the first token that doesn't
     */
    Token Parser::skipWhile(std::function<bool(Token)> pred) {
        while(pred(m_lexer.Peek())) {
            m_lexer.Next();
        }
        return m_lexer.Next();
    }

    std::vector<parser::Error> Parser::getErrors() {
        return m_errors;
    }


} // namespace parser
