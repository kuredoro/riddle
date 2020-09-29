#pragma once
#include "lexer.hpp"
#include <memory>
#include <vector>

namespace ast {

template <typename T> using sPtr = std::shared_ptr<T>;

struct Node;
struct Program;
struct RoutineDecl;
struct Parameter;
struct TypeDecl;
struct Type;
struct AliasedType;
struct PrimitiveType;
struct ArrayType;
struct RecordType;
struct VariableDecl;
struct Body;
struct Statement;
struct ReturnStatement;
struct Assignment;
struct WhileLoop;
struct ForLoop;
struct IfStatement;
struct Expression;
struct UnaryExpression;
struct BinaryExpression;
struct Primary;
struct RoutineCall;

class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visit(Program* node) = 0;
    virtual void visit(RoutineDecl* node) = 0;
    virtual void visit(Parameter* node) = 0;
    virtual void visit(Type* node) = 0;
    virtual void visit(TypeDecl* node) = 0;
    virtual void visit(PrimitiveType* node) = 0;
    virtual void visit(ArrayType* node) = 0;
    virtual void visit(RecordType* node) = 0;
    virtual void visit(VariableDecl* node) = 0;
    virtual void visit(Body* node) = 0;
    virtual void visit(Statement* node) = 0;
    virtual void visit(ReturnStatement* node) = 0;
    virtual void visit(Assignment* node) = 0;
    virtual void visit(WhileLoop* node) = 0;
    virtual void visit(ForLoop* node) = 0;
    virtual void visit(IfStatement* node) = 0;
    virtual void visit(Expression* node) = 0;
    virtual void visit(Primary* node) = 0;
    virtual void visit(RoutineCall* node) = 0;
    virtual void visit(UnaryExpression* node) = 0;
    virtual void visit(BinaryExpression* node) = 0;
};

struct Node {
    lexer::Token::Position begin, end;
    bool operator==(const Node& other) const {
        return begin == other.begin && end == other.end;
    }
    virtual void accept(Visitor& v) = 0;
    virtual ~Node() = default;
};

struct Expression : virtual Node {
    void accept(Visitor& v) override { v.visit(this); }
};

struct Program : Node {
    std::vector<sPtr<RoutineDecl>> routines;
    std::vector<sPtr<VariableDecl>> variables;
    std::vector<sPtr<TypeDecl>> types;
    bool operator==(const Program& other) const {
        return Node::operator==(other) && routines == other.routines &&
               variables == other.variables && types == other.types;
    }
    void accept(Visitor& v) override { v.visit(this); }
};

struct RoutineDecl : Node {
    std::string name;
    std::vector<sPtr<Parameter>> parameters;
    sPtr<Type> returnType;
    sPtr<Body> body;
    bool operator==(const RoutineDecl& other) const {
        return Node::operator==(other) && name == other.name &&
               returnType == other.returnType && body == other.body &&
               parameters == other.parameters;
    }
    void accept(Visitor& v) override { v.visit(this); }
};

struct Parameter : Node {
    std::string name;
    sPtr<Type> type;
    bool operator==(const Parameter& other) const {
        return Node::operator==(other) && name == other.name &&
               type == other.type;
    }
    void accept(Visitor& v) override { v.visit(this); }
};

struct TypeDecl : Node {
    std::string name;
    sPtr<Type> type;
    bool operator==(const TypeDecl& other) const {
        return Node::operator==(other) && name == other.name;
    }
    virtual void accept(Visitor& v) override { v.visit(this); }
};

struct Type : Node {
    virtual void accept(Visitor& v) override { v.visit(this); }
};

/**
 * To handle the "Identifier" kind of type
 */
struct AliasedType : Type {
    std::string name;
    bool operator==(const AliasedType& other) const {
        return Node::operator==(other) && name == other.name;
    }
    void accept(Visitor& v) override { v.visit(this); }
};

struct PrimitiveType : Type {
    lexer::TokenType type;
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
    std::vector<sPtr<VariableDecl>> fields;
    bool operator==(const RecordType& other) const {
        return Node::operator==(other) && fields == other.fields;
    }
    void accept(Visitor& v) override { v.visit(this); }
};

struct VariableDecl : Node {
    std::string name;
    sPtr<Type> type;
    sPtr<Expression> expression;
    bool operator==(const VariableDecl& other) const {
        return Node::operator==(other) && name == other.name &&
               type == other.type && expression == other.expression;
    }
    void accept(Visitor& v) override { v.visit(this); }
};

struct Body : Node {
    std::vector<sPtr<Statement>> statements;
    std::vector<sPtr<VariableDecl>> variables;
    std::vector<sPtr<TypeDecl>> types;
    bool operator==(const Body& other) const {
        return Node::operator==(other) && statements == other.statements &&
               variables == other.variables && types == other.types;
    }
    void accept(Visitor& v) override { v.visit(this); }
};

struct Statement : virtual Node {
    void accept(Visitor& v) override { v.visit(this); }
};

struct Assignment : Statement {
    sPtr<Expression> lhs, rhs; // Left/Right-Hand-Side
    bool operator==(const Assignment& other) const {
        return Node::operator==(other) && lhs == other.lhs && rhs == other.rhs;
    }
    void accept(Visitor& v) override { v.visit(this); }
};

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
    std::string loopVar;
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

struct ReturnStatement : Statement {
    sPtr<Expression> expression;
    bool operator==(const ReturnStatement& other) const {
        return Node::operator==(other) && expression == other.expression;
    }
    void accept(Visitor& v) override { v.visit(this); }
};

struct UnaryExpression : Expression {
    sPtr<Expression> operand;
    lexer::TokenType operation;

    bool operator==(const UnaryExpression& other) const {
        return Node::operator==(other) && operand == other.operand &&
               operation == other.operation;
    }
    void accept(Visitor& v) override { v.visit(this); }
};

struct BinaryExpression : Expression {
    sPtr<Expression> operand1;
    sPtr<Expression> operand2;
    lexer::TokenType operation;

    bool operator==(const BinaryExpression& other) const {
        return Node::operator==(other) && operand1 == other.operand1 &&
               operand2 == other.operand2 && operation == other.operation;
    }
    void accept(Visitor& v) override { v.visit(this); }
};

struct Primary : Expression, Statement {
    lexer::TokenType value;
    bool operator==(const Primary& other) const {
        return Node::operator==(other) && value == other.value;
    }
    void accept(Visitor& v) override { v.visit(this); }
};

struct RoutineCall : Primary {
    sPtr<RoutineDecl> routine;
    std::string routineName;
    std::vector<sPtr<Expression>> args;

    bool operator==(const RoutineCall& other) const {
        return Node::operator==(other) && args == other.args &&
               routine == other.routine;
    }
    void accept(Visitor& v) override { v.visit(this); }
};

} // namespace ast
