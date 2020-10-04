#include "visitors.hpp"

namespace visitors {

void IdentifierResolver::visit(Program* node) {
    // First, verify that no 2 routines have the same name
    for (auto routine : node->routines) {
        if (routines.find(routine->name) != routines.end()) {
            // TODO: Move the following to a function/macro
            m_errors.push_back(Error{
                .pos = routine->begin,
                .message = "A routine already exists with that name",
            });
            return;
        }
        routines.insert({routine->name, routine});
    }

    // Then, verify that global variables names are unique (from routines too)
    for (auto variable : node->variables) {
        // Any routine with the same name?
        if (routines.find(variable->name) != routines.end()) {
            m_errors.push_back(Error{
                .pos = variable->begin,
                .message = "A routine already exists with that name",
            });
            return;
        }
        // Any other global variable with the same name?
        for (auto definedVariable : variables) {
            if (definedVariable->name == variable->name) {
                m_errors.push_back(Error{
                    .pos = variable->begin,
                    .message = "Another variable with the same name exists",
                });
                return;
            }
        }
        variables.push_back(variable);
    }

    // TODO: do the same for types (since they can be given aliases)

    // Finally, visit all routine declaration
    for (auto routine : node->routines) {
        routine->accept(*this);
    }

    variables.clear();
    routines.clear();
}

void IdentifierResolver::visit(RoutineDecl* node) {
    // To keep track of the scope, each function that introduces a new scope
    //  will append its declarations to the `variables` vector and then remove
    //  them again before returning.
    // When searching for the closest variable declaration later on, the same
    //  vector will be iterated on from end to start

    auto oldSize = variables.size();

    for (auto parameter : node->parameters) {
        variables.push_back(parameter);
    }

    node->body->accept(*this);

    variables.resize(oldSize);
}

void IdentifierResolver::visit(Type*) {}

void IdentifierResolver::visit(AliasedType* node) {
    // TODO: find the type declaration that has the same name and link/replace
}

void IdentifierResolver::visit(PrimitiveType*) {}

void IdentifierResolver::visit(IntegerType*) {}

void IdentifierResolver::visit(RealType*) {}

void IdentifierResolver::visit(BooleanType*) {}

void IdentifierResolver::visit(ArrayType*) {}

void IdentifierResolver::visit(RecordType* node) {
    for (auto field : node->fields) {
        field->accept(*this);
    }
}

void IdentifierResolver::visit(VariableDecl* node) {
    node->type->accept(*this);
    if (node->expression != nullptr) {
        node->expression->accept(*this);
    }
}

void IdentifierResolver::visit(TypeDecl* node) { node->type->accept(*this); }

void IdentifierResolver::visit(Body* node) {
    auto oldSize = variables.size();

    for (auto variable : node->variables) {
        variables.push_back(variable);
    }

    for (auto statement : node->statements) {
        statement->accept(*this);
    }

    variables.resize(oldSize);
}

void IdentifierResolver::visit(Statement*) {}

void IdentifierResolver::visit(ReturnStatement* node) {
    node->expression->accept(*this);
}

void IdentifierResolver::visit(Assignment* node) {
    node->lhs->accept(*this);
    node->rhs->accept(*this);
}

void IdentifierResolver::visit(WhileLoop* node) {
    node->condition->accept(*this);
    node->body->accept(*this);
}

void IdentifierResolver::visit(ForLoop* node) {
    auto oldSize = variables.size();

    variables.push_back(node->loopVar);

    node->body->accept(*this);

    variables.resize(oldSize);
}

void IdentifierResolver::visit(IfStatement* node) {
    node->condition->accept(*this);
    node->ifBody->accept(*this);
    if (node->elseBody != nullptr) {
        node->elseBody->accept(*this);
    }
}

void IdentifierResolver::visit(Expression*) {}

void IdentifierResolver::visit(UnaryExpression* node) {
    node->operand->accept(*this);
}

void IdentifierResolver::visit(BinaryExpression* node) {
    node->operand1->accept(*this);
    node->operand2->accept(*this);
}

void IdentifierResolver::visit(Primary*) {}

void IdentifierResolver::visit(IntegerLiteral*) {}

void IdentifierResolver::visit(RealLiteral*) {}

void IdentifierResolver::visit(BooleanLiteral*) {}

void IdentifierResolver::visit(Identifier* node) {
    // Here comes the main work of figuring out which variable/routine this
    //  identifier refers to

    // TODO: abstract away to a private function
    for (auto it = variables.rbegin(); it != variables.rend(); it++) {
        auto variable = *it;
        if (variable->name == node->name) {
            node->variable = variable;
            node->type = *variable->type;
            return;
        }
    }

    // If it did not match any variable, it must be a parameterless routine call

    auto routineIt = routines.find(node->name);
    if (routineIt == routines.end()) {
        m_errors.push_back(Error{
            .pos = node->begin,
            .message = "Unknown identifier",
        });
        return;
    }

    auto prim = static_cast<ast::Primary*>(node);
    ast::RoutineCall call;
    call.begin = node->begin;
    call.routine = routineIt->second;
    call.routineName = node->name;
    call.end = node->end;
    *prim = call;
}

void IdentifierResolver::visit(RoutineCall* node) {
    auto routineIt = routines.find(node->routineName);
    if (routineIt == routines.end()) {
        m_errors.push_back(Error{
            .pos = node->begin,
            .message = "There is no routine with this name",
        });
        return;
    }
    node->routine = routineIt->second;
    for (auto arg : node->args) {
        arg->accept(*this);
    }
}

} // namespace visitors
