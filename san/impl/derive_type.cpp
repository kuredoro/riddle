#include <iostream>

#include "san.hpp"
namespace san {

using namespace ast;

void DeriveType::visit(Program*) {}

void DeriveType::visit(RoutineDecl*) {}

void DeriveType::visit(Type*) {}

void DeriveType::visit(AliasedType*) {}

void DeriveType::visit(PrimitiveType*) {}

void DeriveType::visit(IntegerType*) {}

void DeriveType::visit(RealType*) {}

void DeriveType::visit(BooleanType*) {}

void DeriveType::visit(ArrayType*) {}

void DeriveType::visit(RecordType*) {}

void DeriveType::visit(VariableDecl*) {}

void DeriveType::visit(TypeDecl*) {}

void DeriveType::visit(Body*) {}

void DeriveType::visit(Statement*) {}

void DeriveType::visit(ReturnStatement*) {}

void DeriveType::visit(Assignment*) {}

void DeriveType::visit(WhileLoop*) {}

void DeriveType::visit(ForLoop*) {}

void DeriveType::visit(IfStatement*) {}

void DeriveType::visit(Expression*) {}

void DeriveType::visit(UnaryExpression*) {}

void DeriveType::visit(BinaryExpression*) {}

void DeriveType::visit(Primary*) {}

void DeriveType::visit(IntegerLiteral*) {}

void DeriveType::visit(RealLiteral*) {}

void DeriveType::visit(BooleanLiteral*) {}

void DeriveType::visit(Identifier*) {}

void DeriveType::visit(RoutineCall*) {}
} // namespace san
