#include <string>

enum token_code{
    VarDec, // var
    TypeDec, // type
    RoutineDec, // routine
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
    DevOp, // /
    RemainderOp, // %
    PlusOp, // +
    MinusOp, // -
    IntegerLiteral, // int const
    // RealLiteral, // Do we need it? I think that "IntegerLiteral Dot IntegerLiteral" may not be the best way because when we should accept integers like 00256
    Identifier, // name
    Dot, // .
    TwoDots, // ..
    Comma, // ,
    BracketOpen, // (
    BracketClose, // )
    SquareBracketOpen, // [
    SquareBracketClose // ]
};

class Token{
public:
    token_code code;
    int coordinate;
    std::string image;

    Token(token_code n_code, int n_coordinate, std::string n_image){
        code = n_code;
        coordinate = n_coordinate;
        image = n_image;
    }
};