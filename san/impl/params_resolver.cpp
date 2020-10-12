#include <iostream>

#include "san.hpp"
namespace san {

using namespace ast;

void ParamsValidator::visit(Program* node) {
    // initial value of a variable could be an expression
    for (auto var : node->variables) {
        var->accept(*this);
    }
    // initial value of a record could be a routine call
    for (auto type : node->types) {
        type->type->accept(*this);
    }
    // routine can contain routine calls
    for (auto routine : node->routines) {
        routine->accept(*this);
    }
}

void ParamsValidator::visit(RoutineDecl* node) { node->body->accept(*this); }

void ParamsValidator::visit(AliasedType* node) {
    node->actualType->accept(*this);
}

void ParamsValidator::visit(IntegerType*) {}

void ParamsValidator::visit(RealType*) {}

void ParamsValidator::visit(BooleanType*) {}

void ParamsValidator::visit(ArrayType* node) {
    node->elementType->accept(*this);
}

void ParamsValidator::visit(RecordType* node) {
    for (auto field : node->fields) {
        field->accept(*this);
    }
}

void ParamsValidator::visit(VariableDecl* node) {
    if (node->type != nullptr) {
        node->type->accept(*this);
    }
    if (node->initialValue != nullptr) {
        node->initialValue->accept(*this);
    }
}

void ParamsValidator::visit(TypeDecl* node) { node->type->accept(*this); }

void ParamsValidator::visit(Body* node) {
    for (auto type : node->types) {
        type->accept(*this);
    }
    for (auto var : node->variables) {
        var->accept(*this);
    }
    for (auto statement : node->statements) {
        statement->accept(*this);
    }
}

void ParamsValidator::visit(ReturnStatement* node) {
    node->expression->accept(*this);
}

void ParamsValidator::visit(Assignment* node) { node->rhs->accept(*this); }

void ParamsValidator::visit(WhileLoop* node) {
    node->condition->accept(*this);
    node->body->accept(*this);
}

void ParamsValidator::visit(ForLoop* node) {
    node->rangeFrom->accept(*this);
    node->rangeTo->accept(*this);
    node->body->accept(*this);
}

void ParamsValidator::visit(IfStatement* node) {
    node->condition->accept(*this);
    node->ifBody->accept(*this);
    if (node->elseBody != nullptr) {
        node->elseBody->accept(*this);
    }
}

void ParamsValidator::visit(UnaryExpression* node) {
    node->operand->accept(*this);
}

void ParamsValidator::visit(BinaryExpression* node) {
    node->operand1->accept(*this);
    node->operand2->accept(*this);
}

void ParamsValidator::visit(IntegerLiteral*) {}

void ParamsValidator::visit(RealLiteral*) {}

void ParamsValidator::visit(BooleanLiteral*) {}

void ParamsValidator::visit(Identifier*) {}

void ParamsValidator::visit(RoutineCall* node) {
    sPtr<RoutineDecl> routine = node->routine.lock();
    std::size_t argCount = node->args.size();
    if (argCount != routine->parameters.size()) {
        error(node->begin, "routine call to {} expects {} arguments, got {}",
              routine->name, routine->parameters.size(), argCount);
    }
    node->type = routine->returnType;
    for (auto arg : node->args) {
        arg->accept(*this);
    }
}

} // namespace san
