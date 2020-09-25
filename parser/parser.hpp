#pragma once

#include "ast.hpp"
#include "lexer.hpp"
#include "token.hpp"

namespace parser {

template <typename T> using sPtr = std::shared_ptr<T>;

// TODO: define function to use this struct, that also advances
struct Error {
    lexer::Token::Position pos;
    std::string message;
};

// TODO: define a type for the "context"

class Parser {
public:
    Parser(lexer::Lexer lexer) : m_lexer(lexer) {}
    sPtr<ast::Program> parseProgram();
    sPtr<ast::Routine> parseRoutine();
    sPtr<ast::Parameter> parseParameter();
    sPtr<ast::Type> parseType();
    sPtr<ast::PrimitiveType> parsePrimitiveType();
    sPtr<ast::ArrayType> parseArrayType();
    sPtr<ast::RecordType> parseRecordType();
    sPtr<ast::Variable> parseVariable();
    sPtr<ast::Body> parseBody();
    sPtr<ast::Statement> parseStatement();
    sPtr<ast::Assignment> parseAssignment();
    sPtr<ast::WhileLoop> parseWhileLoop();
    sPtr<ast::ForLoop> parseForLoop();
    sPtr<ast::IfStatement> parseIfStatement();
    sPtr<ast::Expression> parseExpression();
    sPtr<ast::Expression> parseUnaryExpression();
    sPtr<ast::Expression> parseBinaryExpression(int prec1 = 0);
    sPtr<ast::RoutineCall> parseRoutineCall();
    sPtr<ast::ModifiablePrimary> parseModifiablePrimary(lexer::Token);
    std::vector<Error> getErrors();

private:
    lexer::Lexer m_lexer;
    std::vector<Error> m_errors;
    lexer::Token skipWhile(std::function<bool(lexer::Token)>);
    static bool isNewLine(lexer::Token);
    bool isPrimitive(lexer::TokenType);
    int opPrec(lexer::TokenType);
};

} // namespace parser
