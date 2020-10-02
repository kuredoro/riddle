#include "fmt/color.h"
#include "fmt/core.h"
#include "lexer.hpp"
#include "strings.hpp"
#include "token.hpp"
#include <fstream>
#include <iostream>
#include <iterator>


void printTokens(const std::vector<lexer::Token>& toks) {
    for (auto& tok : toks) {
        auto str = common::replaceAll(tok.lit, "\n", "\\n");
        fmt::print("{} ({}) ", str, lexer::to_string(tok.type));
    }
    fmt::print("\n\n");
}

void outputLineDiagnostics(std::string line, size_t begin, size_t end) {
    begin--;
    fmt::print("\t{}", line.substr(0, begin));
    fmt::print(fg(fmt::color::indian_red), "{}",
               line.substr(begin, end - begin));
    fmt::print("{}\n", line.substr(end));
    fmt::print("\t{}{}\n", std::string(begin, ' '), '^');
}

std::vector<lexer::Token> extractTokens(lexer::Lexer& lx) {
    std::vector<lexer::Token> tokens;
    auto tok = lx.Next();
    for (; tok.type != lexer::TokenType::Eof &&
           tok.type != lexer::TokenType::Illegal;
         tok = lx.Next()) {
        tokens.push_back(tok);
    }
    tokens.push_back(tok); // to keep the Eof|Illegal token

    return tokens;
}

int main(int argc, char* argv[]) {
    if (argc == 2) {
        std::ifstream f(argv[1]);
        std::string code((std::istreambuf_iterator<char>(f)),
                         (std::istreambuf_iterator<char>()));
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

        if (auto tok = tokens.back(); tok.type == lexer::TokenType::Illegal) {
            fmt::print(fg(fmt::color::indian_red) | fmt::emphasis::bold,
                       "error: ");
            fmt::print("could not tokenize further.\n");
            outputLineDiagnostics(line, tok.pos.column, line.size());
            continue;
        }

        printTokens(tokens);
    }

    return 0;
}
