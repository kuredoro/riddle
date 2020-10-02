#pragma once

#include "ast.hpp"
#include "lexer.hpp"
#include "token.hpp"

namespace parser {

template <typename T> using sPtr = std::shared_ptr<T>;

struct Error {
    lexer::Token::Position pos;
    std::string message;
};

class Parser {
public:
    Parser(lexer::Lexer lexer) : m_lexer(lexer) {}
    sPtr<ast::Program> parseProgram();
    sPtr<ast::RoutineDecl> parseRoutineDecl();
    sPtr<ast::Parameter> parseParameter();
    sPtr<ast::TypeDecl> parseTypeDecl();
    sPtr<ast::Type> parseType();
    sPtr<ast::PrimitiveType> parsePrimitiveType();
    sPtr<ast::ArrayType> parseArrayType();
    sPtr<ast::RecordType> parseRecordType();
    sPtr<ast::VariableDecl> parseVariableDecl();
    sPtr<ast::Body> parseBody();
    sPtr<ast::Statement> parseStatement();
    sPtr<ast::Assignment> parseAssignment(sPtr<ast::Expression> left);
    sPtr<ast::WhileLoop> parseWhileLoop();
    sPtr<ast::ForLoop> parseForLoop();
    sPtr<ast::IfStatement> parseIfStatement();
    sPtr<ast::ReturnStatement> parseReturnStatement();
    sPtr<ast::Expression> parseExpression();
    sPtr<ast::Expression> parseUnaryExpression();
    sPtr<ast::Expression> parseBinaryExpression(int prec1 = 0);
    sPtr<ast::RoutineCall> parseRoutineCall(lexer::Token);
    std::vector<Error> getErrors();

private:
    lexer::Lexer m_lexer;
    lexer::Token m_current;

    std::vector<Error> m_errors;


    void expect(const std::vector<lexer::TokenType>&, std::string = "");
    void expect(const lexer::TokenType&, std::string = "");
    void next();
    void peek();

    void skipWhitespace();
    void advance(const std::vector<lexer::TokenType>&);
    void advance(const lexer::TokenType&);
    void error(const std::string& msg);
    void error(const lexer::Token& pos, const std::string& msg);
    bool isPrimary(const lexer::TokenType&);
    int opPrec(const lexer::TokenType&);
};

} // namespace parser
