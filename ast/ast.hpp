#pragma once

#include <vector>
#include <memory>
#include "lexer.hpp"

namespace ast {

template <typename T>
using sPtr = std::shared_ptr<T>;

struct Node;
struct Program;
struct Routine;
struct Parameter;
struct Type;
struct PrimitiveType;
struct ArrayType;
struct RecordType;
struct Variable;
struct Body;
struct Statement;
struct Assignment;
struct RoutineCall;
struct WhileLoop;
struct ForLoop;
struct IfStatement;
struct Expression;
struct UnaryExpression;
struct BinaryExpression;

class Visitor
{
  public:
    virtual ~Visitor() = default;
    virtual void visit(Program *node) = 0;
    virtual void visit(Routine *node) = 0;
    virtual void visit(Parameter *node) = 0;
    virtual void visit(Type *node) = 0;
    virtual void visit(PrimitiveType *node) = 0;
    virtual void visit(ArrayType *node) = 0;
    virtual void visit(RecordType *node) = 0;
    virtual void visit(Variable *node) = 0;
    virtual void visit(Body *node) = 0;
    virtual void visit(Statement *node) = 0;
    virtual void visit(Assignment *node) = 0;
    virtual void visit(RoutineCall *node) = 0;
    virtual void visit(WhileLoop *node) = 0;
    virtual void visit(ForLoop *node) = 0;
    virtual void visit(IfStatement *node) = 0;
    virtual void visit(Expression *node) = 0;
    virtual void visit(UnaryExpression *node) = 0;
    virtual void visit(BinaryExpression *node) = 0;
};

// TODO: fill the following structs with relevant data

struct Node {
    lexer::Token::Position begin, end;
    bool operator==(const Node& other) const {
        return begin == other.begin && end == other.end;
    }
    virtual ~Node() = default;
};
struct Program : Node {
    std::vector<sPtr<Routine>> routines;
    std::vector<sPtr<Variable>> variables;
    std::vector<sPtr<Type>> types;
    bool operator==(const Program& other) const {
        return Node::operator==(other)
                && routines == other.routines
                && variables == other.variables
                && types == other.types;
    }
    void accept(Visitor& v) {
        v.visit(this);
    }
};
struct Routine : Node {
    lexer::Token name;
    std::vector<sPtr<Parameter>> parameters;
    sPtr<Type> returnType;
    sPtr<Body> body;
    bool operator==(const Routine& other) const {
        return Node::operator==(other)
                && name == other.name
                && returnType == other.returnType
                && body == other.body
                && parameters == other.parameters;
    }
    void accept(Visitor& v) {
        v.visit(this);
    }
};
struct Parameter : Node {
    void accept(Visitor& v) {
        v.visit(this);
    }
};
struct Type : Node {
    void accept(Visitor& v) {
        v.visit(this);
    }
};
struct PrimitiveType : Type {
    void accept(Visitor& v) {
        v.visit(this);
    }
};
struct ArrayType : Type {
    void accept(Visitor& v) {
        v.visit(this);
    }
};
struct RecordType : Type {
    void accept(Visitor& v) {
        v.visit(this);
    }
};
struct Variable : Node {
    void accept(Visitor& v) {
        v.visit(this);
    }
};
struct Body : Node {
    void accept(Visitor& v) {
        v.visit(this);
    }
};
struct Statement : Node {
    void accept(Visitor& v) {
        v.visit(this);
    }
};
struct Assignment : Statement {
    void accept(Visitor& v) {
        v.visit(this);
    }
};
struct RoutineCall : Statement {
    void accept(Visitor& v) {
        v.visit(this);
    }
};
struct WhileLoop : Statement {
    void accept(Visitor& v) {
        v.visit(this);
    }
};
struct ForLoop : Statement {
    void accept(Visitor& v) {
        v.visit(this);
    }
};
struct IfStatement : Statement {
    void accept(Visitor& v) {
        v.visit(this);
    }
};
struct Expression : Node {
    void accept(Visitor& v) {
        v.visit(this);
    }
};
struct UnaryExpression : Expression {
    void accept(Visitor& v) {
        v.visit(this);
    }
};
struct BinaryExpression : Expression {
    void accept(Visitor& v) {
        v.visit(this);
    }
};


} // namespace ast
