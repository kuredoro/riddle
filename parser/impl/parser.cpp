#include "parser.hpp"
#include "token.hpp"

namespace parser {

using lexer::Token;
using lexer::TokenType;

// ---- @kureduro

static std::array<TokenType, 2> primitives{
    TokenType::IntegerType,
    TokenType::RealType,
};

sPtr<ast::Program> Parser::parseProgram() {
    ast::Program programNode;
    Token currentToken;
    while ((currentToken = m_lexer.Peek()).type != TokenType::Eof) {
        switch (currentToken.type) {
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
            while (m_lexer.Peek().type != TokenType::NewLine)
                m_lexer.Next();
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
        while (m_lexer.Next().type != TokenType::End)
            ;
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
        while (m_lexer.Next().type != TokenType::End)
            ;
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
    return std::make_shared<ast::Parameter>(parameterNode);
}

sPtr<ast::Type> Parser::parseType() {
    Token currentToken = m_lexer.Peek();
    if (TokenType* type = std::find(std::begin(primitives),
                                    std::end(primitives), currentToken.type);
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
        m_errors.push_back(
            Error{.pos = currentToken.pos,
                  .message = "Expected 'array' keyword but did not find it"});
        return nullptr;
    }
    ast::ArrayType arrayNode;
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
    while (m_lexer.Peek().type == TokenType::NewLine)
        m_lexer.Next();
    while (m_lexer.Peek().type != TokenType::End) {
        recordNode.fields.push_back(parseVariable());
        while (m_lexer.Peek().type == TokenType::NewLine)
            m_lexer.Next();
    }
    m_lexer.Next();
    return std::make_shared<ast::RecordType>(recordNode);
}

// ---- @CrazyDream1

sPtr<ast::Variable> Parser::parseVariable() {
    auto token = m_lexer.Next();
    if (token.type != TokenType::Var) {
        m_errors.push_back(Error{
            .pos = token.pos,
            .message = "Expected \"var\" keyword but didn't find it.",
        });
    }
    token = m_lexer.Next();
    if (token.type != TokenType::Identifier) {
        m_errors.push_back(Error{
            .pos = token.pos,
            .message = "Expected an identifier after \"var\" keyword.",
        });
    }
    // ...
    return nullptr;
}

sPtr<ast::Body> Parser::parseBody() { return nullptr; }

// ---- @MefAldemisov

sPtr<ast::WhileLoop> Parser::parseWhileLoop() {
    // condition: Expr
    // body: Body
    // WhileLoop : while Expression loop Body end
    ast::WhileLoop whileNode;
    Token currentToken = skipWhile(isNewLine);
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
    return std::make_shared<ast::WhileLoop>(whileNode);
}

sPtr<ast::ForLoop> Parser::parseForLoop() {
    /** ForLoop : for Identifier Range loop Body end
    Range : in [ reverse ] Expression .. Expression

    loop var: Identifier
    rangeFrom: Expression
    rangeTo: Expression
    reverse: bool
    body: Body
    */

    // check 'for' kwd
    ast::ForLoop forNode;
    Token currentToken = skipWhile(isNewLine);
    if (currentToken.type != TokenType::For) {
        m_errors.push_back(Error{
            .pos = currentToken.pos,
            .message = "Expected \"for\" keyword but didn't find it.",
        });
        return nullptr;
    }
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
    // check 'in' kwd
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
    // check optional 'reverse' kwd
    while (m_lexer.Peek().type == TokenType::NewLine)
        m_lexer.Next();
    forNode.reverse = (m_lexer.Peek().type == TokenType::Reverse);
    if (forNode.reverse) {
        currentToken = m_lexer.Next();
    }
    // check Expression
    forNode.rangeFrom = parseExpression();
    currentToken = m_lexer.Next();

    // check two dots
    if (currentToken.type != TokenType::TwoDots) {
        m_errors.push_back(Error{
            .pos = currentToken.pos,
            .message = "Expected \"..\" token but didn't find it.",
        });
        while (m_lexer.Next().type != TokenType::End)
            ;
        return nullptr;
    }
    // check Expression
    forNode.rangeTo = parseExpression();

    // check loop kwd
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
    // read body
    forNode.body = parseBody();
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
    return std::make_shared<ast::ForLoop>(forNode);
}

sPtr<ast::IfStatement> Parser::parseIfStatement() {

    // if Expression then Body [ else Body ] end
    // cond: Expression
    // then: Body
    // else: Body
    // check if token
    ast::IfStatement ifNode;
    Token currentToken = skipWhile(isNewLine);

    if (currentToken.type != TokenType::If) {
        m_errors.push_back(Error{
            .pos = currentToken.pos,
            .message = "Expected \"if\" keyword but didn't find it.",
        });
        return nullptr;
    }
    // read condition
    ifNode.condition = parseExpression();

    // check then kwd
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
    // read  if body
    ifNode.ifBody = parseBody();
    // check if ther is an 'else' kwd
    currentToken = skipWhile(isNewLine);

    if (currentToken.type == TokenType::Else) {
        // read else body
        ifNode.elseBody = parseBody();
        currentToken = skipWhile(isNewLine);
    }

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
    return std::make_shared<ast::IfStatement>(ifNode);
}

// ---- @aabounegm

sPtr<ast::Expression> Parser::parseExpression() {
    return parseBinaryExpression();
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

bool Parser::isPrimitive(TokenType token) {
    return token == TokenType::Identifier || token == TokenType::Int ||
           token == TokenType::Real || token == TokenType::True ||
           token == TokenType::False;
}

sPtr<ast::Expression> Parser::parseUnaryExpression() {
    Token tok = m_lexer.Peek();
    ast::Expression expr;
    if (opPrec(tok.type) >= 0) {
        if (tok.type == TokenType::Not || tok.type == TokenType::Sub ||
            tok.type == TokenType::Add) {

            tok = m_lexer.Next();
            expr.operation = tok;
            expr.operand1 = parseUnaryExpression();
            return std::make_shared<ast::Expression>(expr);

        } else if (tok.type == TokenType::OpenParen) {
            tok = m_lexer.Next();
            sPtr<ast::Expression> expr = parseBinaryExpression(0);
            if (m_lexer.Peek().type != TokenType::CloseParen) {
                m_errors.push_back(Error{
                    .pos = tok.pos,
                    .message = "Expected to find ')'",
                });
                while (m_lexer.Peek().type != TokenType::CloseParen)
                    m_lexer.Next();
                return nullptr;
            }
            tok = m_lexer.Next();
            return expr;
        } else {
            // throw error
            m_errors.push_back(Error{
                .pos = tok.pos,
                .message = "Expected to find unary operator",
            });
            return nullptr;
        }
    } else if (isPrimitive(tok.type)) {
        tok = m_lexer.Next();
        // check if More Then Identifier
        if (tok.type == TokenType::Identifier) {

            Token t = m_lexer.Peek();
            if (t.type == TokenType::OpenParen) {
                // parse routineCall:
                ast::RoutineCall rc;
                rc.routine = tok; // save the function name
                expr.operation = t;
                do {
                    t = m_lexer.Next(); // first call-read '(', others - ','
                    // read expression
                    sPtr<ast::Expression> e = parseExpression();
                    // append to the vector
                    rc.args.push_back(e);

                } while (m_lexer.Peek().type == TokenType::Comma);

                if (m_lexer.Peek().type != TokenType::CloseParen) {
                    m_errors.push_back(Error{
                        .pos = m_lexer.Peek().pos,
                        .message = "Expected to find ')'",
                    });
                    return nullptr;
                }
                t = m_lexer.Next(); // read ')'
                expr.operand1 = std::make_shared<ast::RoutineCall>(rc);
                return std::make_shared<ast::Expression>(expr);

            } else if (t.type == TokenType::Dot ||
                       t.type == TokenType::OpenBrack) {
                // we can have a.b.c[7+9].d[0].a
                // Identifier { . Identifier | [ Expression ] }
                ast::ModifiablePrimary mp;
                // ast::Primitive root;
                // root.value = tok;
                // expr.operation = t;

                // mp.args.push_back(std::make_shared<ast::Expression>(root));

                while (m_lexer.Peek().type == TokenType::Dot ||
                       m_lexer.Peek().type == TokenType::OpenBrack) {

                    t = m_lexer.Next(); // read either dot or '['
                    ast::Primitive e;
                    e.value = t;
                    mp.args.push_back(std::make_shared<ast::Primitive>(e));
                    if (t.type == TokenType::Dot) {
                        // read, check and save identifier

                        if (m_lexer.Peek().type != TokenType::Identifier) {
                            m_errors.push_back(Error{
                                .pos = m_lexer.Peek().pos,
                                .message = "Expected to find an identifier",
                            });
                            return nullptr;
                        }
                        e.value = m_lexer.Next();
                        mp.args.push_back(std::make_shared<ast::Primitive>(e));

                    } else { // if []

                        mp.args.push_back(parseBinaryExpression(0));

                        // read and save expression
                        if (m_lexer.Peek().type != TokenType::CloseBrack) {
                            m_errors.push_back(Error{
                                .pos = m_lexer.Peek().pos,
                                .message = "Expected to find a ']' token",
                            });
                            return nullptr;
                        }
                        t = m_lexer.Next();
                    }
                }
                expr.operand1 = std::make_shared<ast::ModifiablePrimary>(mp);
                return std::make_shared<ast::Expression>(expr);
            }
        }
        ast::Expression expr;
        ast::Primitive prim;
        prim.value = tok;
        expr.operand1 = std::make_shared<ast::Primitive>(prim);
        return std::make_shared<ast::Expression>(expr);
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

        ast::Expression expr;
        expr.operand1 = lhs;
        expr.operation = op;
        expr.operand2 = parseBinaryExpression(prec + 1);
        lhs = std::make_shared<ast::Expression>(expr);
    }
}

// ---- End separation

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

std::vector<parser::Error> Parser::getErrors() { return m_errors; }

} // namespace parser
