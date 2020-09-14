#include <fstream>
#include <iostream>
#include "fmt/core.h"
#include "fmt/color.h"
#include "lexer.hpp"
#include "parser.hpp"

class PrintVisitor : public ast::Visitor {
public:
    PrintVisitor(size_t depth = 0) : depth(depth) {}
    void visit (ast::Program* node) override {

    }
    void visit(ast::Routine *node) override {

    };
    void visit(ast::Parameter *node) override {
        if (node == nullptr) {
            fmt::print("{:|>{}}- [Parameter]> null\n", "", depth);
        } else {
            fmt::print("{:|>{}}- [Parameter]> {}\n", "", depth, node->name.lit);
            depth++;
            visit(node->type.get());
            depth--;
        }
    };
    void visit(ast::Type *node) override {
        if (ast::PrimitiveType *specific = dynamic_cast<ast::PrimitiveType*>(node); specific != nullptr) {
            visit(specific);
        } else if (ast::ArrayType *specific = dynamic_cast<ast::ArrayType*>(node); specific != nullptr) {
            visit(specific);
        } else if (ast::RecordType *specific = dynamic_cast<ast::RecordType*>(node); specific != nullptr) {
            visit(specific);
        } else if (ast::AliasedType *specific = dynamic_cast<ast::AliasedType*>(node); specific != nullptr) {
            fmt::print("{:|>{}}- [Type Identifier]> {}\n", "", depth, specific->name.lit);
        } else {
            fmt::print("{:|>{}}- [Type]> (unknown type)\n", "", depth);
        }
    };
    void visit(ast::PrimitiveType *node) override {
        if (node == nullptr) {
            fmt::print("{:|>{}}- [PrimitiveType]> null\n", "", depth);
        } else {
            fmt::print("{:|>{}}- [PrimitiveType]> {}\n", "", depth, node->type.lit);
        }
    };
    void visit(ast::ArrayType *node) override {
        if (node == nullptr) {
            fmt::print("{:|>{}}- [ArrayType]> null\n", "", depth);
        } else {
            fmt::print("{:|>{}}- [ArrayType]>\n", "", depth);
            depth++;
            visit(node->length.get());
            visit(node->elementType.get());
            depth--;
        }
    };
    void visit(ast::RecordType *node) override {
        if (node == nullptr) {
            fmt::print("{:|>{}}- [RecordType]> null\n", "", depth);
        } else {
            fmt::print("{:|>{}}- [RecordType]>\n", "", depth);
            depth++;
            for (auto field : node->fields) {
                visit(field.get());
            }
            depth--;
        }
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
private:
    size_t depth;
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

    for (;;) {
        fmt::print("riddle> ");
        std::string line;
        std::getline(std::cin, line);

        lexer::Lexer lx{line};
        parser::Parser parser(lx);

        auto ast = parser.parseParameter();
        auto errors = parser.getErrors();
        if (errors.empty()) {
            PrintVisitor v;
            ast->accept(v);
        } else {
            fmt::print(fg(fmt::color::indian_red) | fmt::emphasis::bold, "Errors:\n");
            for (auto error : errors) {
                fmt::print(fg(fmt::color::indian_red), "\t[character: {}]: {}\n", error.pos.column, error.message);
            }
        }
    }

	return 0;
}
