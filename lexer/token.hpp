#pragma once

#include <string>
#include "structures.hpp"

namespace lexer {

enum class TokenType{
    Illegal,
    Eof,
    Comment,

    Identifier, // my_var
    Int,        // 12345
    Real,       // 123.45

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

    OpenParen,  // (
    OpenBrack,  // [
    Comma,   // ,
    Dot,     // .
    TwoDots, // ..

    CloseParen,    // )
    CloseBrack,    // ]
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
    Not,
    And,
    Or,
    Xor,
};

struct Token {
    TokenType type;
    position_t position;
    std::string lit;

    friend bool operator==(const Token& a, const Token& b) {
        return a.type == b.type && 
               a.position == b.position &&
               a.lit == b.lit;
    }

    friend bool operator!=(const Token& a, const Token& b) {
        return !(a == b);
    }
};


}
