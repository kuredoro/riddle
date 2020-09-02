#include <fstream>
#include <iostream>
#include <iterator>
#include "fmt/core.h"
#include "fmt/color.h"
#include "strings.hpp"
#include "lexer.hpp"
#include "token.hpp"
#include "strings.hpp"

std::vector<std::string> g_TokTypeStr{
    "Illegal",
    "Eof",
    "Comment",
    "Ident",
    "Int",
    "Real",
    "Less",
    "Greater",
    "Eq",
    "Leq",
    "Geq",
    "Neq",
    "Assign",
    "Add",
    "Sub",
    "Mul",
    "Div",
    "Mod",
    "OParen",
    "OBrack",
    "Comma",
    "Dot",
    "TwoDots",
    "CParen",
    "CBrack",
    "Semicolon",
    "Colon",
    "NewLine",
    "Var",
    "Type",
    "Routine",
    "Is",
    "IntegerType",
    "RealType",
    "Boolean",
    "Record",
    "Array",
    "True",
    "False",
    "While",
    "For",
    "Loop",
    "End",
    "Reverse,",
    "In,",
    "If",
    "Then,",
    "Else",
    "And",
    "Or",
    "Xor",
};

std::string to_string(lexer::TokenType type) {
    return g_TokTypeStr[static_cast<int>(type)];
}

void printTokens(const std::vector<lexer::Token>& toks) {
    for (auto& tok : toks) {
        auto str = common::replaceAll(tok.lit, "\n", "\\n");
        fmt::print("{} ({}) ", str, to_string(tok.type));
    }
    fmt::print("\n\n");
}

void outputLineDiagnostics(std::string line, size_t begin, size_t end) {
    begin--;
    fmt::print("\t{}", line.substr(0, begin));
    fmt::print(fg(fmt::color::indian_red), "{}", line.substr(begin, end - begin));
    fmt::print("{}\n", line.substr(end));
    fmt::print("\t{}{}\n", std::string(begin, ' '), '^');
}

std::vector<lexer::Token> extractTokens(lexer::Lexer& lx) {
    std::vector<lexer::Token> tokens;
    for (auto tok = lx.Next(); tok.type != lexer::TokenType::Eof && tok.type != lexer::TokenType::Illegal; tok = lx.Next()) {
        tokens.push_back(tok);
    }

    return tokens;
}

int main(int argc, char* argv[]) {
    if (argc == 2) {
        std::ifstream f(argv[1]);
        std::string code((std::istreambuf_iterator<char>(f)),
                         (std::istreambuf_iterator<char>() ));
        fmt::print("Code:\n{}\n\n", code);
        lexer::Lexer lx{code};
        auto tokens = extractTokens(lx);
        printTokens(tokens);
        return 0;
    }

    for (;;) {
        fmt::print("riddle> ");
        std::string line;
        std::getline(std::cin, line);

        lexer::Lexer lx{line};
        auto tokens = extractTokens(lx);

        if (auto tok = lx.Next(); tok.type == lexer::TokenType::Illegal) {
            fmt::print(fg(fmt::color::indian_red) | fmt::emphasis::bold, "error: ");
            fmt::print("could not tokenize further.\n");
            outputLineDiagnostics(line, tok.column, line.size());
            continue;
        }

        printTokens(tokens);
    }

    return 0;
}
