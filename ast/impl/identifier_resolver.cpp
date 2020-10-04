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

    for (auto typeDecl : node->types) {
        types.push_back(typeDecl);
    }

    // Finally, visit all routine declaration
    for (auto routine : node->routines) {
        routine->accept(*this);
    }

    variables.clear();
    routines.clear();
    types.clear();
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
        parameter->type->accept(*this);
    }

    node->body->accept(*this);

    variables.resize(oldSize);
}

void IdentifierResolver::visit(Type*) {}

void IdentifierResolver::visit(AliasedType* node) {
    for (auto typeDecl : types) {
        if (typeDecl->name == node->name) {
            node->actualType = typeDecl->type;
            return;
        }
    }
    m_errors.push_back(Error{
        .pos = node->begin,
        .message = fmt::format("{} does not name a type", node->name),
    });
}

void IdentifierResolver::visit(PrimitiveType*) {}

void IdentifierResolver::visit(IntegerType*) {}

void IdentifierResolver::visit(RealType*) {}

void IdentifierResolver::visit(BooleanType*) {}

void IdentifierResolver::visit(ArrayType* node) {
    if (node->length != nullptr) {
        node->length->accept(*this);
        checkReplacement(node->length);
    }
    node->elementType->accept(*this);
}

void IdentifierResolver::visit(RecordType* node) {
    for (auto field : node->fields) {
        field->accept(*this);
    }
}

void IdentifierResolver::visit(VariableDecl* node) {
    node->type->accept(*this);
    if (node->initialValue != nullptr) {
        node->initialValue->accept(*this);
        checkReplacement(node->initialValue);
    }
}

void IdentifierResolver::visit(TypeDecl* node) { node->type->accept(*this); }

void IdentifierResolver::visit(Body* node) {
    auto oldVarsSize = variables.size();
    auto oldTypesSize = types.size();

    for (auto& variable : node->variables) {
        variables.push_back(variable);
        if (variable->initialValue != nullptr) {
            variable->initialValue->accept(*this);
            checkReplacement(variable->initialValue);
        }
    }
    for (auto type : node->types) {
        type->accept(*this);
        types.push_back(type);
    }
    for (auto statement : node->statements) {
        statement->accept(*this);
    }

    variables.resize(oldVarsSize);
    types.resize(oldTypesSize);
}

void IdentifierResolver::visit(Statement*) {}

void IdentifierResolver::visit(ReturnStatement* node) {
    node->expression->accept(*this);
    checkReplacement(node->expression);
}

void IdentifierResolver::visit(Assignment* node) {
    node->lhs->accept(*this);
    checkReplacement(node->lhs);
    node->rhs->accept(*this);
    checkReplacement(node->rhs);
}

void IdentifierResolver::visit(WhileLoop* node) {
    node->condition->accept(*this);
    checkReplacement(node->condition);
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
    checkReplacement(node->condition);
    node->ifBody->accept(*this);
    if (node->elseBody != nullptr) {
        node->elseBody->accept(*this);
    }
}

void IdentifierResolver::visit(Expression*) {}

void IdentifierResolver::visit(UnaryExpression* node) {
    node->operand->accept(*this);
    checkReplacement(node->operand);
}

void IdentifierResolver::visit(BinaryExpression* node) {
    node->operand1->accept(*this);
    checkReplacement(node->operand1);
    if (node->operation == lexer::TokenType::Dot) {
        auto operand1 = std::dynamic_pointer_cast<Identifier>(node->operand1);
        if (operand1 == nullptr) {
            m_errors.push_back(Error{
                .pos = node->operand1->begin,
                .message = "Expected variable identifier before '.'",
            });
            return;
        }

        auto operand2 = std::dynamic_pointer_cast<Identifier>(node->operand2);
        if (operand2 == nullptr) {
            m_errors.push_back(Error{
                .pos = node->operand2->begin,
                .message = "Expected identifier after '.'",
            });
            return;
        }

        auto recordDecl = dynamic_cast<RecordType*>(&operand1->type);
        if (recordDecl == nullptr) {
            m_errors.push_back(Error{
                .pos = node->operand2->begin,
                .message = "Can only access member of records",
            });
            return;
        }
        for (auto field : recordDecl->fields) {
            if (field->name == operand2->name) {
                operand2->variable = field;
                operand2->type = *field->type;
                break;
            }
        }
        if (operand2->variable == nullptr) {
            m_errors.push_back(Error{
                .pos = operand2->begin,
                .message = fmt::format("{} is not a member of {}",
                                       operand2->name, operand1->name),
            });
            return;
        }
    } else if (node->operation == lexer::TokenType::OpenBrack) {
        auto operand1 = std::dynamic_pointer_cast<Identifier>(node->operand1);
        if (operand1 == nullptr) {
            m_errors.push_back(Error{
                .pos = node->operand1->begin,
                .message = "Expected variable identifier before '['",
            });
            return;
        }
        auto arrayDecl = dynamic_cast<ArrayType*>(&operand1->type);
        if (arrayDecl == nullptr) {
            m_errors.push_back(Error{
                .pos = node->operand1->begin,
                .message = "Cannot index in a variable of non-array type",
            });
            return;
        }
        node->operand2->accept(*this);
        checkReplacement(node->operand2);
    } else {
        node->operand2->accept(*this);
        checkReplacement(node->operand2);
    }
}

void IdentifierResolver::visit(Primary*) {}

void IdentifierResolver::visit(IntegerLiteral*) {}

void IdentifierResolver::visit(RealLiteral*) {}

void IdentifierResolver::visit(BooleanLiteral*) {}

void IdentifierResolver::visit(Identifier* node) {
    // Here comes the main work of figuring out which variable/routine this
    //  identifier refers to

    auto variable = findVarDecl(node->name);
    if (variable != nullptr) {
        node->variable = variable;
        if (variable->type != nullptr) {
            node->type = *variable->type;
        }
        return;
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

    toReplace = std::make_shared<RoutineCall>();
    toReplace->begin = node->begin;
    toReplace->routine = routineIt->second;
    toReplace->routineName = node->name;
    toReplace->end = node->end;
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
    for (auto& arg : node->args) {

        arg->accept(*this);
        checkReplacement(arg);
    }
}

/**
 * Checks if `toReplace` is set, and if so changes the passed parameter to point
 *  to it instead
 */
void IdentifierResolver::checkReplacement(sPtr<Expression>& expr) {
    if (toReplace != nullptr) {
        expr = toReplace;
        toReplace = nullptr;
    }
}

sPtr<VariableDecl> IdentifierResolver::findVarDecl(std::string name) {
    for (auto it = variables.rbegin(); it != variables.rend(); it++) {
        auto variable = *it;
        if (variable->name == name) {
            return variable;
        }
    }
    return nullptr;
}

} // namespace visitors
