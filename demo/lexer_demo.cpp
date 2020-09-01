#include <iostream>
#include "fmt/core.h"
#include "fmt/color.h"
#include "strings.hpp"
#include "lexer.hpp"
#include "token.hpp"
#include "strings.hpp"

std::vector<std::string> g_TokTypeStr{
    "Eof",
    "Error",
    "OneLineComment",
    "VarDecl",
    "TypeDecl",
    "RoutineDecl",
    "Is",
    "IntegerType",
    "RealType",
    "BooleanType",
    "RecordType",
    "ArrayType",
    "True",
    "False",
    "WhileLoop",
    "ForLoop",
    "LoopBegin",
    "End",
    "ReverseRange",
    "InRange",
    "If",
    "Then",
    "Else",
    "AndLogic",
    "OrLogic",
    "XorLogic",
    "LessComp",
    "LeqComp",
    "GreaterComp",
    "GeqComp",
    "EqComp",
    "NeqComp",
    "AssignmentOp",
    "MultOp",
    "DivOp",
    "ModOp",
    "AddOp",
    "SubOp",
    "IntegerLiteral",
    "RealLiteral",
    "Identifier",
    "Dot",
    "TwoDots",
    "Comma",
    "ParenOpen",
    "ParenClose",
    "BracketOpen",
    "BracketClose",
    "Colon",
    "Semicolon",
    "NewLine",
};

std::string to_string(lexer::TokenType type) {
    return g_TokTypeStr[static_cast<int>(type)];
}

void printTokens(const std::vector<lexer::Token>& toks) {
    for (auto& tok : toks) {
        auto str = common::replaceAll(tok.image, "\n", "\\n");
        fmt::print("{} ({}) ", str, to_string(tok.type));
    }
    fmt::print("\n\n");
}

void outputLineDiagnostics(std::string line, size_t begin, size_t end) {
    fmt::print("\t{}", line.substr(0, begin));
    fmt::print(fg(fmt::color::crimson), "{}", line.substr(begin, end - begin));
    fmt::print("{}\n", line.substr(end));
    fmt::print("\t{}{}\n", std::string(begin, ' '), '^');
}

std::vector<lexer::Token> extractTokens(lexer::Lexer& lx) {
    std::vector<lexer::Token> tokens;
    for (auto tok = lx.Next(); tok.type != lexer::TokenType::Eof && tok.type != lexer::TokenType::Error; tok = lx.Next()) {
        tokens.push_back(tok);
    }

    return tokens;
}

int main() {
    for (;;) {
        fmt::print("riddle> ");
        std::string line;
        std::getline(std::cin, line);

        lexer::Lexer lx{line};
        auto tokens = extractTokens(lx);

        if (auto tok = lx.Next(); tok.type == lexer::TokenType::Error) {
            fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "error: ");
            fmt::print("could not tokenize further.\n");
            outputLineDiagnostics(line, tok.srcPos, line.size());
            continue;
        }

        printTokens(tokens);
    }

    return 0;
}
