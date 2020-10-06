#include "san.hpp"

using namespace ast;

namespace san {

void ArrayLengthEnforcer::visit(Program* node) {
    for (auto var : node->variables) {
        var->type->accept(*this);
    }
    for (auto routine : node->routines) {
        routine->accept(*this);
    }
}

void ArrayLengthEnforcer::visit(RoutineDecl* node) {
    m_insideParameters = true;
    for (auto param : node->parameters) {
        param->type->accept(*this);
    }
    m_insideParameters = false;

    if (node->returnType != nullptr) {
        node->returnType->accept(*this);
    }
    node->body->accept(*this);
}

void ArrayLengthEnforcer::visit(Type*) {}

void ArrayLengthEnforcer::visit(AliasedType*) {}

void ArrayLengthEnforcer::visit(PrimitiveType*) {}

void ArrayLengthEnforcer::visit(IntegerType*) {}

void ArrayLengthEnforcer::visit(RealType*) {}

void ArrayLengthEnforcer::visit(BooleanType*) {}

void ArrayLengthEnforcer::visit(ArrayType* node) {
    if (!m_insideParameters && node->length == nullptr) {
        error(node->begin, "array size omitted in non-signature context");
    }
    node->elementType->accept(*this);
}

void ArrayLengthEnforcer::visit(RecordType* node) {
    for (auto field : node->fields) {
        field->accept(*this);
    }
}

void ArrayLengthEnforcer::visit(VariableDecl* node) {
    if (node->type != nullptr) {
        node->type->accept(*this);
    }
}

void ArrayLengthEnforcer::visit(TypeDecl*) {}

void ArrayLengthEnforcer::visit(Body* node) {
    for (auto var : node->variables) {
        var->accept(*this);
    }
}

void ArrayLengthEnforcer::visit(Statement*) {}

void ArrayLengthEnforcer::visit(ReturnStatement*) {}

void ArrayLengthEnforcer::visit(Assignment*) {}

void ArrayLengthEnforcer::visit(WhileLoop* node) { node->body->accept(*this); }

void ArrayLengthEnforcer::visit(ForLoop* node) { node->body->accept(*this); }

void ArrayLengthEnforcer::visit(IfStatement* node) {
    node->ifBody->accept(*this);
    if (node->elseBody != nullptr) {
        node->elseBody->accept(*this);
    }
}

void ArrayLengthEnforcer::visit(Expression*) {}

void ArrayLengthEnforcer::visit(UnaryExpression*) {}

void ArrayLengthEnforcer::visit(BinaryExpression*) {}

void ArrayLengthEnforcer::visit(Primary*) {}

void ArrayLengthEnforcer::visit(IntegerLiteral*) {}

void ArrayLengthEnforcer::visit(RealLiteral*) {}

void ArrayLengthEnforcer::visit(BooleanLiteral*) {}

void ArrayLengthEnforcer::visit(Identifier*) {}

void ArrayLengthEnforcer::visit(RoutineCall*) {}

} // namespace visitors
