#include "ast.hpp"

namespace visitors {

using namespace ast;

class PrintVisitor : public Visitor {
public:
    PrintVisitor(size_t depth = 0) : depth(depth) {}
    void visit(Program* node) override;
    void visit(RoutineDecl* node) override;
    void visit(Parameter* node) override;
    void visit(Type* node) override;
    void visit(AliasedType* node) override;
    void visit(PrimitiveType* node) override;
    void visit(IntegerType* node) override;
    void visit(RealType* node) override;
    void visit(BooleanType* node) override;
    void visit(ArrayType* node) override;
    void visit(RecordType* node) override;
    void visit(VariableDecl* node) override;
    void visit(TypeDecl* node) override;
    void visit(Body* node) override;
    void visit(Statement* node) override;
    void visit(ReturnStatement* node) override;
    void visit(Assignment* node) override;
    void visit(WhileLoop* node) override;
    void visit(ForLoop* node) override;
    void visit(IfStatement* node) override;
    void visit(Expression* node) override;
    void visit(UnaryExpression* node) override;
    void visit(BinaryExpression* node) override;
    void visit(Primary* node) override;
    void visit(IntegerLiteral* node) override;
    void visit(RealLiteral* node) override;
    void visit(BooleanLiteral* node) override;
    void visit(Identifier* node) override;
    void visit(RoutineCall* node) override;

private:
    size_t depth;
};

/**
 * This visitor performs resolution of variables/routine calls to their
 *  declarations.
 * First, it verifies that there are no conflicts in names in each scope.
 * Then, each instance of Identifier is either filled with a reference to its
 *  declaration or replaced with a routine call referring to its routine
 *  declaration.
 */
class IdentifierResolver : public Visitor {
public:
    void visit(Program* node) override;
    void visit(RoutineDecl* node) override;
    void visit(Parameter* node) override;
    void visit(Type* node) override;
    void visit(AliasedType* node) override;
    void visit(PrimitiveType* node) override;
    void visit(IntegerType* node) override;
    void visit(RealType* node) override;
    void visit(BooleanType* node) override;
    void visit(ArrayType* node) override;
    void visit(RecordType* node) override;
    void visit(VariableDecl* node) override;
    void visit(TypeDecl* node) override;
    void visit(Body* node) override;
    void visit(Statement* node) override;
    void visit(ReturnStatement* node) override;
    void visit(Assignment* node) override;
    void visit(WhileLoop* node) override;
    void visit(ForLoop* node) override;
    void visit(IfStatement* node) override;
    void visit(Expression* node) override;
    void visit(UnaryExpression* node) override;
    void visit(BinaryExpression* node) override;
    void visit(Primary* node) override;
    void visit(IntegerLiteral* node) override;
    void visit(RealLiteral* node) override;
    void visit(BooleanLiteral* node) override;
    void visit(Identifier* node) override;
    void visit(RoutineCall* node) override;

private:
    // `routines` is a map since we cannot have 2 routines with the same name
    std::map<std::string, sPtr<RoutineDecl>> routines;
    // `variables` is a vector to allow repetition (in different scopes)
    std::vector<sPtr<VariableDecl>> variables;
};

} // namespace visitors
