#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>

#include "fmt/color.h"
#include "fmt/core.h"
#include "lexer.hpp"
#include "parser.hpp"
#include "visitors.hpp"

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
        std::function<std::shared_ptr<ast::Node>(parser::Parser&)> parseFunc;
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
            parseFunc = [](parser::Parser& p) { return p.parseRoutineDecl(); };
            break;
        case 2:
            parseFunc = [](parser::Parser& p) { return p.parseExpression(); };
            break;
        case 3:
            parseFunc = [](parser::Parser& p) { return p.parseTypeDecl(); };
            break;
        case 4:
            parseFunc = [](parser::Parser& p) { return p.parseStatement(); };
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

        auto ast = parseFunc(parser);
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
