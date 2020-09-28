#include <fstream>
#include <iostream>
#include <sstream>

#include "fmt/color.h"
#include "fmt/core.h"
#include "lexer.hpp"
#include "parser.hpp"

class PrintVisitor : public ast::Visitor {
public:
    PrintVisitor(size_t depth = 0) : depth(depth) {}
    void visit(ast::Program* node) override {
        if (node == nullptr) {
            fmt::print("{:|>{}}- [Program]> null\n", "", depth);
            return;
        }
        fmt::print("{:|>{}}- [Program]>\n", "", depth);
        depth++;
        for (auto type : node->types) {
            type->accept(*this);
        }
        for (auto routine : node->routines) {
            routine->accept(*this);
        }
        for (auto variableDecl : node->variables) {
            variableDecl->accept(*this);
        }
        depth--;
    }
    void visit(ast::RoutineDecl* node) override {
        if (node == nullptr) {
            fmt::print("{:|>{}}- [RoutineDecl]> null\n", "", depth);
            return;
        }
        fmt::print("{:|>{}}- [RoutineDecl]> {}\n", "", depth, node->name.lit);
        depth++;
        for (auto parameter : node->parameters) {
            parameter->accept(*this);
        }
        node->body->accept(*this);
        if (node->returnType != nullptr) {
            node->returnType->accept(*this);
        }
        depth--;
    }
    void visit(ast::Parameter* node) override {
        if (node == nullptr) {
            fmt::print("{:|>{}}- [Parameter]> null\n", "", depth);
            return;
        }
        fmt::print("{:|>{}}- [Parameter]> {}\n", "", depth, node->name.lit);
        depth++;
        node->type->accept(*this);
        depth--;
    }
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
    }
    void visit(ast::PrimitiveType* node) override {
        if (node == nullptr) {
            fmt::print("{:|>{}}- [PrimitiveType]> null\n", "", depth);
            return;
        }
        fmt::print("{:|>{}}- [PrimitiveType]> {}\n", "", depth, node->type.lit);
    }
    void visit(ast::ArrayType* node) override {
        if (node == nullptr) {
            fmt::print("{:|>{}}- [ArrayType]> null\n", "", depth);
            return;
        }
        fmt::print("{:|>{}}- [ArrayType]>\n", "", depth);
        depth++;
        if (node->length != nullptr) {
            node->length->accept(*this);
        }
        if (node->elementType != nullptr) {
            node->elementType->accept(*this);
        }
        depth--;
    }
    void visit(ast::RecordType* node) override {
        if (node == nullptr) {
            fmt::print("{:|>{}}- [RecordType]> null\n", "", depth);
            return;
        }
        fmt::print("{:|>{}}- [RecordType]>\n", "", depth);
        depth++;
        for (auto field : node->fields) {
            field->accept(*this);
        }
        depth--;
    }
    void visit(ast::VariableDecl* node) override {
        if (node == nullptr) {
            fmt::print("{:|>{}}- [VariableDecl]> null\n", "", depth);
        }
        fmt::print("{:|>{}}- [VariableDecl]> {}\n", "", depth, node->name.lit);
        depth++;
        if (node->type != nullptr) {
            node->type->accept(*this);
        }
        if (node->expression != nullptr) {
            node->expression->accept(*this);
        }
        depth--;
    }
    void visit(ast::TypeDecl* node) override {
        if (node == nullptr) {
            fmt::print("{:|>{}}- [TypeDecl]> null\n", "", depth);
        }
        fmt::print("{:|>{}}- [TypeDecl]> {}\n", "", depth, node->name);
        depth++;
        if (node->type != nullptr) {
            node->type->accept(*this);
        }
        depth--;
    }
    void visit(ast::Body* node) override {
        if (node == nullptr) {
            fmt::print("{:|>{}}- [Body]> null\n", "", depth);
            return;
        }
        fmt::print("{:|>{}}- [Body]>\n", "", depth);
        depth++;
        for (auto type : node->types) {
            type->accept(*this);
        }
        for (auto variableDecl : node->variables) {
            variableDecl->accept(*this);
        }
        for (auto statement : node->statements) {
            statement->accept(*this);
        }
        depth--;
    }
    void visit(ast::Statement* node) override {
        if (node == nullptr) {
            fmt::print("{:|>{}}- [Statement]> null\n", "", depth);
            return;
        }
        fmt::print("{:|>{}}- [Statement]> Unknown\n", "", depth);
    }
    void visit(ast::ReturnStatement* node) override {
        if (node == nullptr) {
            fmt::print("{:|>{}}- [Return]> null\n", "", depth);
            return;
        }
        fmt::print("{:|>{}}- [Return]>\n", "", depth);
        depth++;
        if (node->expression != nullptr) {
            node->expression->accept(*this);
        }
        depth--;
    }
    void visit(ast::Assignment* node) override {
        if (node == nullptr) {
            fmt::print("{:|>{}}- [Assignment]> null\n", "", depth);
            return;
        }
        fmt::print("{:|>{}}- [Assignment]>\n", "", depth);
        depth++;
        node->lhs->accept(*this);
        node->rhs->accept(*this);
        depth--;
    }
    void visit(ast::WhileLoop* node) override {
        if (node == nullptr) {
            fmt::print("{:|>{}}- [WhileLoop]> null\n", "", depth);
            return;
        }
        fmt::print("{:|>{}}- [WhileLoop]>\n", "", depth);
        depth++;
        node->condition->accept(*this);
        node->body->accept(*this);
        depth--;
    }
    void visit(ast::ForLoop* node) override {
        if (node == nullptr) {
            fmt::print("{:|>{}}- [ForLoop]> null\n", "", depth);
            return;
        }
        fmt::print("{:|>{}}- [ForLoop]>\n", "", depth);
        depth++;
        // node->loopVar->accept(*this);
        node->rangeFrom->accept(*this);
        node->rangeTo->accept(*this);
        node->body->accept(*this);
        depth--;
    }
    void visit(ast::IfStatement* node) override {
        if (node == nullptr) {
            fmt::print("{:|>{}}- [IfStatement]> null\n", "", depth);
            return;
        }
        fmt::print("{:|>{}}- [IfStatement]>\n", "", depth);
        depth++;
        node->condition->accept(*this);
        node->ifBody->accept(*this);
        if (node->elseBody) {
            node->elseBody->accept(*this);
        }
        depth--;
    }
    void visit(ast::Expression* node) override {
        if (node == nullptr) {
            fmt::print("{:|>{}}- [Expression]> null\n", "", depth);
            return;
        }
        fmt::print("{:|>{}}- [Expression]>\n", "", depth);
    }
    void visit(ast::UnaryExpression* node) override {
        if (node == nullptr) {
            fmt::print("{:|>{}}- [UnaryExpression]> null\n", "", depth);
            return;
        }
        fmt::print("{:|>{}}- [UnaryExpression]> {}\n", "", depth,
                   node->operation.lit);
        depth++;
        node->operand->accept(*this);
        depth--;
    }
    void visit(ast::BinaryExpression* node) override {
        if (node == nullptr) {
            fmt::print("{:|>{}}- [BinaryExpression]> null\n", "", depth);
            return;
        }
        fmt::print("{:|>{}}- [BinaryExpression]> {}\n", "", depth,
                   node->operation.lit);
        depth++;
        node->operand1->accept(*this);
        node->operand2->accept(*this);
        depth--;
    }
    void visit(ast::Primary* node) override {
        if (node == nullptr) {
            fmt::print("{:|>{}}- [Primary]> null\n", "", depth);
            return;
        }
        fmt::print("{:|>{}}- [Primary]> {}\n", "", depth, node->value.lit);
    }
    void visit(ast::RoutineCall* node) override {
        if (node == nullptr) {
            fmt::print("{:|>{}}- [RoutineCall]> null\n", "", depth);
            return;
        }
        fmt::print("{:|>{}}- [RoutineCall]> {}\n", "", depth,
                   node->routine.lit);
        depth++;
        for (size_t i = 0; i < node->args.size(); i++) {
            node->args[i]->accept(*this);
        }
        depth--;
    }

private:
    size_t depth;
};

int main(int argc, char* argv[]) {
    if (argc == 2) {
        std::ifstream f(argv[1]);
        std::string code((std::istreambuf_iterator<char>(f)),
                         (std::istreambuf_iterator<char>()));
        fmt::print("Code:\n");
        fmt::print(fg(fmt::color::aqua), "{}\n\n", code);
        lexer::Lexer lx{code};
        parser::Parser parser(lx);

        auto ast = parser.parseProgram();
        auto errors = parser.getErrors();
        if (errors.empty()) {
            PrintVisitor v;
            ast->accept(v);
        } else {
            // get individual lines
            std::vector<std::string> lines;
            std::string line;
            std::istringstream codeStream(code);
            while (std::getline(codeStream, line, '\n')) {
                lines.push_back(line);
            }

            fmt::print(fg(fmt::color::indian_red) | fmt::emphasis::bold,
                       "Parsing Errors:\n");
            for (auto error : errors) {
                fmt::print("*\t{}\n", lines[error.pos.line - 1]);
                fmt::print("\t{:->{}}^{:-<{}}\n", "", error.pos.column - 1, "",
                           lines[error.pos.line - 1].length() -
                               error.pos.column - 1);
                fmt::print(fg(fmt::color::indian_red),
                           "\t[line: {}, column: {}]: {}\n\n", error.pos.line,
                           error.pos.column, error.message);
            }
            return 1;
        }
        return 0;
    }

    for (;;) {
        int choice;
        typedef parser::sPtr<ast::Node> (parser::Parser::*ParserFunc)();
        ParserFunc parseFunc;
        fmt::print("What do you want to parse?\n");
        fmt::print("(0) Exit\n");
        fmt::print("(1) routine\n");
        fmt::print("(2) expression\n");
        fmt::print("(3) type declaration\n");
        fmt::print("(4) statement\n");
        fmt::print("Enter a number> ");
        std::cin >> choice;
        switch (choice) {
        case 0:
            return 0;
        case 1:
            parseFunc = (ParserFunc)(&parser::Parser::parseRoutineDecl);
            break;
        case 2:
            parseFunc = (ParserFunc)(&parser::Parser::parseExpression);
            break;
        case 3:
            parseFunc = (ParserFunc)(&parser::Parser::parseTypeDecl);
            break;
        case 4:
            parseFunc = (ParserFunc)(&parser::Parser::parseStatement);
            break;
        default:
            fmt::print("Invalid option\n");
            return 1;
        }

        fmt::print("riddle> ");
        std::string line;
        std::getline(std::cin >> std::ws, line);

        lexer::Lexer lx{line};
        parser::Parser parser(lx);

        auto ast = (parser.*parseFunc)();
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
        fmt::print("\n");
    }

    return 0;
}
