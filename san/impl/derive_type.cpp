#include "san.hpp"
namespace san {

using namespace ast;
// Tobe removed
void TypeDeriver::visit(Type*) {}

void TypeDeriver::visit(PrimitiveType*) {}

void TypeDeriver::visit(Statement*) {}

void TypeDeriver::visit(Expression*) {}

void TypeDeriver::visit(Primary*) {}

void TypeDeriver::visit(Program* node) {

    // check variables with both type and initial value
    for (auto var : node->variables) {
        var->accept(*this);
    }
    // to check array length type
    for (auto type : node->types) {
        type->accept(*this);
    }
    // routine has an expression
    for (auto routine : node->routines) {
        routine->accept(*this);
    }
}

void TypeDeriver::visit(RoutineDecl* node) {
    m_inRoutineParams = true;
    for (auto parameter : node->parameters) {
        parameter->accept(*this);
    }
    m_inRoutineParams = false;
    node->body->accept(*this);
}

void TypeDeriver::visit(AliasedType* node) { node->actualType->accept(*this); }

void TypeDeriver::visit(IntegerType*) {}

void TypeDeriver::visit(RealType*) {}

void TypeDeriver::visit(BooleanType*) {}

void TypeDeriver::visit(ArrayType* node) {

    if (node->length) {
        node->length->accept(*this);
        // any primitive type can be convered to the int
        if (!typeIsBase(node->length->type)) {
            error(node->begin, "invalid array length type, should be integer");
        }
    } else if (!m_inRoutineParams) {
        error(node->begin, "length of array should be always defined");
    } // length can be absent in case of routine parameter
    node->elementType->accept(*this);
    if (m_searchArray) {
        m_arrayInnerType = node->elementType;
    }
}

void TypeDeriver::visit(RecordType* node) {
    for (auto field : node->fields) {
        field->accept(*this);
        if (m_searchRecord && field->name == m_recordField) {
            m_recordInnerType = field->type;
        }
    }
}

void TypeDeriver::visit(VariableDecl* node) {

    // check type
    if (node->type != nullptr) {
        node->type->accept(*this);
    }
    // check/evaluate expression value
    if (node->initialValue != nullptr) {
        node->initialValue->accept(*this);
    }
    // assign initial value
    if (node->type == nullptr) {
        node->type = node->initialValue->type;
    }
}

void TypeDeriver::visit(TypeDecl* node) { node->type->accept(*this); }

void TypeDeriver::visit(Body* node) {

    // to check array length type
    for (auto type : node->types) {
        type->type->accept(*this);
    }
    // check variables with both type and initial value
    for (auto var : node->variables) {
        var->accept(*this);
    }
    // routine has an expression
    for (auto statement : node->statements) {
        statement->accept(*this);
    }
}

void TypeDeriver::visit(ReturnStatement* node) {
    node->expression->accept(*this);
}

void TypeDeriver::visit(Assignment* node) { node->rhs->accept(*this); }

void TypeDeriver::visit(WhileLoop* node) {
    node->condition->accept(*this);
    // check that condition is convertable to bool
    ast::TypeKind conditionType = node->condition->type->getTypeKind();
    if (conditionType != ast::TypeKind::Integer ||
        conditionType != ast::TypeKind::Boolean) {
        error(node->condition->begin,
              "type of condition should be convertable to boolean");
    }
    node->body->accept(*this);
}

void TypeDeriver::visit(ForLoop* node) {
    // check that loop var is of type int
    node->rangeFrom->accept(*this);
    if (!typeIsBase(node->rangeFrom->type)) {
        error(node->rangeFrom->begin, "invalid type of the beginning of range");
    }

    node->rangeTo->accept(*this);
    if (!typeIsBase(node->rangeTo->type)) {
        error(node->rangeTo->begin, "invalid type of the beginning of range");
    }

    node->loopVar->type = std::make_shared<ast::IntegerType>();
    node->body->accept(*this);
}

void TypeDeriver::visit(IfStatement* node) {
    node->condition->accept(*this);
    // check condition is of type boolean
    ast::TypeKind conditionType = node->condition->type->getTypeKind();
    if (conditionType != ast::TypeKind::Integer ||
        conditionType != ast::TypeKind::Boolean) {
        error(node->condition->begin,
              "type of condition should be convertable to boolean");
    }
    node->ifBody->accept(*this);
    if (node->elseBody != nullptr) {
        node->elseBody->accept(*this);
    }
}

void TypeDeriver::visit(UnaryExpression* node) {
    node->operand->accept(*this);
    node->type = node->operand->type;
}

void TypeDeriver::visit(BinaryExpression* node) {
    //  set correct type
    if (node->operation == lexer::TokenType::OpenBrack) {
        // if operation is array access
        // check that the first item is array -> not nec a[1][0]
        m_searchArray = true;
        node->operand1->accept(*this);
        if (m_arrayInnerType == nullptr) {
            error(node->operand1->begin,
                  "invalid operation [] on the given type");
        }
        m_searchArray = false;
        node->operand2->accept(*this);
        if (!typeIsBase(node->operand2->type)) {
            error(node->operand2->begin, "invalid type for array index");
        }
        node->type = m_arrayInnerType;
    } else if (node->operation == lexer::TokenType::Dot) {
        // if operation is dot ntation
        // check that the first operand is a record -???
        // take the type of key arg
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

        if (m_recordInnerType == nullptr) {
            error(node->operand1->begin,
                  "invalid operation '.' on the given type");
        }

        node->type = m_recordInnerType;

    } else if (node->operand1->type != nullptr &&
               node->operand2->type != nullptr &&
               node->operand1->type->getTypeKind() !=
                   node->operand2->type->getTypeKind()) {
        sPtr<ast::Type> type1 = node->operand1->type;
        if (!typeIsBase(type1)) {
            error(node->operand1->begin, "invalid type of expression");
        }

        sPtr<ast::Type> type2 = node->operand2->type;
        if (!typeIsBase(type2)) {
            error(node->operand2->begin, "invalid type of expression");
        }

        node->type = getGreaterType(type1, type2);
    } else {
        // types are equal
        node->type = node->operand1->type;
    }
}

void TypeDeriver::visit(IntegerLiteral*) {}

void TypeDeriver::visit(RealLiteral*) {}

void TypeDeriver::visit(BooleanLiteral*) {}

void TypeDeriver::visit(Identifier* node) {
    node->variable->accept(*this);

    if (m_searchField) {
        m_recordField = node->name;
    }
    node->type = node->variable->type;
}

// already has a type
void TypeDeriver::visit(RoutineCall*) {}

sPtr<ast::Type> TypeDeriver::getGreaterType(sPtr<ast::Type> type1,
                                            sPtr<ast::Type> type2) {
    // int  & real = real
    // bool & int  = int
    // bool & real = real
    if (type1->getTypeKind() == type2->getTypeKind()) {
        return type1; // if equal
    }
    if (type1->getTypeKind() == ast::TypeKind::Boolean) {
        return type2;
    }
    if (type2->getTypeKind() == ast::TypeKind::Boolean) {
        return type1;
    }
    if (type1->getTypeKind() == ast::TypeKind::Integer) {
        return type2; // return real
    }
    if (type2->getTypeKind() == ast::TypeKind::Integer) {
        return type1; // return real
    }
    return type1;
}
bool TypeDeriver::typeIsBase(sPtr<ast::Type> type) {
    ast::TypeKind kind = type->getTypeKind();
    return (kind == ast::TypeKind::Integer) ||
           (kind == ast::TypeKind::Boolean) || (kind == ast::TypeKind::Real);
}

// bool DeriveType::checkTypesAreEqual(sPtr<ast::Type> type1,
//                                     sPtr<ast::Type> type2) {
//     sPtr<std::vector<ast::TypeKind>> fullType1 = getFullType(type1);
//     sPtr<std::vector<ast::TypeKind>> fullType2 = getFullType(type2);
//     if (fullType1->size() != fullType2->size()) {
//         return false;
//     }
//     for (std::size_t i = 0; i < fullType1->size(); i++) {
//         if (fullType1->at(i) != fullType2->at(i)) {
//             return false;
//         }
//         if (fullType1->at(i) == ast::TypeKind::Array) {
//             // check length
//         }
//     }
// }

// sPtr<std::vector<ast::TypeKind>>
// DeriveType::getFullType(sPtr<ast::Type> type1) {
//     std::vector<ast::TypeKind> result = {type1->getTypeKind()};
//     if (result[0] == ast::TypeKind::Array) {
//         m_searchArray = true;
//         type1->accept(*this);
//         m_searchArray = false;
//         if (m_arrayInnerType) {
//             // append more deep values
//             sPtr<std::vector<ast::TypeKind>> depth =
//                 getFullType(m_arrayInnerType);
//             for (std::size_t i = 0; i < depth->size(); i++) {
//                 result.push_back(depth->at(i));
//             }
//         } else {
//             error(type1->begin, "ivalid array inner type");
//         }
//     } else if (result[0] == ast::TypeKind::Record) {
//     }
// }

} // namespace san
