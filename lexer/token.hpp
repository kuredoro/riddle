#pragma once

#include <string>

namespace lexer {

enum class TokenType{
    Eof,
    Error,
    VarDecl, // var
    TypeDecl, // type
    RoutineDecl, // routine
    Is, // is
    IntegerType, // integer
    RealType, // real
    BooleanType, // boolean
    RecordType, // record
    ArrayType, // array
    True, // true
    False, // false
    WhileLoop, // while
    ForLoop, // for
    LoopBegin, // loop
    End, // end
    ReverseRange, // reverse
    InRange, // in
    If, // if
    Else, // else
    AndLogic, // and
    OrLogic, // or
    XorLogic, // xor
    SmallerComp, // <
    SeqComp, // <=
    BiggerComp, // >
    BeqComp, // >=
    EqComp, // =
    NeqComp, // /=
    MultOp, // *
    DivOp, // /
    ModOp, // %
    AddOp, // +
    SubOp, // -
    IntegerLiteral, // int const
    RealLiteral, // real const
    Identifier, // name
    Dot, // .
    TwoDots, // ..
    Comma, // ,
    BracketOpen, // (
    BracketClose, // )
    SquareBracketOpen, // [
    SquareBracketClose, // ]
    Colon,
    Semicolon,
    NewLine,
};

struct Token {
    TokenType type;
    size_t line;
    size_t srcPos;
    std::string image;
};


}
