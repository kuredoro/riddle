#include "visitors.hpp"
#include <memory>
#include <set>

namespace visitors {

void IdentifierResolver::visit(Program* node) {
    // Check if anything is declared twice in global scope and visit
    // all routines.

    auto hasRedecl = [this](auto& vec, auto decl, std::string declType) {
        auto priorDecl = std::find_if(vec.begin(), vec.end(),
                [&](const auto& declPtr) {
                    return declPtr->name == decl->name &&
                           declPtr->begin < decl->begin;
                });

        if (priorDecl != vec.end()) {
            error(decl->begin, 
                    "redeclaration of {} '{}' declared at line {}",
                    declType, (*priorDecl)->name, (*priorDecl)->begin.line);
            return true;
        }

        return false;
    };

    auto isRedeclared = [node, &hasRedecl](auto decl) {
        return hasRedecl(node->routines, decl, "routine") ||
               hasRedecl(node->variables, decl, "variable") ||
               hasRedecl(node->types, decl, "type");
    };

    bool ok = true;

    for (auto routine : node->routines) {
        if (isRedeclared(routine)) {
            ok = false;
        }
        m_routines.insert({routine->name, routine});
    }

    for (auto var : node->variables) {
        if (isRedeclared(var)) {
            ok = false;
        }

        m_variables.push_back(var);
    }

    for (auto type : node->types) {
        if (isRedeclared(type)) {
            ok = false;
        }
        m_types.push_back(type);
    }

    if (!ok) {
        return;
    }

    for (auto routine : node->routines) {
        routine->accept(*this);
    }

    m_variables.clear();
    m_routines.clear();
    m_types.clear();
}

void IdentifierResolver::visit(RoutineDecl* node) {
    // To keep track of the scope, each function that introduces a new scope
    //  will append its declarations to the `m_variables` vector and then remove
    //  them again before returning.
    // When searching for the closest variable declaration later on, the same
    //  vector will be iterated on from end to start

    auto oldSize = m_variables.size();

    for (auto parameter : node->parameters) {
        m_variables.push_back(parameter);
        parameter->type->accept(*this);
    }

    node->body->accept(*this);

    m_variables.resize(oldSize);
}

void IdentifierResolver::visit(Type*) {}

void IdentifierResolver::visit(AliasedType* node) {
    for (auto it = m_types.rbegin(); it != m_types.rend(); it++) {
        auto typeDecl = *it;
        if (typeDecl->name == node->name) {
            m_toReplaceType = typeDecl->type;
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
    auto oldVarsSize = m_variables.size();
    auto oldm_typesSize = m_types.size();

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
        m_variables.push_back(variable);
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
        m_types.push_back(type);
    }

    for (auto statement : node->statements) {
        statement->accept(*this);
    }

    m_variables.resize(oldVarsSize);
    m_types.resize(oldm_typesSize);
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
    auto oldSize = m_variables.size();

    m_variables.push_back(node->loopVar);

    node->body->accept(*this);

    m_variables.resize(oldSize);
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

    auto routineIt = m_routines.find(node->name);
    if (routineIt == m_routines.end()) {
        m_errors.push_back(Error{
            .pos = node->begin,
            .message = "Unknown identifier",
        });
        return;
    }

    m_toReplaceVar = std::make_shared<RoutineCall>();
    m_toReplaceVar->begin = node->begin;
    m_toReplaceVar->routine = routineIt->second;
    m_toReplaceVar->routineName = node->name;
    m_toReplaceVar->end = node->end;
}

void IdentifierResolver::visit(RoutineCall* node) {
    auto routineIt = m_routines.find(node->routineName);
    if (routineIt == m_routines.end()) {
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
 * Checks if `m_toReplaceVar` is set, and if so changes the passed parameter to
 *  point to it instead
 */
void IdentifierResolver::checkReplacementVar(sPtr<Expression>& expr) {
    if (m_toReplaceVar != nullptr) {
        expr = m_toReplaceVar;
        m_toReplaceVar = nullptr;
    }
}

/**
 * Checks if `m_toReplaceType` is set, and if so changes the passed parameter to
 *  point to it instead
 */
void IdentifierResolver::checkReplacementType(sPtr<Type>& type) {
    if (m_toReplaceType != nullptr) {
        type = m_toReplaceType;
        m_toReplaceType = nullptr;
    }
}

sPtr<VariableDecl> IdentifierResolver::findVarDecl(std::string name) {
    for (auto it = m_variables.rbegin(); it != m_variables.rend(); it++) {
        auto variable = *it;
        if (variable->name == name) {
            return variable;
        }
    }
    return nullptr;
}

sPtr<VariableDecl> IdentifierResolver::duplicateVarExists(
    std::vector<sPtr<VariableDecl>> m_variables) {
    std::set<std::string> names;
    for (auto variable : m_variables) {
        if (names.find(variable->name) != names.end()) {
            return variable;
        }
        names.insert(variable->name);
    }
    return nullptr;
}
sPtr<TypeDecl>
IdentifierResolver::duplicateTypeExists(std::vector<sPtr<TypeDecl>> m_types) {
    std::set<std::string> names;
    for (auto type : m_types) {
        if (names.find(type->name) != names.end()) {
            return type;
        }
        names.insert(type->name);
    }
    return nullptr;
}

} // namespace visitors
