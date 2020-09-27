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
            programNode.types.push_back(parseType());
            break;
        case TokenType::NewLine:
            m_lexer.Next();
            break;
        default:
            m_errors.push_back(Error{
                .pos = currentToken.pos,
                .message = "Unexpected token",
            });
            while (m_lexer.Peek().type != TokenType::NewLine)
                m_lexer.Next();
        }
        currentToken = m_lexer.Peek();
    }
    programNode.end = currentToken.pos;
    return std::make_shared<ast::Program>(programNode);
}

sPtr<ast::RoutineDecl> Parser::parseRoutineDecl() {
    ast::RoutineDecl routineNode;
    Token currentToken = skipWhile(isNewLine);
    routineNode.begin = currentToken.pos;
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
        while (m_lexer.Next().type != TokenType::End)
            ;
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
        while (m_lexer.Next().type != TokenType::End)
            ;
        return nullptr;
    }

    while (currentToken.type != TokenType::CloseParen) {
        routineNode.parameters.push_back(parseParameter());
        currentToken = skipWhile(isNewLine);
        if (currentToken.type != TokenType::Comma &&
            currentToken.type != TokenType::CloseParen) {
            m_errors.push_back(Error{
                .pos = currentToken.pos,
                .message = "Unexpected token",
            });
        }
    }
    currentToken = skipWhile(isNewLine);
    if (currentToken.type != TokenType::Colon &&
        currentToken.type != TokenType::Is) {
        m_errors.push_back(Error{
            .pos = currentToken.pos,
            .message = "Unexpected token",
        });
        // skip till "end" keyword
        while (m_lexer.Next().type != TokenType::End)
            ;
        return nullptr;
    }
    if (currentToken.type == TokenType::Colon) {
        routineNode.returnType = parseType();
    }
    currentToken = skipWhile(isNewLine);

    if (currentToken.type != TokenType::Is) {
        m_errors.push_back(Error{
            .pos = currentToken.pos,
            .message = "Expected to find keyword 'is'",
        });
        // skip till "end" keyword
        while (m_lexer.Next().type != TokenType::End)
            ;
        return nullptr;
    }
    while (m_lexer.Peek().type != TokenType::NewLine)
        m_lexer.Next();
    routineNode.body = parseBody();
    currentToken = skipWhile(isNewLine);
    if (currentToken.type != TokenType::End) {
        m_errors.push_back(Error{
            .pos = currentToken.pos,
            .message = "Expected to find keyword 'end'",
        });
        // skip till "end" keyword
        while (m_lexer.Next().type != TokenType::End)
            ;
        return nullptr;
    }
    routineNode.end = currentToken.pos;
    return std::make_shared<ast::RoutineDecl>(routineNode);
}

sPtr<ast::Parameter> Parser::parseParameter() {
    ast::Parameter parameterNode;
    Token currentToken = skipWhile(isNewLine);
    parameterNode.begin = currentToken.pos;
    if (currentToken.type != TokenType::Identifier) {
        m_errors.push_back(Error{
            .pos = currentToken.pos,
            .message = "Expected to find an identifier",
        });
        // skip till ')' or ','
        while (m_lexer.Peek().type != TokenType::Comma &&
               m_lexer.Peek().type != TokenType::CloseParen) {
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
        while (m_lexer.Peek().type != TokenType::Comma &&
               m_lexer.Peek().type != TokenType::CloseParen) {
            m_lexer.Next();
        }
        return nullptr;
    }
    parameterNode.type = parseType();
    if (parameterNode.type != nullptr) {
        parameterNode.end = parameterNode.type->end;
    }
    return std::make_shared<ast::Parameter>(parameterNode);
}

sPtr<ast::Type> Parser::parseType() {
    Token currentToken = m_lexer.Peek();
    if (TokenType* type = std::find(std::begin(primitives),
                                    std::end(primitives), currentToken.type);
        type != std::end(primitives)) {
        ast::PrimitiveType typeNode;
        typeNode.begin = currentToken.pos;
        typeNode.type = currentToken;
        m_lexer.Next();
        typeNode.end = currentToken.pos;
        return std::make_shared<ast::PrimitiveType>(typeNode);
    } else if (currentToken.type == TokenType::Array) {
        return parseArrayType();
    } else if (currentToken.type == TokenType::Record) {
        return parseRecordType();
    } else if (currentToken.type == TokenType::Identifier) {
        ast::AliasedType typeNode;
        typeNode.begin = currentToken.pos;
        typeNode.name = currentToken;
        m_lexer.Next();
        typeNode.end = currentToken.pos;
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
    ast::ArrayType arrayNode;
    Token currentToken = m_lexer.Next();
    arrayNode.begin = currentToken.pos;
    if (currentToken.type != TokenType::Array) {
        m_errors.push_back(
            Error{.pos = currentToken.pos,
                  .message = "Expected 'array' keyword but did not find it"});
        return nullptr;
    }
    if (m_lexer.Peek().type == TokenType::OpenBrack) {
        m_lexer.Next();
        arrayNode.length = parseExpression();
        currentToken = m_lexer.Next();
        if (currentToken.type != TokenType::CloseBrack) {
            m_errors.push_back(
                Error{.pos = currentToken.pos, .message = "Unexpected token"});
            while (m_lexer.Next().type != TokenType::CloseBrack)
                ;
            return nullptr;
        }
    }
    arrayNode.elementType = parseType();
    if (arrayNode.elementType != nullptr) {
        arrayNode.end = arrayNode.elementType->end;
    }
    return std::make_shared<ast::ArrayType>(arrayNode);
}

sPtr<ast::RecordType> Parser::parseRecordType() {
    Token currentToken = m_lexer.Next();
    if (currentToken.type != TokenType::Record) {
        m_errors.push_back(
            Error{.pos = currentToken.pos,
                  .message = "Expected 'record' keyword but did not find it"});
        return nullptr;
    }
    ast::RecordType recordNode;
    recordNode.begin = currentToken.pos;
    while (m_lexer.Peek().type == TokenType::NewLine)
        m_lexer.Next();
    while (m_lexer.Peek().type != TokenType::End) {
        recordNode.fields.push_back(parseVariableDecl());
        while (m_lexer.Peek().type == TokenType::NewLine)
            m_lexer.Next();
    }
    currentToken = m_lexer.Next(); // consume "end"
    recordNode.end = currentToken.pos;
    return std::make_shared<ast::RecordType>(recordNode);
}

sPtr<ast::VariableDecl> Parser::parseVariableDecl() {
    ast::VariableDecl variableNode;
    Token currentToken = skipWhile(isNewLine);
    variableNode.begin = currentToken.pos;
    if (currentToken.type != TokenType::Var) {
        m_errors.push_back(Error{
            .pos = currentToken.pos,
            .message = "Expected \"var\" keyword but didn't find it.",
        });
        return nullptr;
    }
    currentToken = skipWhile(isNewLine);
    if (currentToken.type != TokenType::Identifier) {
        m_errors.push_back(Error{
            .pos = currentToken.pos,
            .message = "Expected an identifier after \"var\" keyword.",
        });
        return nullptr;
    }
    variableNode.name = currentToken;
    currentToken = skipWhile(isNewLine);
    if (currentToken.type != TokenType::Is &&
        currentToken.type != TokenType::Colon) {
        m_errors.push_back(Error{
            .pos = currentToken.pos,
            .message = "Expected an 'is' keyword or ':' after the identifier.",
        });
        return nullptr;
    }
    if (currentToken.type == TokenType::Colon) {
        variableNode.type = parseType();
        currentToken = skipWhile(isNewLine);
    }
    if (currentToken.type == TokenType::Is) {
        variableNode.expression = parseExpression();
        currentToken = skipWhile(isNewLine);
    }

    if (currentToken.type != TokenType::Semicolon &&
        currentToken.type != TokenType::NewLine) {
        m_errors.push_back(Error{
            .pos = currentToken.pos,
            .message = "Expected a new line or ';'.",
        });
        return nullptr;
    }
    variableNode.end = currentToken.pos;
    return std::make_shared<ast::VariableDecl>(variableNode);
}

sPtr<ast::Body> Parser::parseBody() {
    ast::Body bodyNode;
    Token currentToken = m_lexer.Peek();
    bodyNode.begin = currentToken.pos;
    while (currentToken.type != TokenType::End) {
        switch (currentToken.type) {
        case TokenType::Var:
            bodyNode.variables.push_back(parseVariableDecl());
            break;
        case TokenType::Type:
            bodyNode.types.push_back(parseType());
            break;
        case TokenType::NewLine:
            m_lexer.Next();
            break;
        default:
            bodyNode.statements.push_back(parseStatement());
        }
        currentToken = m_lexer.Peek();
    }
    currentToken = m_lexer.Next(); // consume "end"
    bodyNode.end = currentToken.pos;
    return std::make_shared<ast::Body>(bodyNode);
}

sPtr<ast::Statement> Parser::parseStatement() {
    // TODO: skip whitespace
    auto currentToken = m_lexer.Peek();
    sPtr<ast::Expression> expression;
    switch (currentToken.type) {
    case TokenType::Identifier:
        expression = parseExpression();
        if (m_lexer.Peek().type == TokenType::Assign) {
            return parseAssignment(expression);
        }
        return std::dynamic_pointer_cast<ast::RoutineCall>(expression);
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
        // TODO: skip to next line
        return nullptr;
    }
}

sPtr<ast::Assignment> Parser::parseAssignment(sPtr<ast::Expression> left) {
    auto currentToken = skipWhile(isNewLine);
    if (currentToken.type != TokenType::Assign) {
        m_errors.push_back(Error{
            .pos = currentToken.pos,
            .message = "Expected ':=' but didn't find it.",
        });
        return nullptr;
    }
    ast::Assignment assignmentNode;
    assignmentNode.begin = currentToken.pos;
    assignmentNode.lhs = left;
    assignmentNode.rhs = parseExpression();
    if (assignmentNode.rhs != nullptr) {
        assignmentNode.end = assignmentNode.rhs->end;
    }
    return std::make_shared<ast::Assignment>(assignmentNode);
}

sPtr<ast::WhileLoop> Parser::parseWhileLoop() {
    // condition: Expr
    // body: Body
    // WhileLoop : while Expression loop Body end
    ast::WhileLoop whileNode;
    Token currentToken = skipWhile(isNewLine);
    whileNode.begin = currentToken.pos;
    if (currentToken.type != TokenType::While) {
        m_errors.push_back(Error{
            .pos = currentToken.pos,
            .message = "Expected \"while\" keyword but didn't find it.",
        });
        return nullptr;
    }
    // read expression
    whileNode.condition = parseExpression();
    // check loop kwd
    currentToken = skipWhile(isNewLine);
    if (currentToken.type != TokenType::Loop) {
        m_errors.push_back(Error{
            .pos = currentToken.pos,
            .message = "Expected \"loop\" keyword but didn't find it.",
        });
        while (m_lexer.Next().type != TokenType::End)
            ;
        return nullptr;
    }
    // read body

    whileNode.body = parseBody();
    currentToken = skipWhile(isNewLine);

    // check end kwd
    if (currentToken.type != TokenType::End) {
        m_errors.push_back(Error{
            .pos = currentToken.pos,
            .message = "Expected to find keyword 'end'",
        });
        // skip till "end" keyword
        while (m_lexer.Next().type != TokenType::End)
            ;
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
    Token currentToken = skipWhile(isNewLine);
    if (currentToken.type != TokenType::For) {
        m_errors.push_back(Error{
            .pos = currentToken.pos,
            .message = "Expected \"for\" keyword but didn't find it.",
        });
        return nullptr;
    }
    forNode.begin = currentToken.pos;
    currentToken = skipWhile(isNewLine);

    // check Identifier
    if (currentToken.type != TokenType::Identifier) {
        m_errors.push_back(Error{
            .pos = currentToken.pos,
            .message = "Expected an identifier but did not find one",
        });
        // skip till "end" keyword
        while (m_lexer.Next().type != TokenType::End)
            ;
        return nullptr;
    }
    forNode.loopVar = currentToken; // should be an Identifier

    // check 'in' keyword
    currentToken = skipWhile(isNewLine);
    if (currentToken.type != TokenType::In) {
        m_errors.push_back(Error{
            .pos = currentToken.pos,
            .message = "Expected \"in\" keyword but didn't find it.",
        });
        while (m_lexer.Next().type != TokenType::End)
            ;
        return nullptr;
    }
    // check optional 'reverse' keyword
    while (m_lexer.Peek().type == TokenType::NewLine)
        m_lexer.Next();
    forNode.reverse = (m_lexer.Peek().type == TokenType::Reverse);
    if (forNode.reverse) {
        currentToken = m_lexer.Next();
    }
    forNode.rangeFrom = parseExpression();
    currentToken = m_lexer.Next();

    if (currentToken.type != TokenType::TwoDots) {
        m_errors.push_back(Error{
            .pos = currentToken.pos,
            .message = "Expected \"..\" token but didn't find it.",
        });
        while (m_lexer.Next().type != TokenType::End)
            ;
        return nullptr;
    }
    forNode.rangeTo = parseExpression();

    currentToken = skipWhile(isNewLine);
    if (currentToken.type != TokenType::Loop) {
        m_errors.push_back(Error{
            .pos = currentToken.pos,
            .message = "Expected \"loop\" keyword but didn't find it.",
        });
        // skip till "end" keyword
        while (m_lexer.Next().type != TokenType::End)
            ;
        return nullptr;
    }
    forNode.body = parseBody();
    currentToken = skipWhile(isNewLine);

    if (currentToken.type != TokenType::End) {
        m_errors.push_back(Error{
            .pos = currentToken.pos,
            .message = "Expected to find keyword 'end'",
        });
        // skip till "end" keyword
        while (m_lexer.Next().type != TokenType::End)
            ;
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
    Token currentToken = skipWhile(isNewLine);
    ifNode.begin = currentToken.pos;

    if (currentToken.type != TokenType::If) {
        m_errors.push_back(Error{
            .pos = currentToken.pos,
            .message = "Expected \"if\" keyword but didn't find it.",
        });
        return nullptr;
    }
    ifNode.condition = parseExpression();

    currentToken = skipWhile(isNewLine);
    if (currentToken.type != TokenType::Then) {
        m_errors.push_back(Error{
            .pos = currentToken.pos,
            .message = "Expected \"then\" keyword but didn't find it.",
        });
        // skip till "end" keyword
        while (m_lexer.Next().type != TokenType::End)
            ;
        return nullptr;
    }
    ifNode.ifBody = parseBody();

    currentToken = skipWhile(isNewLine);
    if (currentToken.type == TokenType::Else) {
        ifNode.elseBody = parseBody();
        currentToken = skipWhile(isNewLine);
    }

    if (currentToken.type != TokenType::End) {
        m_errors.push_back(Error{
            .pos = currentToken.pos,
            .message = "Expected to find keyword 'end'",
        });
        // skip till "end" keyword
        while (m_lexer.Next().type != TokenType::End)
            ;
        return nullptr;
    }
    ifNode.end = currentToken.pos;
    return std::make_shared<ast::IfStatement>(ifNode);
}

sPtr<ast::Expression> Parser::parseExpression() {
    return parseBinaryExpression();
}

sPtr<ast::Expression> Parser::parseUnaryExpression() {
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
            exprNode.operation = currentToken;
            exprNode.operand = parseUnaryExpression();
            if (exprNode.operand != nullptr) {
                exprNode.end = exprNode.operand->end;
            }
            return std::make_shared<ast::UnaryExpression>(exprNode);
        } else if (currentToken.type == TokenType::OpenParen) {
            // parenthesis -> more priority
            currentToken = m_lexer.Next();
            sPtr<ast::Expression> exprNode = parseBinaryExpression();
            if (m_lexer.Peek().type != TokenType::CloseParen) {
                m_errors.push_back(Error{
                    .pos = currentToken.pos,
                    .message = "Expected to find ')'",
                });
                while (m_lexer.Peek().type != TokenType::CloseParen)
                    m_lexer.Next();
                return nullptr;
            }
            currentToken = m_lexer.Next(); // read ')'
            return exprNode;
        } else {
            m_errors.push_back(Error{
                .pos = currentToken.pos,
                .message = "Expected to find unary operator",
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
        // return Primitive only
        primNode.value = currentToken;
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
        expr.operation = op;

        if (op.type == TokenType::OpenBrack) {
            expr.operand2 = parseBinaryExpression(0);
            // if op == [ -> check the end ']'
            if (m_lexer.Peek().type != TokenType::CloseBrack) {
                m_errors.push_back(Error{
                    .pos = m_lexer.Peek().pos,
                    .message = "Expected to find a ']' token",
                });
                return nullptr;
            }
            m_lexer.Next();
        } else {
            expr.operand2 = parseBinaryExpression(prec + 1);
        }
        expr.end = op.pos;
        lhs = std::make_shared<ast::BinaryExpression>(expr);
    }
}

sPtr<ast::RoutineCall> Parser::parseRoutineCall(Token routineName) {
    ast::RoutineCall rountineCallNode;
    rountineCallNode.routine = routineName; // save the function name
    rountineCallNode.begin = routineName.pos;
    Token currentToken = m_lexer.Peek();
    if (currentToken.type != TokenType::OpenParen) {
        m_errors.push_back(Error{
            .pos = currentToken.pos,
            .message = "Expected to find '('",
        });
        return nullptr;
    }
    do {
        m_lexer.Next(); // first call-read '(', others - ','
        sPtr<ast::Expression> e = parseExpression();
        if (e != nullptr) {
            rountineCallNode.args.push_back(e);
        }
    } while (m_lexer.Peek().type == TokenType::Comma);

    if (m_lexer.Peek().type != TokenType::CloseParen) {
        m_errors.push_back(Error{
            .pos = m_lexer.Peek().pos,
            .message = "Expected to find ')'",
        });

        return nullptr;
    }
    currentToken = m_lexer.Next(); // read ')'
    rountineCallNode.end = currentToken.pos;
    return std::make_shared<ast::RoutineCall>(rountineCallNode);
}

bool Parser::isNewLine(Token tok) { return tok.type == TokenType::NewLine; }

/**
 * Skips all tokens that match the given predicate, returning the first
 * token that doesn't
 */
Token Parser::skipWhile(std::function<bool(Token)> pred) {
    while (pred(m_lexer.Peek())) {
        m_lexer.Next();
    }
    return m_lexer.Next();
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

std::vector<parser::Error> Parser::getErrors() { return m_errors; }

} // namespace parser
