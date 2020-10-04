#include "token.hpp"
#include <algorithm>
#include <memory>

#include "parser.hpp"

#define RETURN_ON_FAIL()                                                       \
    {                                                                          \
        if (m_current.type == lexer::TokenType::Illegal) {                     \
            return nullptr;                                                    \
        }                                                                      \
    }
#define ADVANCE_ON_FAIL(...)                                                   \
    {                                                                          \
        if (m_current.type == lexer::TokenType::Illegal) {                     \
            advance(__VA_ARGS__);                                              \
            return nullptr;                                                    \
        }                                                                      \
    }

namespace parser {

using lexer::Token;
using lexer::TokenType;

static const std::array<TokenType, 3> primitives{
    TokenType::IntegerType,
    TokenType::RealType,
    TokenType::Boolean,
};

namespace util {

inline bool HasPrimitiveType(Token tok) {
    return std::count(primitives.begin(), primitives.end(), tok.type) != 0;
}

template <typename Container, typename T>
inline auto Contains(const Container& data, const T& value)
    -> decltype(std::end(data), true) {
    return std::end(data) != std::find(std::begin(data), std::end(data), value);
}

std::string GenExpectMessage(const std::vector<TokenType>& types) {
    if (types.size() == 0) {
        return "expected nothing";
    }

    if (types.size() == 1) {
        return fmt::format("expected {}", lexer::to_string(types[0]));
    }

    if (types.size() == 2) {
        return fmt::format("expected {} or {}", lexer::to_string(types[0]),
                           lexer::to_string(types[1]));
    }

    std::string msg = "expected ";
    for (int i = 0; i < (int)types.size() - 1; i++) {
        msg += lexer::to_string(types[i]) + ", ";
    }

    return msg + ", or " + lexer::to_string(types.back());
}

} // namespace util

sPtr<ast::Program> Parser::parseProgram() {
    ast::Program programNode;
    peek();
    programNode.begin = m_current.pos;
    while (m_current.type != TokenType::Eof) {
        switch (m_current.type) {
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
            next();
            break;
        case TokenType::Eof:
            break;
        default:
            error("unexpected token");
            advance(TokenType::NewLine);
        }
        if (m_current.type != TokenType::Eof) {
            peek();
        }
    }

    programNode.end = m_current.pos;
    return std::make_shared<ast::Program>(programNode);
}

sPtr<ast::RoutineDecl> Parser::parseRoutineDecl() {

    expect(TokenType::Routine);
    RETURN_ON_FAIL();

    ast::RoutineDecl routineNode;
    routineNode.begin = m_current.pos;

    expect(TokenType::Identifier);
    ADVANCE_ON_FAIL(TokenType::End);

    routineNode.name = m_current.lit;

    expect(TokenType::OpenParen);
    ADVANCE_ON_FAIL(TokenType::End);

    skipWhitespace();

    // handle the case of no parameters
    peek();

    if (m_current.type == TokenType::CloseParen) {
        m_lexer.Next();
    }

    while (m_current.type != TokenType::CloseParen) {
        routineNode.parameters.push_back(parseParameter());

        expect({TokenType::Comma, TokenType::CloseParen});
        ADVANCE_ON_FAIL({TokenType::CloseParen, TokenType::End});
    }

    expect({TokenType::Colon, TokenType::Is});
    ADVANCE_ON_FAIL(TokenType::End);

    if (m_current.type == TokenType::Colon) {
        skipWhitespace();
        routineNode.returnType = parseType();
        expect(TokenType::Is);
        ADVANCE_ON_FAIL(TokenType::End);
    }

    skipWhitespace();

    routineNode.body = parseBody();

    expect(TokenType::End);
    ADVANCE_ON_FAIL(TokenType::End);

    routineNode.end = m_current.pos;
    return std::make_shared<ast::RoutineDecl>(routineNode);
}

sPtr<ast::VariableDecl> Parser::parseParameter() {
    expect(TokenType::Identifier);
    ADVANCE_ON_FAIL(
        {TokenType::CloseParen, TokenType::Comma, TokenType::NewLine});

    ast::VariableDecl parameterNode;
    parameterNode.begin = m_current.pos;
    parameterNode.name = m_current.lit;

    expect(TokenType::Colon);
    // Note: the following is not correct as it will consume the ) or ,
    //  token expected by `parseRoutineDecl`. A better alternative is needed
    ADVANCE_ON_FAIL(
        {TokenType::CloseParen, TokenType::Comma, TokenType::NewLine});

    parameterNode.type = parseType();

    if (parameterNode.type != nullptr) {
        parameterNode.end = parameterNode.type->end;
    }

    return std::make_shared<ast::VariableDecl>(parameterNode);
}

sPtr<ast::TypeDecl> Parser::parseTypeDecl() {
    expect(TokenType::Type);
    RETURN_ON_FAIL();

    ast::TypeDecl typeDeclNode;
    typeDeclNode.begin = m_current.pos;

    expect(TokenType::Identifier);
    ADVANCE_ON_FAIL({TokenType::Semicolon, TokenType::NewLine});

    typeDeclNode.name = m_current.lit;

    expect(TokenType::Is);
    ADVANCE_ON_FAIL({TokenType::Semicolon, TokenType::NewLine});

    skipWhitespace();
    typeDeclNode.type = parseType();

    expect({TokenType::Semicolon, TokenType::NewLine});
    ADVANCE_ON_FAIL({TokenType::Semicolon, TokenType::NewLine});

    typeDeclNode.end = m_current.pos;
    return std::make_shared<ast::TypeDecl>(typeDeclNode);
}

sPtr<ast::Type> Parser::parseType() {
    peek();

    if (util::HasPrimitiveType(m_current)) {
        sPtr<ast::PrimitiveType> typeNode;
        switch (m_current.type) {
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
            error("unknown primitive type");
            return nullptr;
        }
        typeNode->begin = m_current.pos;
        next();
        typeNode->end = m_current.pos;
        return typeNode;
    } else if (m_current.type == TokenType::Array) {
        return parseArrayType();
    } else if (m_current.type == TokenType::Record) {
        return parseRecordType();
    } else if (m_current.type == TokenType::Identifier) {
        ast::AliasedType typeNode;
        typeNode.begin = m_current.pos;
        typeNode.name = m_current.lit;
        next();
        typeNode.end = m_current.pos;
        return std::make_shared<ast::AliasedType>(typeNode);
    } else {
        error("unknown type");
        m_lexer.Next();
        return nullptr;
    }
}

sPtr<ast::ArrayType> Parser::parseArrayType() {
    expect(TokenType::Array);
    RETURN_ON_FAIL();

    ast::ArrayType arrayNode;
    arrayNode.begin = m_current.pos;

    skipWhitespace();
    if (m_lexer.Peek().type == TokenType::OpenBrack) {
        m_lexer.Next();
        arrayNode.length = parseExpression();
        expect(TokenType::CloseBrack);
        ADVANCE_ON_FAIL(TokenType::CloseBrack);
    }

    skipWhitespace();
    arrayNode.elementType = parseType();

    if (arrayNode.elementType != nullptr) {
        arrayNode.end = arrayNode.elementType->end;
    }

    return std::make_shared<ast::ArrayType>(arrayNode);
}

sPtr<ast::RecordType> Parser::parseRecordType() {
    expect(TokenType::Record);
    RETURN_ON_FAIL();

    ast::RecordType recordNode;
    recordNode.begin = m_current.pos;

    skipWhitespace();
    while (m_lexer.Peek().type != TokenType::End) {
        recordNode.fields.push_back(parseVariableDecl());
        skipWhitespace();
    }

    next(); // consume "end"

    recordNode.end = m_current.pos;
    return std::make_shared<ast::RecordType>(recordNode);
}

sPtr<ast::VariableDecl> Parser::parseVariableDecl() {
    expect(TokenType::Var);
    RETURN_ON_FAIL();

    ast::VariableDecl variableNode;
    variableNode.begin = m_current.pos;

    expect(TokenType::Identifier); // ... after 'var'"
    ADVANCE_ON_FAIL({TokenType::Semicolon, TokenType::NewLine});

    variableNode.name = m_current.lit;

    expect({TokenType::Colon, TokenType::Is}); // ... after identifier"
    ADVANCE_ON_FAIL({TokenType::Semicolon, TokenType::NewLine});

    if (m_current.type == TokenType::Colon) {
        variableNode.type = parseType();

        if (m_lexer.Peek().type == TokenType::Is) {
            m_lexer.Next(); // consume the "is"
            variableNode.initialValue = parseExpression();
        }
    } else if (m_current.type == TokenType::Is) {
        variableNode.initialValue = parseExpression();
    }

    expect({TokenType::Semicolon, TokenType::NewLine});
    ADVANCE_ON_FAIL({TokenType::Semicolon, TokenType::NewLine});

    variableNode.end = m_current.pos;
    return std::make_shared<ast::VariableDecl>(variableNode);
}

sPtr<ast::Body> Parser::parseBody() {
    skipWhitespace();
    peek();

    ast::Body bodyNode;
    bodyNode.begin = m_current.pos;
    while (m_current.type != TokenType::End &&
           m_current.type != TokenType::Else) {
        switch (m_current.type) {
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

        peek();
    }

    bodyNode.end = m_current.pos;
    return std::make_shared<ast::Body>(bodyNode);
}

sPtr<ast::Statement> Parser::parseStatement() {
    skipWhitespace();
    peek();

    switch (m_current.type) {
    case TokenType::Identifier: {
        auto expression = parseExpression();
        if (m_lexer.Peek().type == TokenType::Assign) {
            return parseAssignment(expression);
        }
        // If a line starts with an identifier, it must be a routine call.
        // It is cast to a Primary because it cannot yet be determined if it is
        //  a routine call or a variable name.
        auto primaryNode = std::dynamic_pointer_cast<ast::Primary>(expression);
        if (primaryNode == nullptr) {
            error("invalid token, expected a routine call");
            return nullptr;
        }

        expect({TokenType::Semicolon, TokenType::NewLine});
        ADVANCE_ON_FAIL({TokenType::Semicolon, TokenType::NewLine});

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
        error("unexpected token");
        advance(TokenType::NewLine);
        return nullptr;
    }
}

sPtr<ast::Assignment> Parser::parseAssignment(sPtr<ast::Expression> left) {
    expect(TokenType::Assign);
    RETURN_ON_FAIL();

    ast::Assignment assignmentNode;
    if (left != nullptr) {
        assignmentNode.begin = left->begin;
    }

    assignmentNode.lhs = left;
    assignmentNode.rhs = parseExpression();

    if (assignmentNode.rhs != nullptr) {
        assignmentNode.end = assignmentNode.rhs->end;
    }

    expect({TokenType::Semicolon, TokenType::NewLine});
    ADVANCE_ON_FAIL({TokenType::Semicolon, TokenType::NewLine});

    return std::make_shared<ast::Assignment>(assignmentNode);
}

sPtr<ast::WhileLoop> Parser::parseWhileLoop() {
    // condition: Expr
    // body: Body
    // WhileLoop : while Expression loop Body end
    expect(TokenType::While);
    RETURN_ON_FAIL();

    ast::WhileLoop whileNode;
    whileNode.begin = m_current.pos;
    whileNode.condition = parseExpression();

    expect(TokenType::Loop);
    ADVANCE_ON_FAIL(TokenType::End);

    skipWhitespace();

    whileNode.body = parseBody();

    expect(TokenType::End);
    ADVANCE_ON_FAIL(TokenType::End);

    whileNode.end = m_current.pos;
    return std::make_shared<ast::WhileLoop>(whileNode);
}

sPtr<ast::ForLoop> Parser::parseForLoop() {
    /**
     * ForLoop : for Identifier Range loop Body end
     * Range : in [ reverse ] Expression .. Expression
     * reverse: bool
     */
    expect(TokenType::For);
    RETURN_ON_FAIL();

    ast::ForLoop forNode;
    forNode.begin = m_current.pos;

    expect(TokenType::Identifier);
    ADVANCE_ON_FAIL(TokenType::End);

    forNode.loopVar = std::make_shared<ast::VariableDecl>();
    forNode.loopVar->begin = m_current.pos;
    forNode.loopVar->name = m_current.lit;
    forNode.loopVar->type = std::make_shared<ast::IntegerType>();
    forNode.loopVar->end = m_current.pos;

    expect(TokenType::In);
    ADVANCE_ON_FAIL(TokenType::End);

    skipWhitespace();
    forNode.reverse = (m_lexer.Peek().type == TokenType::Reverse);
    if (forNode.reverse) {
        next(); // consume "reverse" keyword
    }

    skipWhitespace();
    forNode.rangeFrom = parseExpression();

    expect(TokenType::TwoDots);
    ADVANCE_ON_FAIL(TokenType::End);

    forNode.rangeTo = parseExpression();

    expect(TokenType::Loop);
    ADVANCE_ON_FAIL(TokenType::End);

    skipWhitespace();
    forNode.body = parseBody();

    expect(TokenType::End);
    ADVANCE_ON_FAIL(TokenType::End);

    forNode.end = m_current.pos;
    return std::make_shared<ast::ForLoop>(forNode);
}

sPtr<ast::IfStatement> Parser::parseIfStatement() {
    /**
     * if Expression then Body [ else Body ] end
     */
    expect(TokenType::If);
    RETURN_ON_FAIL();

    ast::IfStatement ifNode;
    ifNode.begin = m_current.pos;

    skipWhitespace();
    ifNode.condition = parseExpression();

    expect(TokenType::Then);
    ADVANCE_ON_FAIL(TokenType::End);

    skipWhitespace();
    ifNode.ifBody = parseBody();

    expect({TokenType::Else, TokenType::End});
    ADVANCE_ON_FAIL(TokenType::End);

    if (m_current.type == TokenType::Else) {
        ifNode.elseBody = parseBody();
        expect(TokenType::End);
        ADVANCE_ON_FAIL(TokenType::End);
    }

    ifNode.end = m_current.pos;
    return std::make_shared<ast::IfStatement>(ifNode);
}

sPtr<ast::ReturnStatement> Parser::parseReturnStatement() {
    expect(TokenType::Return);
    RETURN_ON_FAIL();

    ast::ReturnStatement returnNode;
    returnNode.begin = m_current.pos;

    skipWhitespace();
    returnNode.expression = parseExpression();

    expect({TokenType::Semicolon, TokenType::NewLine});
    ADVANCE_ON_FAIL({TokenType::Semicolon, TokenType::NewLine});

    returnNode.end = m_current.pos;
    return std::make_shared<ast::ReturnStatement>(returnNode);
}

sPtr<ast::Expression> Parser::parseExpression() {
    return parseBinaryExpression();
}

sPtr<ast::Expression> Parser::parseUnaryExpression() {
    skipWhitespace();
    peek();

    if (opPrec(m_current.type) >= 0) {
        // if has operations on primary
        if (m_current.type == TokenType::Not ||
            m_current.type == TokenType::Sub ||
            m_current.type == TokenType::Add) {

            // math unary operations
            ast::UnaryExpression exprNode;
            exprNode.begin = m_current.pos;

            next();
            exprNode.operation = m_current.type;

            exprNode.operand = parseUnaryExpression();
            if (exprNode.operand != nullptr) {
                exprNode.end = exprNode.operand->end;
            }

            return std::make_shared<ast::UnaryExpression>(exprNode);

        } else if (m_current.type == TokenType::OpenParen) {
            skipWhitespace();
            // parenthesis -> more priority
            next();

            auto exprNode = parseBinaryExpression();

            expect(TokenType::CloseParen);
            ADVANCE_ON_FAIL(TokenType::CloseParen);

            return exprNode;
        }

        error("expected unary operator");
        return nullptr;

    } else if (isPrimary(m_current.type)) {
        // if is pure primary
        next();

        // check if parametrized routine call
        if (m_current.type == TokenType::Identifier &&
            m_lexer.Peek().type == TokenType::OpenParen) {
            return parseRoutineCall(m_current);
        }

        sPtr<ast::Expression> primNode;
        switch (m_current.type) {
        case TokenType::Int:
            primNode = std::make_shared<ast::IntegerLiteral>(
                std::stoll(m_current.lit));
            break;
        case TokenType::Real:
            primNode =
                std::make_shared<ast::RealLiteral>(std::stod(m_current.lit));
            break;
        case TokenType::True:
            primNode = std::make_shared<ast::BooleanLiteral>(true);
            break;
        case TokenType::False:
            primNode = std::make_shared<ast::BooleanLiteral>(false);
            break;
        case TokenType::Identifier: // can possibly be a routine call
            primNode = std::make_shared<ast::Identifier>(m_current.lit);
            break;
        default:
            error("unknown primary expression");
            return nullptr;
        }

        primNode->begin = m_current.pos;
        primNode->end = m_current.pos;

        return primNode;
    }

    return nullptr;
}

sPtr<ast::Expression> Parser::parseBinaryExpression(int prec1) {
    auto lhs = parseUnaryExpression();

    for (;;) {
        auto op = m_lexer.Peek();
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

            expect(TokenType::CloseBrack);
            ADVANCE_ON_FAIL(TokenType::CloseBrack);
        } else {
            expr.operand2 = parseBinaryExpression(prec + 1);
        }

        expr.end = expr.operand2->end;
        lhs = std::make_shared<ast::BinaryExpression>(expr);
    }
}

sPtr<ast::RoutineCall> Parser::parseRoutineCall(Token routineName) {
    ast::RoutineCall rountineCallNode;
    rountineCallNode.routineName = routineName.lit; // save the function name
    rountineCallNode.begin = routineName.pos;

    peek();
    // '(' is optional when calling a routine without params
    if (m_current.type == TokenType::OpenParen) {
        next(); // consume the '('
        // handle the case of no parameters
        peek();
        if (m_current.type == TokenType::CloseParen) {
            m_lexer.Next(); // consume the ')'
        }

        while (m_current.type != TokenType::CloseParen) {
            rountineCallNode.args.push_back(parseExpression());

            expect({TokenType::Comma, TokenType::CloseParen});
            ADVANCE_ON_FAIL(TokenType::CloseParen);
        }
    }

    rountineCallNode.end = m_current.pos;
    return std::make_shared<ast::RoutineCall>(rountineCallNode);
}

int Parser::opPrec(const TokenType& token) {
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

bool Parser::isPrimary(const TokenType& token) {
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
void Parser::expect(const std::vector<TokenType>& types, std::string errMsg) {
    // if "new line" is not one of the characters we are looking for, then skip
    // any occurence of it
    if (!util::Contains(types, TokenType::NewLine)) {
        skipWhitespace();
    }

    auto next = m_lexer.Next();
    if (!util::Contains(types, next.type)) {
        if (errMsg.empty()) {
            errMsg = util::GenExpectMessage(types) + ", got " +
                     lexer::to_string(next.type);
        }
        error(next, errMsg);
        next = Token{.type = TokenType::Illegal};
    }

    m_current = next;
}

/**
 * An overload of `expect` that allows passing just one type for ease of use.
 * Wraps that token type in a vector
 */
void Parser::expect(const TokenType& type, std::string errMsg) {
    return expect(std::vector{type}, errMsg);
}

void Parser::next() { m_current = m_lexer.Next(); }

void Parser::peek() { m_current = m_lexer.Peek(); }

void Parser::skipWhitespace() {
    while (m_lexer.Peek().type == TokenType::NewLine) {
        m_lexer.Next();
    }
}

/**
 * Keeps consuming tokens until it finds one of the desired token types, and
 *  consumes that one as well.
 */
void Parser::advance(const std::vector<TokenType>& types) {
    Token next = m_lexer.Next();
    while (next.type != TokenType::Eof && !util::Contains(types, next.type)) {
        next = m_lexer.Next();
    }
}

/**
 * An overload of `advance` that allows passing just one type for ease of use.
 * Wraps that token type in a vector.
 */
void Parser::advance(const TokenType& type) {
    return advance(std::vector{type});
}

void Parser::error(const std::string& msg) {
    m_errors.push_back(ast::Error{
        .pos = m_current.pos,
        .message = msg,
    });
}

void Parser::error(const lexer::Token& tok, const std::string& msg) {
    m_errors.push_back(ast::Error{
        .pos = tok.pos,
        .message = msg,
    });
}

std::vector<ast::Error> Parser::getErrors() { return m_errors; }

} // namespace parser
