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

ssPtr<ast::Expression> Parser::parseExpression() {
    // TODO: Concrete implementation
    // Expression : Relation { ( and | or | xor ) Relation }
    /* Relation : Simple [ ( < | <= | > | >= | = | /= ) Simple ]
        Simple : Factor { ( + | - ) Factor }
        Factor : Summand { ( * | / | % ) Summand }
        Summand : Primary | ( Expression )
        */
    // Token currentToken = m_lexer.Next();
    int p;
    ast::Expression expression = parseBinaryExpression(-1);
    while ((p = getPriority(m_lexer.Peek().type)) >=
           0) { // TODO move to the parse Binary
        expression.operand1 = expression;
        expression.operation = m_lexer.Next();
        expression.operand2 = parseBinaryExpression(p);
        /* code */
    }

    // x = parseBinaryExpression();
    // while Peek next token getPriority() >=0 :
    //  new bin Expr:= x += parseBinaryExpression();
    //
}

sPtr<ast::Expression> Parser::parseBinaryExpression(int priority) {
    ast::Expression expression;
    expression.operand1 =
        parseUnaryExpression(priority); // do I need priority for unary?
    int p;
    TokenType opType = m_lexer.Peek().type;
    if ((p = getPriority(opType)>=0) {
        if (p < priority) {
            return expression.operand1;
        }
        expression.operator= m_lexer.Next();
        expression.operand2 = parseBinaryExpression(p);
    }
    return expression;
}

sPtr<ast::Expression> Parser::parseUnaryExpression(int priority) {
    TokenType type = m_lexer.Peek().type;
    switch (type) {
        // primitives
    case TokenType::Identifier:
    case TokenType::Int:
    case TokenType::Real:
    case TokenType::True:
    case TokenType::False:
        // return type.Next()???
        break;
    case TokenType::OpenParen:
        ast::Expression expr = parseBinaryExpression(priority + 1);
        if (m_lexer.Peek().type == TokenType::CloseParen) {
            return expr;
        }
        // error
        return nullptr
    }
    // else if (opType = TokenType::OpenParen){
    //     // check braces

    // }
    return nullptr
}

int getPriority(TokenType token) {
    switch (token) {
    case TokenType::And:
    case TokenType::Or:
    case TokenType::Xor:
        return 0;
    case TokenType::Less:
    case TokenType::Greater:
    case TokenType::Eq:
    case TokenType::Leq:
    case TokenType::Geq:
    case TokenType::Neq:
        return 1;
    case TokenType::Add:
    case TokenType::Sub:
        return 2;
    case TokenType::Mul:
    case TokenType::Div:
    case TokenType::Mod:
        return 3;
    case TokenType::Not:
        return 4;
    }
    // throw error
    return -1;
}

// ---- End separation

bool Parser::isNewLine(Token tok) { return tok.type == TokenType::NewLine; }

/**
 * Skips all tokens that match the given predicate, returning the first token
 * that doesn't
 */
Token Parser::skipWhile(std::function<bool(Token)> pred) {
    while (pred(m_lexer.Peek())) {
        m_lexer.Next();
    }
    return m_lexer.Next();
}

std::vector<parser::Error> Parser::getErrors() { return m_errors; }

} // namespace parser
