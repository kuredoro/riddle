#include <fstream>
#include <iostream>
#include "lexer.hpp"
#include "parser.hpp"

class PrintVisitor : public ast::Visitor {
    void visit (ast::Program* node) override {

    }
    void visit(ast::Routine *node) override {

    };
    void visit(ast::Parameter *node) override {

    };
    void visit(ast::Type *node) override {

    };
    void visit(ast::PrimitiveType *node) override {

    };
    void visit(ast::ArrayType *node) override {

    };
    void visit(ast::RecordType *node) override {

    };
    void visit(ast::Variable *node) override {

    };
    void visit(ast::Body *node) override {

    };
    void visit(ast::Statement *node) override {

    };
    void visit(ast::Assignment *node) override {

    };
    void visit(ast::RoutineCall *node) override {

    };
    void visit(ast::WhileLoop *node) override {

    };
    void visit(ast::ForLoop *node) override {

    };
    void visit(ast::IfStatement *node) override {

    };
    void visit(ast::Expression *node) override {

    };
    void visit(ast::UnaryExpression *node) override {

    };
    void visit(ast::BinaryExpression *node) override {

    };
};

int main (int argc, char *argv[]) {
	if (argc == 2) {
        std::ifstream f(argv[1]);
        std::string code((std::istreambuf_iterator<char>(f)),
                         (std::istreambuf_iterator<char>() ));
        fmt::print("Code:\n{}\n\n", code);
        lexer::Lexer lx{code};
		parser::Parser parser(lx);
        auto ast = parser.parseProgram();
        PrintVisitor v;
        ast->accept(v);

        return 0;
    }

	return 0;
}
