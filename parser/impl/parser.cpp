#include "parser.hpp"
#include "token.hpp"
#include <memory>

namespace parser {

using lexer::Token;
using lexer::TokenType;

static std::array<TokenType, 3> primitives{
    TokenType::IntegerType,
    TokenType::RealType,
    TokenType::Boolean,
};

sPtr<ast::Program> Parser::parseProgram() {
    ast::Program programNode;
    Token currentToken = m_lexer.Peek();
    programNode.begin = currentToken.pos;
    while (currentToken.type != TokenType::Eof) {
        switch (currentToken.type) {
        case TokenType::Routine:
            programNode.routines.push_back(parseRoutineDecl());
            break;
        case TokenType::Var:
            programNode.variables.push_back(parseVariableDecl());
            break;
        case TokenType::Type:
            programNode.types.push_back(parseTypeDecl());
            break;
        case TokenType::NewLine:
            currentToken = m_lexer.Next();
            break;
        case TokenType::Eof:
            break;
        default:
            m_errors.push_back(Error{
                .pos = currentToken.pos,
                .message = "Unexpected token.",
            });
            while (m_lexer.Peek().type != TokenType::NewLine &&
                   m_lexer.Peek().type != TokenType::Eof)
                m_lexer.Next();
        }
        if (currentToken.type != TokenType::Eof) {
            currentToken = m_lexer.Peek();
        }
    }
    programNode.end = currentToken.pos;
    return std::make_shared<ast::Program>(programNode);
}

sPtr<ast::RoutineDecl> Parser::parseRoutineDecl() {
    ast::RoutineDecl routineNode;
    Token currentToken = expect(
        TokenType::Routine, "Expected 'routine' keyword but did not find one.");
    if (currentToken.type == TokenType::Illegal) {
        return nullptr;
    }
    routineNode.begin = currentToken.pos;
    currentToken = expect(TokenType::Identifier,
                          "Expected an identifier but did not find one.");
    if (currentToken.type == TokenType::Illegal) {
        advance(TokenType::End);
        return nullptr;
    }
    routineNode.name = currentToken.lit;

    if (expect(TokenType::OpenParen, "Expected to find '('.").type ==
        TokenType::Illegal) {
        advance(TokenType::End);
        return nullptr;
    }

    skipWhitespace();

    // handle the case of no parameters
    currentToken = m_lexer.Peek();
    if (currentToken.type == TokenType::CloseParen) {
        m_lexer.Next();
    }
    while (currentToken.type != TokenType::CloseParen) {
        routineNode.parameters.push_back(parseParameter());
        currentToken = expect({TokenType::Comma, TokenType::CloseParen});

        if (currentToken.type == TokenType::Illegal) {
            advance({TokenType::CloseParen, TokenType::End});
            return nullptr;
        }
    }

    currentToken = expect({TokenType::Colon, TokenType::Is});
    if (currentToken.type == TokenType::Illegal) {
        advance(TokenType::End);
        return nullptr;
    }

    if (currentToken.type == TokenType::Colon) {
        skipWhitespace();
        routineNode.returnType = parseType();
        currentToken =
            expect(TokenType::Is, "Expected to find \"is\" keyword.");
    }

    if (currentToken.type == TokenType::Illegal) {
        advance(TokenType::End);
        return nullptr;
    }

    skipWhitespace();

    routineNode.body = parseBody();
    currentToken = expect(TokenType::End, "Expected \"end\" keyword.");
    if (currentToken.type == TokenType::Illegal) {
        advance(TokenType::End);
        return nullptr;
    }
    routineNode.end = currentToken.pos;
    return std::make_shared<ast::RoutineDecl>(routineNode);
}

sPtr<ast::Parameter> Parser::parseParameter() {
    ast::Parameter parameterNode;
    Token currentToken =
        expect(TokenType::Identifier, "Expected to find an identifier.");
    if (currentToken.type == TokenType::Illegal) {
        advance({TokenType::CloseParen, TokenType::Comma, TokenType::NewLine});
        return nullptr;
    }
    parameterNode.begin = currentToken.pos;
    parameterNode.name = currentToken.lit;

    currentToken = expect(TokenType::Colon, "Expected to find a ':'.");
    if (currentToken.type == TokenType::Illegal) {
        // Note: the following is not correct as it will consume the ) or ,
        //  token expected by `parseRoutineDecl`. A better alternative is needed
        advance({TokenType::CloseParen, TokenType::Comma, TokenType::NewLine});
        return nullptr;
    }

    parameterNode.type = parseType();
    if (parameterNode.type != nullptr) {
        parameterNode.end = parameterNode.type->end;
    }
    return std::make_shared<ast::Parameter>(parameterNode);
}

sPtr<ast::TypeDecl> Parser::parseTypeDecl() {
    ast::TypeDecl typeDeclNode;
    Token currentToken =
        expect(TokenType::Type, "Expected to find \"type\" keyword.");
    if (currentToken.type == TokenType::Illegal) {
        return nullptr;
    }
    typeDeclNode.begin = currentToken.pos;
    currentToken =
        expect(TokenType::Identifier, "Expected to find an identifier.");
    if (currentToken.type == TokenType::Illegal) {
        advance({TokenType::Semicolon, TokenType::NewLine});
        return nullptr;
    }
    typeDeclNode.name = currentToken.lit;
    currentToken = expect(TokenType::Is, "Expected to find \"is\" keyword.");
    if (currentToken.type == TokenType::Illegal) {
        advance({TokenType::Semicolon, TokenType::NewLine});
        return nullptr;
    }

    skipWhitespace();
    typeDeclNode.type = parseType();

    currentToken = expect({TokenType::Semicolon, TokenType::NewLine},
                          "Expected a ';' or a new line.");
    if (currentToken.type == TokenType::Illegal) {
        advance({TokenType::Semicolon, TokenType::NewLine});
        return nullptr;
    }
    typeDeclNode.end = currentToken.pos;
    return std::make_shared<ast::TypeDecl>(typeDeclNode);
}

sPtr<ast::Type> Parser::parseType() {
    Token currentToken = m_lexer.Peek();
    if (TokenType* type = std::find(std::begin(primitives),
                                    std::end(primitives), currentToken.type);
        type != std::end(primitives)) {
        sPtr<ast::PrimitiveType> typeNode;
        switch (currentToken.type) {
        case TokenType::IntegerType:
            typeNode = std::make_shared<ast::IntegerType>();
            break;
        case TokenType::RealType:
            typeNode = std::make_shared<ast::RealType>();
            break;
        case TokenType::Boolean:
            typeNode = std::make_shared<ast::BooleanType>();
            break;
        default:
            m_errors.push_back(Error{
                .pos = currentToken.pos,
                .message = "Unknown primitive type",
            });
            return nullptr;
        }
        typeNode->begin = currentToken.pos;
        currentToken = m_lexer.Next();
        typeNode->end = currentToken.pos;
        return typeNode;
    } else if (currentToken.type == TokenType::Array) {
        return parseArrayType();
    } else if (currentToken.type == TokenType::Record) {
        return parseRecordType();
    } else if (currentToken.type == TokenType::Identifier) {
        ast::AliasedType typeNode;
        typeNode.begin = currentToken.pos;
        typeNode.name = currentToken.lit;
        currentToken = m_lexer.Next();
        typeNode.end = currentToken.pos;
        return std::make_shared<ast::AliasedType>(typeNode);
    } else {
        m_errors.push_back(Error{
            .pos = currentToken.pos,
            .message = "Unknown type.",
        });
        m_lexer.Next();
        return nullptr;
    }
}

sPtr<ast::ArrayType> Parser::parseArrayType() {
    ast::ArrayType arrayNode;
    Token currentToken = expect(
        TokenType::Array, "Expected 'array' keyword but did not find it.");
    if (currentToken.type == TokenType::Illegal) {
        return nullptr;
    }
    arrayNode.begin = currentToken.pos;

    skipWhitespace();
    if (m_lexer.Peek().type == TokenType::OpenBrack) {
        m_lexer.Next();
        arrayNode.length = parseExpression();
        currentToken = expect(TokenType::CloseBrack);
        if (currentToken.type == TokenType::Illegal) {
            advance(TokenType::CloseBrack);
            return nullptr;
        }
    }

    skipWhitespace();
    arrayNode.elementType = parseType();
    if (arrayNode.elementType != nullptr) {
        arrayNode.end = arrayNode.elementType->end;
    }
    return std::make_shared<ast::ArrayType>(arrayNode);
}

sPtr<ast::RecordType> Parser::parseRecordType() {
    Token currentToken = expect(
        TokenType::Record, "Expected 'record' keyword but did not find it");
    if (currentToken.type == TokenType::Illegal) {
        return nullptr;
    }
    ast::RecordType recordNode;
    recordNode.begin = currentToken.pos;

    skipWhitespace();
    while (m_lexer.Peek().type != TokenType::End) {
        recordNode.fields.push_back(parseVariableDecl());
        skipWhitespace();
    }
    currentToken = m_lexer.Next(); // consume "end"
    recordNode.end = currentToken.pos;
    return std::make_shared<ast::RecordType>(recordNode);
}

sPtr<ast::VariableDecl> Parser::parseVariableDecl() {
    ast::VariableDecl variableNode;
    Token currentToken =
        expect(TokenType::Var, "Expected \"var\" keyword but didn't find it.");
    if (currentToken.type == TokenType::Illegal) {
        return nullptr;
    }
    variableNode.begin = currentToken.pos;

    currentToken = expect(TokenType::Identifier,
                          "Expected an identifier after \"var\" keyword.");
    if (currentToken.type == TokenType::Illegal) {
        advance({TokenType::Semicolon, TokenType::NewLine});
        return nullptr;
    }
    variableNode.name = currentToken.lit;

    currentToken =
        expect({TokenType::Colon, TokenType::Is},
               "Expected an 'is' keyword or ':' after the identifier.");
    if (currentToken.type == TokenType::Illegal) {
        advance({TokenType::Semicolon, TokenType::NewLine});
        return nullptr;
    }
    if (currentToken.type == TokenType::Colon) {
        variableNode.type = parseType();
        if (m_lexer.Peek().type == TokenType::Is) {
            m_lexer.Next(); // consume the "is"
            variableNode.expression = parseExpression();
        }
    } else if (currentToken.type == TokenType::Is) {
        variableNode.expression = parseExpression();
    }

    currentToken = expect({TokenType::Semicolon, TokenType::NewLine},
                          "Expected a ';' or a new line.");
    if (currentToken.type == TokenType::Illegal) {
        advance({TokenType::Semicolon, TokenType::NewLine});
        return nullptr;
    }
    variableNode.end = currentToken.pos;
    return std::make_shared<ast::VariableDecl>(variableNode);
}

sPtr<ast::Body> Parser::parseBody() {
    skipWhitespace();
    ast::Body bodyNode;
    Token currentToken = m_lexer.Peek();
    bodyNode.begin = currentToken.pos;
    while (currentToken.type != TokenType::End &&
           currentToken.type != TokenType::Else) {
        switch (currentToken.type) {
        case TokenType::Var:
            bodyNode.variables.push_back(parseVariableDecl());
            break;
        case TokenType::Type:
            bodyNode.types.push_back(parseTypeDecl());
            break;
        case TokenType::NewLine:
            m_lexer.Next();
            break;
        default:
            bodyNode.statements.push_back(parseStatement());
        }
        currentToken = m_lexer.Peek();
    }
    bodyNode.end = currentToken.pos;
    return std::make_shared<ast::Body>(bodyNode);
}

sPtr<ast::Statement> Parser::parseStatement() {
    skipWhitespace();
    auto currentToken = m_lexer.Peek();
    switch (currentToken.type) {
    case TokenType::Identifier: {
        sPtr<ast::Expression> expression = parseExpression();
        if (m_lexer.Peek().type == TokenType::Assign) {
            return parseAssignment(expression);
        }
        // If a line starts with an identifier, it must be a routine call.
        // It is cast to a Primary because it cannot yet be determined if it is
        //  a routine call or a variable name.
        sPtr<ast::Primary> primaryNode =
            std::dynamic_pointer_cast<ast::Primary>(expression);
        if (primaryNode == nullptr) {
            m_errors.push_back(Error{
                .pos = currentToken.pos,
                .message = "Invalid token. Expected routine call",
            });
            return nullptr;
        }
        currentToken = expect({TokenType::NewLine, TokenType::Semicolon});
        if (currentToken.type == TokenType::Illegal) {
            advance({TokenType::NewLine, TokenType::Semicolon});
            return nullptr;
        }
        return primaryNode;
    }
    case TokenType::While:
        return parseWhileLoop();
    case TokenType::For:
        return parseForLoop();
    case TokenType::If:
        return parseIfStatement();
    case TokenType::Return:
        return parseReturnStatement();
    default:
        m_errors.push_back(Error{
            .pos = currentToken.pos,
            .message = "Unexpected token.",
        });
        advance(TokenType::NewLine);
        return nullptr;
    }
}

sPtr<ast::Assignment> Parser::parseAssignment(sPtr<ast::Expression> left) {
    Token currentToken =
        expect(TokenType::Assign, "Expected ':=' but didn't find it.");
    if (currentToken.type == TokenType::Illegal) {
        return nullptr;
    }
    ast::Assignment assignmentNode;
    if (left != nullptr) {
        assignmentNode.begin = left->begin;
    }
    assignmentNode.lhs = left;
    assignmentNode.rhs = parseExpression();
    if (assignmentNode.rhs != nullptr) {
        assignmentNode.end = assignmentNode.rhs->end;
    }
    expect({TokenType::NewLine, TokenType::Semicolon},
           "Expected a ';' or a new line.");
    return std::make_shared<ast::Assignment>(assignmentNode);
}

sPtr<ast::WhileLoop> Parser::parseWhileLoop() {
    // condition: Expr
    // body: Body
    // WhileLoop : while Expression loop Body end
    ast::WhileLoop whileNode;
    Token currentToken = expect(
        TokenType::While, "Expected \"while\" keyword but didn't find it.");
    if (currentToken.type == TokenType::Illegal) {
        return nullptr;
    }
    whileNode.begin = currentToken.pos;

    whileNode.condition = parseExpression();

    currentToken = expect(TokenType::Loop,
                          "Expected \"loop\" keyword but didn't find it.");
    if (currentToken.type == TokenType::Illegal) {
        advance(TokenType::End);
        return nullptr;
    }

    skipWhitespace();

    whileNode.body = parseBody();

    currentToken =
        expect(TokenType::End, "Expected \"end\" keyword but didn't find it.");
    if (currentToken.type == TokenType::Illegal) {
        advance(TokenType::End);
        return nullptr;
    }
    whileNode.end = currentToken.pos;
    return std::make_shared<ast::WhileLoop>(whileNode);
}

sPtr<ast::ForLoop> Parser::parseForLoop() {
    /**
     * ForLoop : for Identifier Range loop Body end
     * Range : in [ reverse ] Expression .. Expression
     * reverse: bool
     */

    ast::ForLoop forNode;
    Token currentToken =
        expect(TokenType::For, "Expected \"for\" keyword but didn't find it.");
    if (currentToken.type == TokenType::Illegal) {
        return nullptr;
    }
    forNode.begin = currentToken.pos;

    currentToken = expect(TokenType::Identifier,
                          "Expected an identifier but did not find one.");
    if (currentToken.type == TokenType::Illegal) {
        advance(TokenType::End);
        return nullptr;
    }
    forNode.loopVar = currentToken.lit;

    currentToken =
        expect(TokenType::In, "Expected \"in\" keyword but didn't find it.");
    if (currentToken.type == TokenType::Illegal) {
        advance(TokenType::End);
        return nullptr;
    }

    skipWhitespace();
    forNode.reverse = (m_lexer.Peek().type == TokenType::Reverse);
    if (forNode.reverse) {
        currentToken = m_lexer.Next(); // consume "reverse" keyword
    }

    skipWhitespace();

    forNode.rangeFrom = parseExpression();

    currentToken =
        expect(TokenType::TwoDots, "Expected \"..\" token but didn't find it.");
    if (currentToken.type == TokenType::Illegal) {
        advance(TokenType::End);
        return nullptr;
    }

    forNode.rangeTo = parseExpression();

    currentToken = expect(TokenType::Loop,
                          "Expected \"loop\" keyword but didn't find it.");
    if (currentToken.type == TokenType::Illegal) {
        advance(TokenType::End);
        return nullptr;
    }

    skipWhitespace();
    forNode.body = parseBody();

    currentToken =
        expect(TokenType::End, "Expected \"end\" keyword but didn't find it.");
    if (currentToken.type == TokenType::Illegal) {
        advance(TokenType::End);
        return nullptr;
    }
    forNode.end = currentToken.pos;
    return std::make_shared<ast::ForLoop>(forNode);
}

sPtr<ast::IfStatement> Parser::parseIfStatement() {
    /**
     * if Expression then Body [ else Body ] end
     */
    ast::IfStatement ifNode;
    Token currentToken =
        expect(TokenType::If, "Expected \"if\" keyword but didn't find it.");
    if (currentToken.type == TokenType::Illegal) {
        return nullptr;
    }
    ifNode.begin = currentToken.pos;

    skipWhitespace();
    ifNode.condition = parseExpression();

    currentToken = expect(TokenType::Then,
                          "Expected \"then\" keyword but didn't find it.");
    if (currentToken.type == TokenType::Illegal) {
        advance(TokenType::End);
        return nullptr;
    }

    skipWhitespace();
    ifNode.ifBody = parseBody();

    currentToken = expect({TokenType::Else, TokenType::End},
                          "Expected either \"else\" or \"end\" keyword.");
    if (currentToken.type == TokenType::Illegal) {
        advance(TokenType::End);
        return nullptr;
    }
    if (currentToken.type == TokenType::Else) {
        ifNode.elseBody = parseBody();
        currentToken = expect(TokenType::End, "Expected \"end\" keyword.");
    }
    if (currentToken.type == TokenType::Illegal) {
        advance(TokenType::End);
        return nullptr;
    }
    ifNode.end = currentToken.pos;
    return std::make_shared<ast::IfStatement>(ifNode);
}

sPtr<ast::ReturnStatement> Parser::parseReturnStatement() {
    ast::ReturnStatement returnNode;
    Token currentToken = expect(
        TokenType::Return, "Expected \"return\" keyword but didn't find it.");
    if (currentToken.type == TokenType::Illegal) {
        return nullptr;
    }
    returnNode.begin = currentToken.pos;

    skipWhitespace();
    returnNode.expression = parseExpression();

    currentToken = expect({TokenType::NewLine, TokenType::Semicolon},
                          "Expected a ';' or a new line.");
    if (currentToken.type == TokenType::Illegal) {
        advance({TokenType::NewLine, TokenType::Semicolon});
        return nullptr;
    }
    returnNode.end = currentToken.pos;
    return std::make_shared<ast::ReturnStatement>(returnNode);
}

sPtr<ast::Expression> Parser::parseExpression() {
    return parseBinaryExpression();
}

sPtr<ast::Expression> Parser::parseUnaryExpression() {
    skipWhitespace();
    Token currentToken = m_lexer.Peek();

    if (opPrec(currentToken.type) >= 0) {
        // if has operations on primary
        if (currentToken.type == TokenType::Not ||
            currentToken.type == TokenType::Sub ||
            currentToken.type == TokenType::Add) {
            // math unary operations
            ast::UnaryExpression exprNode;
            exprNode.begin = currentToken.pos;
            currentToken = m_lexer.Next();
            exprNode.operation = currentToken.type;
            exprNode.operand = parseUnaryExpression();
            if (exprNode.operand != nullptr) {
                exprNode.end = exprNode.operand->end;
            }
            return std::make_shared<ast::UnaryExpression>(exprNode);
        } else if (currentToken.type == TokenType::OpenParen) {
            skipWhitespace();
            // parenthesis -> more priority
            currentToken = m_lexer.Next();
            sPtr<ast::Expression> exprNode = parseBinaryExpression();
            currentToken =
                expect(TokenType::CloseParen, "Expected to find ')'.");
            if (currentToken.type == TokenType::Illegal) {
                advance(TokenType::CloseParen);
                return nullptr;
            }
            return exprNode;
        } else {
            m_errors.push_back(Error{
                .pos = currentToken.pos,
                .message = "Expected to find unary operator.",
            });
            return nullptr;
        }
    } else if (isPrimary(currentToken.type)) {
        // if is pure primary
        ast::Primary primNode;
        primNode.begin = currentToken.pos;
        currentToken = m_lexer.Next();
        // check if parametrized routine call
        if (currentToken.type == TokenType::Identifier &&
            m_lexer.Peek().type == TokenType::OpenParen) {
            return parseRoutineCall(currentToken);
            // TODO: if ambiguous, make an identifier
        }
        // return Primary only
        primNode.value = currentToken.type;
        primNode.end = currentToken.pos;
        return std::make_shared<ast::Primary>(primNode);
    }
    return nullptr;
}

sPtr<ast::Expression> Parser::parseBinaryExpression(int prec1) {
    sPtr<ast::Expression> lhs = parseUnaryExpression();

    for (;;) {
        Token op = m_lexer.Peek();
        int prec = opPrec(op.type);

        if (prec < prec1) {
            return lhs;
        }

        op = m_lexer.Next();

        ast::BinaryExpression expr;
        if (lhs != nullptr) {
            expr.begin = lhs->begin;
        }
        expr.operand1 = lhs;
        expr.operation = op.type;

        if (op.type == TokenType::OpenBrack) {
            expr.operand2 = parseBinaryExpression(0);

            Token currentToken =
                expect(TokenType::CloseBrack, "Expected to find a ']'.");
            if (currentToken.type == TokenType::Illegal) {
                advance(TokenType::CloseBrack);
                return nullptr;
            }
        } else {
            expr.operand2 = parseBinaryExpression(prec + 1);
        }
        expr.end = op.pos;
        lhs = std::make_shared<ast::BinaryExpression>(expr);
    }
}

sPtr<ast::RoutineCall> Parser::parseRoutineCall(Token routineName) {
    ast::RoutineCall rountineCallNode;
    rountineCallNode.routineName = routineName.lit; // save the function name
    rountineCallNode.begin = routineName.pos;

    Token currentToken = m_lexer.Peek();
    // '(' is optional when calling a routine without params
    if (currentToken.type == TokenType::OpenParen) {
        m_lexer.Next(); // consume the '('
        // handle the case of no parameters
        currentToken = m_lexer.Peek();
        if (currentToken.type == TokenType::CloseParen) {
            m_lexer.Next(); // consume the ')'
        }
        while (currentToken.type != TokenType::CloseParen) {
            rountineCallNode.args.push_back(parseExpression());
            currentToken = expect({TokenType::Comma, TokenType::CloseParen});
            if (currentToken.type == TokenType::Illegal) {
                advance(TokenType::CloseParen);
                return nullptr;
            }
        }
    }
    rountineCallNode.end = currentToken.pos;
    return std::make_shared<ast::RoutineCall>(rountineCallNode);
}

int Parser::opPrec(TokenType token) {
    switch (token) {
    case TokenType::Or:  // or
    case TokenType::Xor: // xor
        return 1;
    case TokenType::And: // and
        return 2;
    case TokenType::Eq:  // =
    case TokenType::Neq: // /=
        return 3;
    case TokenType::Less:    // <
    case TokenType::Leq:     // <=
    case TokenType::Greater: // >
    case TokenType::Geq:     // >=
        return 4;
    case TokenType::Add: // +
    case TokenType::Sub: // -
        return 5;
    case TokenType::Mul: // *
    case TokenType::Div: // /
    case TokenType::Mod: // %
        return 6;
    case TokenType::Not:       // not
    case TokenType::OpenParen: // ()
        return 7;
    case TokenType::Dot:       // .
    case TokenType::OpenBrack: // []
        return 8;
    default:
        return -1; // unknown op
    }
}

bool Parser::isPrimary(TokenType token) {
    return token == TokenType::Identifier || token == TokenType::Int ||
           token == TokenType::Real || token == TokenType::True ||
           token == TokenType::False;
}

/**
 * Expects the next token to be one of the given types.
 * Otherwise, appends an error to the array of errors and returns an illegal\
 * token. The next token is consumed (along with any whitespace before it)
 * regardless of whether or not it was a desired token.
 */
Token Parser::expect(std::vector<TokenType> types, std::string err_msg) {
    // if "new line" is not one of the characters we are looking for, then skip
    // any occurence of it
    if (std::find(types.begin(), types.end(), TokenType::NewLine) ==
        types.end()) {
        skipWhitespace();
    }
    Token next = m_lexer.Next();
    if (std::find(types.begin(), types.end(), next.type) == types.end()) {
        m_errors.push_back(Error{
            .pos = next.pos,
            .message = err_msg,
        });
        return Token{.type = TokenType::Illegal};
    }
    return next;
}

/**
 * An overload of `expect` that allows passing just one type for ease of use.
 * Wraps that token type in a vector
 */
Token Parser::expect(TokenType type, std::string err_msg) {
    return expect(std::vector{type}, err_msg);
}

void Parser::skipWhitespace() {
    while (m_lexer.Peek().type == TokenType::NewLine) {
        m_lexer.Next();
    }
}

/**
 * Keeps consuming tokens until it finds one of the desired token types, and
 *  consumes that one as well.
 */
void Parser::advance(std::vector<TokenType> types) {
    Token next = m_lexer.Next();
    while (next.type != TokenType::Eof &&
           std::find(types.begin(), types.end(), next.type) == types.end()) {
        next = m_lexer.Next();
    }
}

/**
 * An overload of `advance` that allows passing just one type for ease of use.
 * Wraps that token type in a vector.
 */
void Parser::advance(TokenType type) { return advance(std::vector{type}); }

std::vector<parser::Error> Parser::getErrors() { return m_errors; }

} // namespace parser
