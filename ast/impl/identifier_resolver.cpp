#include "visitors.hpp"
#include <memory>
#include <set>

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
    for (auto it = types.rbegin(); it != types.rend(); it++) {
        auto typeDecl = *it;
        if (typeDecl->name == node->name) {
            toReplaceType = typeDecl->type;
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
        checkReplacementVar(node->length);
    }
    node->elementType->accept(*this);
}

void IdentifierResolver::visit(RecordType* node) {
    // ensure that no two fields have the same name
    std::set<std::string> field_names;
    for (auto field : node->fields) {
        if (field_names.find(field->name) != field_names.end()) {
            m_errors.push_back(Error{
                .pos = field->begin,
                .message = "Field name already in use",
            });
            return;
        }
        field_names.insert(field->name);
    }

    for (auto field : node->fields) {
        field->accept(*this);
    }
}

void IdentifierResolver::visit(VariableDecl* node) {
    node->type->accept(*this);
    checkReplacementType(node->type);
    if (node->initialValue != nullptr) {
        node->initialValue->accept(*this);
        checkReplacementVar(node->initialValue);
    }
}

void IdentifierResolver::visit(TypeDecl* node) { node->type->accept(*this); }

void IdentifierResolver::visit(Body* node) {
    auto oldVarsSize = variables.size();
    auto oldTypesSize = types.size();

    if (auto collision = duplicateVarExists(node->variables);
        collision != nullptr) {
        m_errors.push_back(Error{
            .pos = collision->begin,
            .message = "A variable with the same name already exists in the "
                       "same scope",
        });
        return;
    }

    for (auto& variable : node->variables) {
        variables.push_back(variable);
        if (variable->initialValue != nullptr) {
            variable->initialValue->accept(*this);
            checkReplacementVar(variable->initialValue);
        }
        if (variable->type != nullptr) {
            variable->type->accept(*this);
            checkReplacementType(variable->type);
        }
    }

    if (auto collision = duplicateTypeExists(node->types);
        collision != nullptr) {
        m_errors.push_back(Error{
            .pos = collision->begin,
            .message =
                "A type with the same name already exists in the same scope",
        });
        return;
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
    checkReplacementVar(node->expression);
}

void IdentifierResolver::visit(Assignment* node) {
    node->lhs->accept(*this);
    checkReplacementVar(node->lhs);
    node->rhs->accept(*this);
    checkReplacementVar(node->rhs);
}

void IdentifierResolver::visit(WhileLoop* node) {
    node->condition->accept(*this);
    checkReplacementVar(node->condition);
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
    checkReplacementVar(node->condition);
    node->ifBody->accept(*this);
    if (node->elseBody != nullptr) {
        node->elseBody->accept(*this);
    }
}

void IdentifierResolver::visit(Expression*) {}

void IdentifierResolver::visit(UnaryExpression* node) {
    node->operand->accept(*this);
    checkReplacementVar(node->operand);
}

void IdentifierResolver::visit(BinaryExpression* node) {
    node->operand1->accept(*this);
    checkReplacementVar(node->operand1);
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

        auto recordDecl = std::dynamic_pointer_cast<RecordType>(operand1->type);
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
                operand2->type = field->type;
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
        // TODO: Need to implement expression type evaluation for this to work
        //  for expressions like `a.b[5] := 5`
        // Or perhaps this entire type checking does not belong to this visitor?

        auto arrayDecl =
            std::dynamic_pointer_cast<ArrayType>(node->operand1->type);
        if (arrayDecl == nullptr) {
            m_errors.push_back(Error{
                .pos = node->operand1->end,
                .message = "Cannot index in a variable of non-array type",
            });
            return;
        }
        node->operand2->accept(*this);
        checkReplacementVar(node->operand2);
    } else {
        node->operand2->accept(*this);
        checkReplacementVar(node->operand2);
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
            node->type = variable->type;
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

    toReplaceVar = std::make_shared<RoutineCall>();
    toReplaceVar->begin = node->begin;
    toReplaceVar->routine = routineIt->second;
    toReplaceVar->routineName = node->name;
    toReplaceVar->end = node->end;
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
        checkReplacementVar(arg);
    }
}

/**
 * Checks if `toReplaceVar` is set, and if so changes the passed parameter to
 *  point to it instead
 */
void IdentifierResolver::checkReplacementVar(sPtr<Expression>& expr) {
    if (toReplaceVar != nullptr) {
        expr = toReplaceVar;
        toReplaceVar = nullptr;
    }
}

/**
 * Checks if `toReplaceType` is set, and if so changes the passed parameter to
 *  point to it instead
 */
void IdentifierResolver::checkReplacementType(sPtr<Type>& type) {
    if (toReplaceType != nullptr) {
        type = toReplaceType;
        toReplaceType = nullptr;
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

sPtr<VariableDecl> IdentifierResolver::duplicateVarExists(
    std::vector<sPtr<VariableDecl>> variables) {
    std::set<std::string> names;
    for (auto variable : variables) {
        if (names.find(variable->name) != names.end()) {
            return variable;
        }
        names.insert(variable->name);
    }
    return nullptr;
}
sPtr<TypeDecl>
IdentifierResolver::duplicateTypeExists(std::vector<sPtr<TypeDecl>> types) {
    std::set<std::string> names;
    for (auto type : types) {
        if (names.find(type->name) != names.end()) {
            return type;
        }
        names.insert(type->name);
    }
    return nullptr;
}

} // namespace visitors
