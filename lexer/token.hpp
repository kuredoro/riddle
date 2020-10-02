#pragma once

#include <string>

namespace lexer {

enum class TokenType {
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

    OpenParen, // (
    OpenBrack, // [
    Comma,     // ,
    Dot,       // .
    TwoDots,   // ..

    CloseParen, // )
    CloseBrack, // ]
    Semicolon,  // ;
    Colon,      // :
    NewLine,    // \n

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
    Return,
};

struct Token {

    struct Position {
        size_t line;   // starting at 1
        size_t column; // starting at 1, in bytes
        bool operator==(const Position& other) const {
            return line == other.line && column == other.column;
        }
    };

    TokenType type;
    Position pos;
    std::string lit;

    friend bool operator==(const Token& a, const Token& b) {
        return a.type == b.type && a.pos == b.pos && a.lit == b.lit;
    }

    friend bool operator!=(const Token& a, const Token& b) { return !(a == b); }
};

std::string to_string(TokenType type);

} // namespace lexer
