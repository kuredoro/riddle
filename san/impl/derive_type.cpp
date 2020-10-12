#include "san.hpp"

using namespace ast;

namespace san {

void TypeDeriver::visit(Program* node) {
    // check variables with both type and initial value
    for (auto& var : node->variables) {
        var->accept(*this);
    }
    // to check array length type
    for (auto& type : node->types) {
        type->accept(*this);
    }
    // routine has an expression
    for (auto& routine : node->routines) {
        routine->accept(*this);
    }
}

void TypeDeriver::visit(RoutineDecl* node) {
    m_inRoutineParams = true;
    for (auto& parameter : node->parameters) {
        parameter->accept(*this);
    }
    m_inRoutineParams = false;
    // Return type might be an array with defined length
    if (node->returnType != nullptr) {
        node->returnType->accept(*this);
    }
    node->body->accept(*this);
}

void TypeDeriver::visit(AliasedType* node) { node->actualType->accept(*this); }

void TypeDeriver::visit(IntegerType*) {}

void TypeDeriver::visit(RealType*) {}

void TypeDeriver::visit(BooleanType*) {}

void TypeDeriver::visit(ArrayType* node) {
    if (node->length != nullptr) {
        node->length->accept(*this);
        // any primitive type can be converted to int
        if (!typeIsPrimitive(node->length->type)) {
            error(node->begin, "invalid array length type, should be integer");
        }
    } else if (!m_inRoutineParams && !m_searchArray) {
        error(node->begin, "length of array should be always defined");
    } // length can be absent in case of routine parameter

    node->elementType->accept(*this);
    if (m_searchArray) {
        m_arrayInnerType = node->elementType;
    }
}

void TypeDeriver::visit(RecordType* node) {
    for (auto& field : node->fields) {
        field->accept(*this);
        if (m_searchRecord && field->name == m_recordField) {
            m_recordInnerType = field->type;
        }
    }
}

void TypeDeriver::visit(VariableDecl* node) {
    // validation
    if (node->type == nullptr && node->initialValue == nullptr) {
        error(node->begin, "invalid defnition of variable");
    }

    if (node->type != nullptr) {
        node->type->accept(*this);
        if (node->initialValue != nullptr) {
            node->initialValue->accept(*this);
            // checkTypesAreEqual or conforming:
            // initial value can be only of primitive type
            // the only nonconforming case is during attempt to cast real to
            // bool
            if (node->type->getTypeKind() == TypeKind::Boolean &&
                !typeIsBooleanconvertible(node->initialValue->type)) {
                error(node->begin,
                      "invalid combination of initial type and initial value");
            }
        }
    } else if (node->initialValue != nullptr) {
        node->initialValue->accept(*this);
        node->type = node->initialValue->type;
    } else {
        error(node->begin, "invalid variable declaration");
    }
}

void TypeDeriver::visit(TypeDecl* node) { node->type->accept(*this); }

void TypeDeriver::visit(Body* node) {
    for (auto& type : node->types) {
        type->accept(*this);
    }
    for (auto& var : node->variables) {
        var->accept(*this);
    }
    for (auto& statement : node->statements) {
        statement->accept(*this);
    }
}

void TypeDeriver::visit(ReturnStatement* node) {
    node->expression->accept(*this);
}

void TypeDeriver::visit(Assignment* node) {
    node->rhs->accept(*this);
    node->lhs->accept(*this);
    // check the type conformance
    if (node->lhs->type->getTypeKind() == TypeKind::Boolean &&
        !typeIsBooleanconvertible(node->rhs->type)) {
        error(node->begin, "type of rhs is not convertible to the type of lhs");
    }
}

void TypeDeriver::visit(WhileLoop* node) {
    node->condition->accept(*this);
    // check that condition is convertible to bool
    if (!typeIsBooleanconvertible(node->condition->type)) {
        error(node->condition->begin,
              "type of condition should be convertible to boolean");
    }
    node->body->accept(*this);
}

void TypeDeriver::visit(ForLoop* node) {
    // check that loop var is of type int
    node->rangeFrom->accept(*this);
    if (!typeIsPrimitive(node->rangeFrom->type)) {
        error(node->rangeFrom->begin, "invalid type of the beginning of range");
    }

    node->rangeTo->accept(*this);
    if (!typeIsPrimitive(node->rangeTo->type)) {
        error(node->rangeTo->begin, "invalid type of the end of range");
    }

    node->body->accept(*this);
}

void TypeDeriver::visit(IfStatement* node) {
    node->condition->accept(*this);
    // check condition is of type boolean
    if (!typeIsBooleanconvertible(node->condition->type)) {
        error(node->condition->begin,
              "type of condition should be convertible to boolean");
    }
    node->ifBody->accept(*this);
    if (node->elseBody != nullptr) {
        node->elseBody->accept(*this);
    }
}

void TypeDeriver::visit(UnaryExpression* node) {
    node->operand->accept(*this);
    if (node->operation == lexer::TokenType::Not) {
        if (!typeIsBooleanconvertible(node->operand->type)) {
            error(
                node->operand->begin,
                "operand of 'not' operation should be convertible to boolean");
        }
        node->type = std::make_shared<BooleanType>();
    } else {
        node->type = node->operand->type;
    }
}

void TypeDeriver::visit(BinaryExpression* node) {
    //  set correct type
    if (node->operation == lexer::TokenType::OpenBrack) {
        // if operation is array access
        // find the type of array
        m_searchArray = true;
        node->operand1->accept(*this);
        if (m_arrayInnerType == nullptr) {
            error(node->operand1->begin,
                  "invalid operation [] on the given type");
        }
        m_searchArray = false;
        node->operand2->accept(*this);
        if (!typeIsPrimitive(node->operand2->type)) {
            error(node->operand2->begin, "invalid type for array index");
        }
        node->type = m_arrayInnerType;
    } else if (node->operation == lexer::TokenType::Dot) {
        // if operation is dot notation
        // find record field name
        m_searchField = true;
        node->operand2->accept(*this);
        m_searchField = false;
        if (m_recordField == "") {
            error(node->operand2->begin, "field name of record not found");
        }
        // search for the type of key arg
        m_searchRecord = true;
        node->operand1->accept(*this);
        m_searchRecord = false;
        m_recordField = "";

        if (m_recordInnerType == nullptr) {
            error(node->operand1->begin,
                  "invalid operation '.' on the given type");
        }
        node->type = m_recordInnerType;
        m_recordInnerType = nullptr;
    } else if (node->operation == lexer::TokenType::Or ||
               node->operation == lexer::TokenType::Xor ||
               node->operation == lexer::TokenType::And) {
        // lhs and rhs should be convertible to boolean
        node->operand1->accept(*this);

        if (!typeIsBooleanconvertible(node->operand1->type)) {
            error(node->operand1->begin,
                  "lhs of logical operation should be convertible to boolean");
        }

        node->operand2->accept(*this);

        if (!typeIsBooleanconvertible(node->operand2->type)) {
            error(node->operand2->begin,
                  "rhs of logical operation should be convertible to boolean");
        }

        node->type = std::make_shared<BooleanType>();
    } else if (node->operation == lexer::TokenType::Eq ||
               node->operation == lexer::TokenType::Neq ||
               node->operation == lexer::TokenType::Leq ||
               node->operation == lexer::TokenType::Geq ||
               node->operation == lexer::TokenType::Less ||
               node->operation == lexer::TokenType::Greater) {
        // the result of comparisons is always boolean
        node->operand1->accept(*this);
        if (!typeIsPrimitive(node->operand1->type)) {
            error(node->operand1->begin,
                  "invalid type for comparison operation");
        }
        node->operand2->accept(*this);
        if (!typeIsPrimitive(node->operand2->type)) {
            error(node->operand2->begin,
                  "invalid type for comparison operation");
        }
        node->type = std::make_shared<BooleanType>();

    } else {
        node->operand1->accept(*this);
        node->operand2->accept(*this);
        if (node->operand1->type->getTypeKind() !=
            node->operand2->type->getTypeKind()) {
            sPtr<Type> type1 = node->operand1->type;
            if (!typeIsPrimitive(type1)) {
                error(node->operand1->begin, "invalid type of expression");
            }

            sPtr<Type> type2 = node->operand2->type;
            if (!typeIsPrimitive(type2)) {
                error(node->operand2->begin, "invalid type of expression");
            }

            node->type = getGreaterType(type1, type2);
        } else {
            // types are equal
            node->type = node->operand1->type;
        }
    }
}

void TypeDeriver::visit(IntegerLiteral*) {}

void TypeDeriver::visit(RealLiteral*) {}

void TypeDeriver::visit(BooleanLiteral*) {}

void TypeDeriver::visit(Identifier* node) {
    // field name is an identifier, but not linked to a variable
    if (node->variable != nullptr) {
        node->variable->accept(*this);
        node->type = node->variable->type;
    }

    if (m_searchField) {
        m_recordField = node->name;
    }
}

void TypeDeriver::visit(RoutineCall* node) {
    for (auto& arg : node->args) {
        arg->accept(*this);
    }
}

sPtr<Type> TypeDeriver::getGreaterType(sPtr<Type> type1, sPtr<Type> type2) {
    // int  & real = real
    // bool & int  = int
    // bool & real = real
    if (type1->getTypeKind() == type2->getTypeKind()) {
        return type1; // if equal
    }
    if (type1->getTypeKind() == TypeKind::Boolean) {
        return type2;
    }
    if (type2->getTypeKind() == TypeKind::Boolean) {
        return type1;
    }
    if (type1->getTypeKind() == TypeKind::Integer) {
        return type2; // return real
    }
    if (type2->getTypeKind() == TypeKind::Integer) {
        return type1; // return real
    }
    return type1;
}
bool TypeDeriver::typeIsPrimitive(sPtr<Type> type) {
    TypeKind kind = type->getTypeKind();
    return (kind == TypeKind::Integer) || (kind == TypeKind::Boolean) ||
           (kind == TypeKind::Real);
}
bool TypeDeriver::typeIsBooleanconvertible(sPtr<Type> type) {
    TypeKind conditionType = type->getTypeKind();
    return conditionType == TypeKind::Integer ||
           conditionType == TypeKind::Boolean;
}

// bool DeriveType::checkTypesAreEqual(sPtr<Type> type1,
//                                     sPtr<Type> type2) {
//     sPtr<std::vector<TypeKind>> fullType1 = getFullType(type1);
//     sPtr<std::vector<TypeKind>> fullType2 = getFullType(type2);
//     if (fullType1->size() != fullType2->size()) {
//         return false;
//     }
//     for (std::size_t i = 0; i < fullType1->size(); i++) {
//         if (fullType1->at(i) != fullType2->at(i)) {
//             return false;
//         }
//         if (fullType1->at(i) == TypeKind::Array) {
//             // check length
//         }
//     }
// }

// sPtr<std::vector<TypeKind>>
// DeriveType::getFullType(sPtr<Type> type1) {
//     std::vector<TypeKind> result = {type1->getTypeKind()};
//     if (result[0] == TypeKind::Array) {
//         m_searchArray = true;
//         type1->accept(*this);
//         m_searchArray = false;
//         if (m_arrayInnerType) {
//             // append more deep values
//             sPtr<std::vector<TypeKind>> depth =
//                 getFullType(m_arrayInnerType);
//             for (std::size_t i = 0; i < depth->size(); i++) {
//                 result.push_back(depth->at(i));
//             }
//         } else {
//             error(type1->begin, "ivalid array inner type");
//         }
//     } else if (result[0] == TypeKind::Record) {
//     }
// }

} // namespace san
