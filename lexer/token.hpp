#pragma once

#include <string>

namespace lexer {

enum class TokenType{
    Eof,
    Error,
    OneLineComment, // //
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
    Then, // then
    Else, // else
    AndLogic, // and
    OrLogic, // or
    XorLogic, // xor
    LessComp, // <
    LeqComp, // <=
    GreaterComp, // >
    GeqComp, // >=
    EqComp, // =
    NeqComp, // /=
    AssignmentOp, // :=
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
    ParenOpen, // (
    ParenClose, // )
    BracketOpen, // [
    BracketClose, // ]
    Colon, // :
    Semicolon, // ;
    NewLine,
};

struct Token {
    TokenType type;
    size_t line;
    size_t srcPos;
    std::string image;

    friend bool operator==(const Token& a, const Token& b) {
        return a.type == b.type && 
               a.line == b.line &&
               a.srcPos == b.srcPos &&
               a.image == b.image;
    }

    friend bool operator!=(const Token& a, const Token& b) {
        return !(a == b);
    }
};


}
