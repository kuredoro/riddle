#include <iostream>
#include "lexer.hpp"
#include "token.hpp"

int main()
{
    std::cout << "Type\t\tPosition\t\tSrc\n";
    std::vector<Token> tokens = read("./examples/ex1.rdd");
    for (Token val : tokens)
    {
        std::cout << int(val.type) << "\t\t" << val.srcPos << "\t\t" << val.image << "\n";
    }
    return 0;
}
