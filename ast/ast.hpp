#pragma once

#include <vector>
#include <memory>
#include "lexer.hpp"

namespace ast {

template <typename T>
using sPtr = std::shared_ptr<T>;

struct Node {
    lexer::Token::Position begin, end;
    virtual bool operator==(const Node& other) const {
        return begin == other.begin && end == other.end;
    }

    virtual ~Node() = default;
};

struct Program;
struct Routine;
struct Parameter;
struct Type;
struct PrimitiveType;
struct ArrayType;
struct RecordType;
struct Variable;
struct BodyNode;
struct Statement;
struct Assignment;
struct RoutineCall;
struct WhileLoop;
struct ForLoop;
struct IfStatement;
struct Expression;
struct UnaryExpression;
struct BinaryExpression;

// TODO: fill the following structs with relevant data

struct Program : Node {
    std::vector<sPtr<Routine>> routineTable;
    bool operator==(const Program& other) const {
        return Node::operator==(other) && routineTable == other.routineTable;
    }
};

struct Routine : Node {};
struct Parameter : Node {};
struct Type : Node {};
struct PrimitiveType : Type {};
struct ArrayType : Type {};
struct RecordType : Type {};
struct Variable : Node {};
struct BodyNode : Node {};
struct Statement : Node {};
struct Assignment : Statement {};
struct RoutineCall : Statement {};
struct WhileLoop : Statement {};
struct ForLoop : Statement {};
struct IfStatement : Statement {};
struct Expression : Node {};
struct UnaryExpression : Expression {};
struct BinaryExpression : Expression {};


} // namespace ast
