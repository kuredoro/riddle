#include <fstream>
#include <iostream>

#include "fmt/color.h"
#include "fmt/core.h"
#include "lexer.hpp"
#include "parser.hpp"

class PrintVisitor : public ast::Visitor {
public:
    PrintVisitor(size_t depth = 0) : depth(depth) {}
    void visit(ast::Program* node) override {}
    void visit(ast::Routine* node) override{

    };
    void visit(ast::Parameter* node) override {
        if (node == nullptr) {
            fmt::print("{:|>{}}- [Parameter]> null\n", "", depth);
        } else {
            fmt::print("{:|>{}}- [Parameter]> {}\n", "", depth, node->name.lit);
            depth++;
            node->type->accept(*this);
            depth--;
        }
    };
    void visit(ast::Type* node) override {
        if (ast::PrimitiveType* specific =
                dynamic_cast<ast::PrimitiveType*>(node);
            specific != nullptr) {
            specific->accept(*this);
        } else if (ast::ArrayType* specific =
                       dynamic_cast<ast::ArrayType*>(node);
                   specific != nullptr) {
            specific->accept(*this);
        } else if (ast::RecordType* specific =
                       dynamic_cast<ast::RecordType*>(node);
                   specific != nullptr) {
            specific->accept(*this);
        } else if (ast::AliasedType* specific =
                       dynamic_cast<ast::AliasedType*>(node);
                   specific != nullptr) {
            fmt::print("{:|>{}}- [Type Identifier]> {}\n", "", depth,
                       specific->name.lit);
        } else {
            fmt::print("{:|>{}}- [Type]> (unknown type)\n", "", depth);
        }
    };
    void visit(ast::PrimitiveType* node) override {
        if (node == nullptr) {
            fmt::print("{:|>{}}- [PrimitiveType]> null\n", "", depth);
        } else {
            fmt::print("{:|>{}}- [PrimitiveType]> {}\n", "", depth,
                       node->type.lit);
        }
    };
    void visit(ast::ArrayType* node) override {
        if (node == nullptr) {
            fmt::print("{:|>{}}- [ArrayType]> null\n", "", depth);
        } else {
            fmt::print("{:|>{}}- [ArrayType]>\n", "", depth);
            depth++;
            node->length->accept(*this);
            node->elementType->accept(*this);
            depth--;
        }
    };
    void visit(ast::RecordType* node) override {
        if (node == nullptr) {
            fmt::print("{:|>{}}- [RecordType]> null\n", "", depth);
        } else {
            fmt::print("{:|>{}}- [RecordType]>\n", "", depth);
            depth++;
            for (auto field : node->fields) {
                field->accept(*this);
            }
            depth--;
        }
    };
    void visit(ast::Variable* node) override{

    };
    void visit(ast::Body* node) override{

    };
    void visit(ast::Statement* node) override{

    };
    void visit(ast::Assignment* node) override{

    };
    // void visit(ast::RoutineCall* node) override{

    // };
    void visit(ast::WhileLoop* node) override {
        if (node == nullptr) {
            fmt::print("{:|>{}}- [WhileLoop]> null\n", "", depth);
        } else {
            fmt::print("{:|>{}}- [WhileLoop]>\n", "", depth);
            depth++;
            node->condition->accept(*this);
            node->body->accept(*this);
            depth--;
        }
    };
    void visit(ast::ForLoop* node) override {
        if (node == nullptr) {
            fmt::print("{:|>{}}- [ForLoop]> null\n", "", depth);
        } else {
            fmt::print("{:|>{}}- [ForLoop]>\n", "", depth);
            depth++;
            // node->loopVar->accept(*this);
            node->rangeFrom->accept(*this);
            node->rangeTo->accept(*this);
            node->body->accept(*this);
            depth--;
        }
    };
    void visit(ast::IfStatement* node) override {
        if (node == nullptr) {
            fmt::print("{:|>{}}- [IfStatement]> null\n", "", depth);
        } else {
            fmt::print("{:|>{}}- [IfStatement]>\n", "", depth);
            depth++;
            node->condition->accept(*this);
            node->ifBody->accept(*this);
            if (node->elseBody) {
                node->elseBody->accept(*this);
            }
            depth--;
        }
    };
    void visit(ast::Expression* node) override {
        if (node == nullptr) {
            fmt::print("{:|>{}}- [Expression]> null\n", "", depth);
        } else {
            // if binary
            if (node->operand2 != nullptr) {

                fmt::print("{:|>{}}- [BinaryOperator]> {}\n", "", depth,
                           node->operation.lit);
                depth++;
                node->operand1->accept(*this);
                node->operand2->accept(*this);
                depth--;
            } else {

                fmt::print("{:|>{}}- [UnaryOperator]> {}\n", "", depth,
                           node->operation.lit);
                depth++;
                node->operand1->accept(*this);
                depth--;
            }
        }
    };
    void visit(ast::Primitive* node) override {
        if (node == nullptr) {
            fmt::print("{:|>{}}- [Primitive]> null\n", "", depth);
        } else {
            fmt::print("{:|>{}}- [Primitive]> {}\n", "", depth,
                       node->value.lit);
        }
    };
    void visit(ast::ModifiablePrimary* node) override {
        if (node == nullptr) {
            fmt::print("{:|>{}}- [ModifiablePrimary]> null\n", "", depth);
        } else {
            fmt::print("{:|>{}}- [ModifiablePrimary]> size {} \n", "", depth,
                       node->args.size());
            depth++;
            for (int i = 0; i < node->args.size(); i++) {
                node->args[i]->accept(*this);
            }
            depth--;
        }
    };
    void visit(ast::RoutineCall* node) override {
        if (node == nullptr) {
            fmt::print("{:|>{}}- [RoutineCall]> null\n", "", depth);
        } else {
            fmt::print("{:|>{}}- [RoutineCall]> {}\n", "", depth,
                       node->routine.lit);
            depth++;
            for (int i = 0; i < node->args.size(); i++) {
                node->args[i]->accept(*this);
            }
            depth--;
        }
    };

private:
    size_t depth;
};

int main(int argc, char* argv[]) {
    if (argc == 2) {
        std::ifstream f(argv[1]);
        std::string code((std::istreambuf_iterator<char>(f)),
                         (std::istreambuf_iterator<char>()));
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

        auto ast = parser.parseExpression();
        auto errors = parser.getErrors();
        if (errors.empty()) {
            PrintVisitor v;
            ast->accept(v);
        } else {
            fmt::print(fg(fmt::color::indian_red) | fmt::emphasis::bold,
                       "Errors:\n");
            for (auto error : errors) {
                fmt::print(fg(fmt::color::indian_red),
                           "\t[character: {}]: {}\n", error.pos.column,
                           error.message);
            }
        }
    }

    return 0;
}
