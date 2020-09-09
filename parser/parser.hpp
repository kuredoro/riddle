#pragma once

#include "structures.hpp"
#include "lexer.hpp"


namespace AST
{


struct Node {
    position_t begin, end;
    virtual bool operator==(const Node& other) const;
    virtual ~Node() = default;
};

// TODO: fill the following structs with relevant data

struct ProgramNode : Node {
    std::vector<struct RoutineNode> routine_table;
    bool operator==(const ProgramNode& other) const;
};

struct RoutineNode : Node {};
struct ParameterNode : Node {};
struct TypeNode : Node {};
struct PrimitiveTypeNode : TypeNode {};
struct ArrayTypeNode : TypeNode {};
struct RecordTypeNode : TypeNode {};
struct VariableNode : Node {};
struct BodyNode : Node {};
struct StatementNode : Node {};
struct AssignmentNode : StatementNode {};
struct RoutineCallNode : StatementNode {};
struct WhileLoopNode : StatementNode {};
struct ForLoopNode : StatementNode {};
struct IfStatementNode : StatementNode {};
struct ExpressionNode : Node {};
struct UnaryExpressionNode : ExpressionNode {};
struct BinaryExpressionNode : ExpressionNode {};

// TODO: define function to use this struct, that also advances
struct Error {
    position_t position;
    std::string message;
};

// TODO: define a type for the "context"

class Parser {
public:
    Parser(lexer::Lexer lexer) : m_lexer(lexer) {}
    ProgramNode parseProgram ();
    RoutineNode parseRoutine ();
    ParameterNode parseParameter ();
    TypeNode parseType ();
    PrimitiveTypeNode parsePrimitiveType ();
    ArrayTypeNode parseArrayType ();
    RecordTypeNode parseRecordType ();
    VariableNode parseVariable ();
    BodyNode parseBody ();
    StatementNode parseStatement ();
    AssignmentNode parseAssignment ();
    RoutineCallNode parseRoutineCall ();
    WhileLoopNode parseWhileLoop ();
    ForLoopNode parseForLoop ();
    IfStatementNode parseIfStatement ();
    ExpressionNode parseExpression ();
    UnaryExpressionNode parseUnaryExpression ();
    BinaryExpressionNode parseBinaryExpression ();

private:
    lexer::Lexer m_lexer;
    std::vector<Error> m_errors;
};


} // namespace AST
