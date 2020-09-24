#pragma once

#include <memory>
#include <vector>

#include "lexer.hpp"

namespace ast {

template <typename T> using sPtr = std::shared_ptr<T>;

struct Node;
struct Program;
struct Routine;
struct Parameter;
struct Type;
struct AliasedType;
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
// struct UnaryExpression;
// struct BinaryExpression;
struct Primitive;
struct RoutineCall;
struct ModifiablePrimary;

class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visit(Program* node) = 0;
    virtual void visit(Routine* node) = 0;
    virtual void visit(Parameter* node) = 0;
    virtual void visit(Type* node) = 0;
    virtual void visit(PrimitiveType* node) = 0;
    virtual void visit(ArrayType* node) = 0;
    virtual void visit(RecordType* node) = 0;
    virtual void visit(Variable* node) = 0;
    virtual void visit(Body* node) = 0;
    virtual void visit(Statement* node) = 0;
    virtual void visit(Assignment* node) = 0;
    // virtual void visit(RoutineCall* node) = 0;
    virtual void visit(WhileLoop* node) = 0;
    virtual void visit(ForLoop* node) = 0;
    virtual void visit(IfStatement* node) = 0;
    virtual void visit(Expression* node) = 0;
    virtual void visit(Primitive* node) = 0;
    virtual void visit(ModifiablePrimary* node) = 0;
    virtual void visit(RoutineCall* node) = 0;
    // virtual void visit(UnaryExpression* node) = 0;
    // virtual void visit(BinaryExpression* node) = 0;
};

// TODO: fill the following structs with relevant data

struct Node {
    lexer::Token::Position begin, end;
    bool operator==(const Node& other) const {
        return begin == other.begin && end == other.end;
    }
    virtual void accept(Visitor& v) = 0;
    virtual ~Node() = default;
};
struct Program : Node {
    std::vector<sPtr<Routine>> routines;
    std::vector<sPtr<Variable>> variables;
    std::vector<sPtr<Type>> types;
    bool operator==(const Program& other) const {
        return Node::operator==(other) && routines == other.routines &&
               variables == other.variables && types == other.types;
    }
    void accept(Visitor& v) override { v.visit(this); }
};
struct Routine : Node {
    lexer::Token name;
    std::vector<sPtr<Parameter>> parameters;
    sPtr<Type> returnType;
    sPtr<Body> body;
    bool operator==(const Routine& other) const {
        return Node::operator==(other) && name == other.name &&
               returnType == other.returnType && body == other.body &&
               parameters == other.parameters;
    }
    void accept(Visitor& v) override { v.visit(this); }
};
struct Parameter : Node {
    lexer::Token name;
    sPtr<Type> type;
    bool operator==(const Parameter& other) const {
        return Node::operator==(other) && name == other.name &&
               type == other.type;
    }
    void accept(Visitor& v) override { v.visit(this); }
};
struct Type : Node {
    virtual void accept(Visitor& v) override { v.visit(this); }
};
/**
 * To handle the "Identifier" kind of type
 */
struct AliasedType : Type {
    lexer::Token name;
    bool operator==(const AliasedType& other) const {
        return Node::operator==(other) && name == other.name;
    }
    void accept(Visitor& v) override { v.visit(this); }
};
struct PrimitiveType : Type {
    lexer::Token type;
    bool operator==(const PrimitiveType& other) const {
        return Node::operator==(other) && type == other.type;
    }
    void accept(Visitor& v) override { v.visit(this); }
};
struct ArrayType : Type {
    sPtr<Expression> length;
    sPtr<Type> elementType;
    bool operator==(const ArrayType& other) const {
        return Node::operator==(other) && length == other.length &&
               elementType == other.elementType;
    }
    void accept(Visitor& v) override { v.visit(this); }
};
struct RecordType : Type {
    std::vector<sPtr<Variable>> fields;
    bool operator==(const RecordType& other) const {
        return Node::operator==(other) && fields == other.fields;
    }
    void accept(Visitor& v) override { v.visit(this); }
};
struct Variable : Node {
    void accept(Visitor& v) override { v.visit(this); }
};
struct Body : Node {
    void accept(Visitor& v) override { v.visit(this); }
};
struct Statement : Node {
    void accept(Visitor& v) override { v.visit(this); }
};
struct Assignment : Statement {
    void accept(Visitor& v) override { v.visit(this); }
};
// struct RoutineCall : Statement {
//     void accept(Visitor& v) override { v.visit(this); }
// };
struct WhileLoop : Statement {
    sPtr<Expression> condition;
    sPtr<Body> body;
    bool operator==(const WhileLoop& other) const {
        return Node::operator==(other) && condition == other.condition &&
               body == other.body;
    }
    void accept(Visitor& v) override { v.visit(this); }
};
struct ForLoop : Statement {
    lexer::Token loopVar;
    sPtr<Expression> rangeFrom;
    sPtr<Expression> rangeTo;
    bool reverse;
    sPtr<Body> body;
    bool operator==(const ForLoop& other) const {
        return Node::operator==(other) && loopVar == other.loopVar &&
               rangeFrom == other.rangeFrom && rangeTo == other.rangeTo &&
               reverse == other.reverse && body == other.body;
    }
    void accept(Visitor& v) override { v.visit(this); }
};
struct IfStatement : Statement {
    sPtr<Expression> condition;
    sPtr<Body> ifBody;
    sPtr<Body> elseBody;
    bool operator==(const IfStatement& other) const {
        return Node::operator==(other) && condition == other.condition &&
               ifBody == other.ifBody && elseBody == other.elseBody;
    }
    void accept(Visitor& v) override { v.visit(this); }
};

struct Expression : Node {
    sPtr<Expression> operand1;
    sPtr<Expression> operand2;
    lexer::Token operation;

    bool operator==(const Expression& other) const {
        return Node::operator==(other) && operand1 == other.operand1 &&
               operand2 == other.operand2 && operation == other.operation;
    }
    void accept(Visitor& v) override { v.visit(this); }
};
struct Primitive : Expression {
    lexer::Token value;
    bool operator==(const Primitive& other) const {
        return Node::operator==(other) && value == other.value;
    }
    void accept(Visitor& v) override { v.visit(this); }
};
struct ModifiablePrimary : Expression {
    std::vector<sPtr<Expression>> args;

    bool operator==(const ModifiablePrimary& other) const {
        return Node::operator==(other) && args == other.args;
    }
    void accept(Visitor& v) override { v.visit(this); }
};
struct RoutineCall : Expression {
    lexer::Token routine;
    std::vector<sPtr<Expression>> args;

    bool operator==(const RoutineCall& other) const {
        return Node::operator==(other) && args == other.args &&
               routine == other.routine;
    }
    void accept(Visitor& v) override { v.visit(this); }
};
} // namespace ast
