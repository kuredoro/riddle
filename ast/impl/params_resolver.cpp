#include <iostream>

#include "visitors.hpp"
namespace visitors {

using namespace ast;

void ParamsValidator::visit(Program* node) {
    // initial value of a varible could be an expression
    for (auto var : node->variables) {
        var->type->accept(*this);
    }
    // length of array could be an expression
    for (auto type : node->types) {
        type->type->accept(*this);
    }
    // routine can contain routine calls
    for (auto routine : node->routines) {
        routine->accept(*this);
    }
}

void ParamsValidator::visit(RoutineDecl* node) { node->body->accept(*this); }

void ParamsValidator::visit(Type*) {}

void ParamsValidator::visit(AliasedType* node) {
    node->actualType->accept(*this);
}

void ParamsValidator::visit(PrimitiveType*) {}

void ParamsValidator::visit(IntegerType*) {}

void ParamsValidator::visit(RealType*) {}

void ParamsValidator::visit(BooleanType*) {}

void ParamsValidator::visit(ArrayType* node) { node->length->accept(*this); }

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
    for (auto var : node->variables) {
        var->accept(*this);
    }
    for (auto statement : node->statements) {
        statement->accept(*this);
    }
}

void ParamsValidator::visit(Statement*) {}

void ParamsValidator::visit(ReturnStatement* node) {
    node->expression->accept(*this);
}

void ParamsValidator::visit(Assignment* node) { node->rhs->accept(*this); }

void ParamsValidator::visit(WhileLoop* node) { node->body->accept(*this); }

void ParamsValidator::visit(ForLoop* node) { node->body->accept(*this); }

void ParamsValidator::visit(IfStatement* node) {
    node->ifBody->accept(*this);
    if (node->elseBody != nullptr) {
        node->elseBody->accept(*this);
    }
}

void ParamsValidator::visit(Expression*) {}

void ParamsValidator::visit(UnaryExpression* node) {
    node->operand->accept(*this);
}

void ParamsValidator::visit(BinaryExpression* node) {
    node->operand1->accept(*this);
    node->operand2->accept(*this);
}

void ParamsValidator::visit(Primary*) {}

void ParamsValidator::visit(IntegerLiteral*) {}

void ParamsValidator::visit(RealLiteral*) {}

void ParamsValidator::visit(BooleanLiteral*) {}

void ParamsValidator::visit(Identifier*) {}

void ParamsValidator::visit(RoutineCall* node) {
    sPtr<RoutineDecl> routine = node->routine.lock();
    std::size_t length = node->args.size();
    if (node->args.size() != routine->parameters.size()) {
        error(node->begin, "amount of arguments is invalid");
    }
    node->type = routine->returnType;
    for (std::size_t i = 0; i < length; i++) {
        // check that types of args are pairwise equal:
        if (node->args[i]->type != routine->parameters[i]->type) {
            error(node->args[i]->begin, "argument type is invalid");
        }
    }
}

} // namespace visitors