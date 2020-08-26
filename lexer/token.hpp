#pragma once

#include <string>

enum class TokenType
{
    InvalidToken,
    VarDec,             // var
    TypeDec,            // type
    RoutineDec,         // routine
    Return,             // return
    Is,                 // is
    IntegerType,        // integer
    RealType,           // real
    BooleanType,        // boolean
    RecordType,         // record
    ArrayType,          // array
    True,               // true
    False,              // false
    WhileLoop,          // while
    ForLoop,            // for
    LoopBegin,          // loop
    End,                // end
    ReverseRange,       // reverse
    InRange,            // in
    If,                 // if
    Then,               // then
    Else,               // else
    AndLogic,           // and
    OrLogic,            // or
    XorLogic,           // xor
    SmallerComp,        // <
    SeqComp,            // <=
    BiggerComp,         // >
    BeqComp,            // >=
    EqComp,             // =
    NeqComp,            // /=
    MultOp,             // *
    DevOp,              // /
    RemainderOp,        // %
    PlusOp,             // +
    MinusOp,            // -
    IntegerLiteral,     // int const
    RealLiteral,        // real const
    Identifier,         // name
    Dot,                // .
    TwoDots,            // ..
    Comma,              // ,
    BracketOpen,        // (
    BracketClose,       // )
    SquareBracketOpen,  // [
    SquareBracketClose, // ]
    SingleLineComment,  // //
    NewLine,            // \n
    Semicolumn,         // ;
    Assignment,         // :=
    Column,             // ;
};

struct Token
{
    TokenType type;
    int srcPos;
    std::string image;
};
