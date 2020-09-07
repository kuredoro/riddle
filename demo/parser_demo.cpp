#include <fstream>
#include <iostream>
#include "lexer.hpp"
#include "parser.hpp"


int main (int argc, char *argv[]) {
	if (argc == 2) {
        std::ifstream f(argv[1]);
        std::string code((std::istreambuf_iterator<char>(f)),
                         (std::istreambuf_iterator<char>() ));
        fmt::print("Code:\n{}\n\n", code);
        lexer::Lexer lx{code};
		AST::Parser parser(lx);
        auto ast = parser.parseProgram();

		// TODO: print the tree beautifully

        return 0;
    }

	return 0;
}
