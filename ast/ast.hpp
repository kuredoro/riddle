#pragma once
#include "fmt/format.h"
#include "lexer.hpp"
#include <memory>
#include <vector>

namespace ast {

template <typename T> using sPtr = std::shared_ptr<T>;

struct Error {
    lexer::Token::Position pos;
    std::string message;
};

struct Node;
struct Program;
struct RoutineDecl;
struct TypeDecl;
struct Type;
struct AliasedType;
struct PrimitiveType;
struct IntegerType;
struct RealType;
struct BooleanType;
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
struct IntegerLiteral;
struct RealLiteral;
struct BooleanLiteral;
struct Identifier;
struct RoutineCall;

template <typename T = void> class Visitor {
public:
    virtual ~Visitor() = default;
    virtual T visit(Program* node) = 0;
    virtual T visit(RoutineDecl* node) = 0;
    virtual T visit(AliasedType* node) = 0;
    virtual T visit(TypeDecl* node) = 0;
    virtual T visit(IntegerType* node) = 0;
    virtual T visit(RealType* node) = 0;
    virtual T visit(BooleanType* node) = 0;
    virtual T visit(ArrayType* node) = 0;
    virtual T visit(RecordType* node) = 0;
    virtual T visit(VariableDecl* node) = 0;
    virtual T visit(Body* node) = 0;
    virtual T visit(ReturnStatement* node) = 0;
    virtual T visit(Assignment* node) = 0;
    virtual T visit(WhileLoop* node) = 0;
    virtual T visit(ForLoop* node) = 0;
    virtual T visit(IfStatement* node) = 0;
    virtual T visit(IntegerLiteral* node) = 0;
    virtual T visit(RealLiteral* node) = 0;
    virtual T visit(BooleanLiteral* node) = 0;
    virtual T visit(Identifier* node) = 0;
    virtual T visit(RoutineCall* node) = 0;
    virtual T visit(UnaryExpression* node) = 0;
    virtual T visit(BinaryExpression* node) = 0;

    std::vector<Error> getErrors() { return std::vector<Error>(m_errors); }

protected:
    std::vector<Error> m_errors;

    template <typename... Args>
    inline void error(lexer::Token::Position pos, const std::string& msg,
                      Args... args) {
        m_errors.push_back(Error{
            .pos = pos,
            .message = fmt::format(msg, args...),
        });
    }
};

struct Node {
    lexer::Token::Position begin, end;
    bool operator==(const Node& other) const {
        return begin == other.begin && end == other.end;
    }
    // should be virtual, never to be called
    template <typename T> T accept(Visitor<T>&){};
    virtual ~Node() = default;
};

struct Program : Node {
    std::vector<sPtr<RoutineDecl>> routines;
    std::vector<sPtr<VariableDecl>> variables;
    std::vector<sPtr<TypeDecl>> types;
    bool operator==(const Program& other) const {
        return Node::operator==(other) && routines == other.routines &&
               variables == other.variables && types == other.types;
    }
    template <typename T> T accept(Visitor<T>& v) { return v.visit(this); }
};

struct RoutineDecl : Node {
    std::string name;
    std::vector<sPtr<VariableDecl>> parameters;
    sPtr<Type> returnType;
    sPtr<Body> body;
    bool operator==(const RoutineDecl& other) const {
        return Node::operator==(other) && name == other.name &&
               returnType == other.returnType && body == other.body &&
               parameters == other.parameters;
    }
    template <typename T> T accept(Visitor<T>& v) { return v.visit(this); }
};

struct TypeDecl : Node {
    std::string name;
    sPtr<Type> type;
    bool operator==(const TypeDecl& other) const {
        return Node::operator==(other) && name == other.name;
    }
    template <typename T> T accept(Visitor<T>& v) { return v.visit(this); }
};

struct Type : Node {
    // should be virtual, never to be called
    template <typename T> T accept(Visitor<T>&){};
};

/**
 * To handle the "Identifier" kind of type
 */
struct AliasedType : Type {
    std::string name;
    sPtr<Type> actualType;
    bool operator==(const AliasedType& other) const {
        return Node::operator==(other) && name == other.name;
    }
    template <typename T> T accept(Visitor<T>& v) { return v.visit(this); }
};

struct PrimitiveType : Type {
    bool operator==(const PrimitiveType& other) const {
        return Node::operator==(other);
    }
    // should be virtual, never to be called
    template <typename T> T accept(Visitor<T>&){};
};

struct IntegerType : PrimitiveType {
    template <typename T> T accept(Visitor<T>& v) { return v.visit(this); }
};

struct RealType : PrimitiveType {
    template <typename T> T accept(Visitor<T>& v) { return v.visit(this); }
};

struct BooleanType : PrimitiveType {
    template <typename T> T accept(Visitor<T>& v) { return v.visit(this); }
};

struct ArrayType : Type {
    sPtr<Expression> length;
    sPtr<Type> elementType;
    bool operator==(const ArrayType& other) const {
        return Node::operator==(other) && length == other.length &&
               elementType == other.elementType;
    }
    template <typename T> T accept(Visitor<T>& v) { return v.visit(this); }
};

struct RecordType : Type {
    std::vector<sPtr<VariableDecl>> fields;
    bool operator==(const RecordType& other) const {
        return Node::operator==(other) && fields == other.fields;
    }
    template <typename T> T accept(Visitor<T>& v) { return v.visit(this); }
};

struct VariableDecl : Node {
    std::string name;
    sPtr<Type> type;
    sPtr<Expression> initialValue;
    bool operator==(const VariableDecl& other) const {
        return Node::operator==(other) && name == other.name &&
               type == other.type && initialValue == other.initialValue;
    }
    template <typename T> T accept(Visitor<T>& v) { return v.visit(this); }
};

struct Body : Node {
    std::vector<sPtr<Statement>> statements;
    std::vector<sPtr<VariableDecl>> variables;
    std::vector<sPtr<TypeDecl>> types;
    bool operator==(const Body& other) const {
        return Node::operator==(other) && statements == other.statements &&
               variables == other.variables && types == other.types;
    }
    template <typename T> T accept(Visitor<T>& v) { return v.visit(this); }
};

struct Statement : virtual Node {
    // should be virtual, never to be called
    template <typename T> T accept(Visitor<T>& v) {
        if (auto ret = dynamic_cast<ReturnStatement*>(this)) {
            return ret->accept(v);
        }
    };
};

struct Assignment : Statement {
    sPtr<Expression> lhs, rhs; // Left/Right-Hand-Side
    bool operator==(const Assignment& other) const {
        return Node::operator==(other) && lhs == other.lhs && rhs == other.rhs;
    }
    template <typename T> T accept(Visitor<T>& v) { return v.visit(this); }
};

struct WhileLoop : Statement {
    sPtr<Expression> condition;
    sPtr<Body> body;
    bool operator==(const WhileLoop& other) const {
        return Node::operator==(other) && condition == other.condition &&
               body == other.body;
    }
    template <typename T> T accept(Visitor<T>& v) { return v.visit(this); }
};

struct ForLoop : Statement {
    sPtr<VariableDecl> loopVar;
    sPtr<Expression> rangeFrom;
    sPtr<Expression> rangeTo;
    bool reverse = false;
    sPtr<Body> body;
    bool operator==(const ForLoop& other) const {
        return Node::operator==(other) && loopVar == other.loopVar &&
               rangeFrom == other.rangeFrom && rangeTo == other.rangeTo &&
               reverse == other.reverse && body == other.body;
    }
    template <typename T> T accept(Visitor<T>& v) { return v.visit(this); }
};

struct IfStatement : Statement {
    sPtr<Expression> condition;
    sPtr<Body> ifBody;
    sPtr<Body> elseBody;
    bool operator==(const IfStatement& other) const {
        return Node::operator==(other) && condition == other.condition &&
               ifBody == other.ifBody && elseBody == other.elseBody;
    }
    template <typename T> T accept(Visitor<T>& v) { return v.visit(this); }
};

struct ReturnStatement : Statement {
    sPtr<Expression> expression;
    bool operator==(const ReturnStatement& other) const {
        return Node::operator==(other) && expression == other.expression;
    }
    template <typename T> T accept(Visitor<T>& v) { return v.visit(this); }
};

struct Expression : virtual Node {
    bool constant = false; // tells if this expression is compile-time constant
    sPtr<Type> type;
    // should be virtual, never to be called
    template <typename T> T accept(Visitor<T>& v) {
        if (auto bin = dynamic_cast<BinaryExpression*>(this)) {
            return bin->accept(v);
        } else if (auto un = dynamic_cast<UnaryExpression*>(this)) {
            return un->accept(v);
        } else if (auto rl = dynamic_cast<RealLiteral*>(this)) {
            return rl->accept(v);
        }
    };
    virtual ~Expression() = default;
};

struct UnaryExpression : Expression {
    sPtr<Expression> operand;
    lexer::TokenType operation;

    bool operator==(const UnaryExpression& other) const {
        return Node::operator==(other) && operand == other.operand &&
               operation == other.operation;
    }
    template <typename T> T accept(Visitor<T>& v) { return v.visit(this); }
};

struct BinaryExpression : Expression {
    sPtr<Expression> operand1;
    sPtr<Expression> operand2;
    lexer::TokenType operation;

    bool operator==(const BinaryExpression& other) const {
        return Node::operator==(other) && operand1 == other.operand1 &&
               operand2 == other.operand2 && operation == other.operation;
    }
    template <typename T> T accept(Visitor<T>& v) { return v.visit(this); }
};

struct Primary : Expression, Statement {
    bool operator==(const Primary& other) const {
        return Node::operator==(other);
    }
    // should be virtual, never to be called
    template <typename T> T accept(Visitor<T>&){};
};

struct IntegerLiteral : Expression {
    long long value;
    IntegerLiteral(long long value) : value(value) {
        this->constant = true;
        this->type = std::make_shared<IntegerType>();
    }
    bool operator==(const IntegerLiteral& other) const {
        return Expression::operator==(other) && value == other.value;
    }
    template <typename T> T accept(Visitor<T>& v) { return v.visit(this); }
};

struct RealLiteral : Expression {
    double value;
    RealLiteral(double value) : value(value) {
        this->constant = true;
        this->type = std::make_shared<RealType>();
    }
    bool operator==(const RealLiteral& other) const {
        return Expression::operator==(other) && value == other.value;
    }
    template <typename T> T accept(Visitor<T>& v) { return v.visit(this); }
};

struct BooleanLiteral : Expression {
    bool value;
    BooleanLiteral(bool value) : value(value) {
        this->constant = true;
        this->type = std::make_shared<BooleanType>();
    }
    bool operator==(const BooleanLiteral& other) const {
        return Expression::operator==(other) && value == other.value;
    }
    template <typename T> T accept(Visitor<T>& v) { return v.visit(this); }
};

// Used for holding variables. Can temporary hold unparenthesized routine calls
//  until resolved.
struct Identifier : Primary {
    std::string name;
    sPtr<VariableDecl> variable;
    Identifier(std::string name) : name(name) {}
    bool operator==(const Identifier& other) const {
        return Primary::operator==(other) && name == other.name &&
               variable == other.variable;
    }
    template <typename T> T accept(Visitor<T>& v) { return v.visit(this); }
};

struct RoutineCall : Primary {
    std::weak_ptr<RoutineDecl> routine;
    std::string routineName;
    std::vector<sPtr<Expression>> args;

    bool operator==(const RoutineCall& other) const {
        return Node::operator==(other) && args == other.args &&
               routine.lock() == other.routine.lock();
    }
    template <typename T> T accept(Visitor<T>& v) { return v.visit(this); }
};

} // namespace ast
