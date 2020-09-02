#pragma once

#include <string>

namespace lexer {

enum class TokenType{
    Illegal,
    Eof,
    Comment,

    Ident, // my_var
    Int,   // 12345
    Real,  // 123.45

    Less,    // <
    Greater, // >
    Eq,      // =

    Leq,    // <=
    Geq,    // >=
    Neq,    // /=
    Assign, // :=

    Add, // +
    Sub, // -
    Mul, // *
    Div, // /
    Mod, // %

    OParen,  // (
    OBrack,  // [
    Comma,   // ,
    Dot,     // .
    TwoDots, // ..

    CParen,    // )
    CBrack,    // ]
    Semicolon, // ;
    Colon,     // :
    NewLine,   // \n

    // Keywords
    Var,
    Type,
    Routine,
    Is,
    IntegerType,
    RealType,
    Boolean,
    Record,
    Array,
    True,
    False,
    While,
    For,
    Loop,
    End,
    Reverse, 
    In, 
    If,
    Then, 
    Else,
    And,
    Or,
    Xor,
};

struct Token {
    TokenType type;
    size_t line;       // starting at 1
    size_t column;     // starting at 1, in bytes
    std::string lit;

    friend bool operator==(const Token& a, const Token& b) {
        return a.type == b.type && 
               a.line == b.line &&
               a.column == b.column &&
               a.lit == b.lit;
    }

    friend bool operator!=(const Token& a, const Token& b) {
        return !(a == b);
    }
};


}
