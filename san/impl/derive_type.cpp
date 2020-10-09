#include <iostream>

#include "san.hpp"
namespace san {

using namespace ast;

/**
 * This visitor is responsibel for
 * - detection of possibility of type converges
 * - determining of the resulted type of the expression
 * - check, that array index, range thresholds are integers
 * - check that dot is called only on structures
 */
void DeriveType::visit(Program* node) {

    // check variables with both type and initial value
    for (auto var : node->variables) {
        var->accept(*this);
    }
    // to check array length type
    for (auto type : node->types) {
        type->type->accept(*this);
    }
    // routine has an expression
    for (auto routine : node->routines) {
        routine->accept(*this);
    }
}

void DeriveType::visit(RoutineDecl* node) {
    for (auto parameter : node->parameters) {
        parameter->accept(*this);
    }
    node->body->accept(*this);
}

void DeriveType::visit(Type*) {}

void DeriveType::visit(AliasedType* node) { node->actualType->accept(*this); }

void DeriveType::visit(PrimitiveType*) {}

void DeriveType::visit(IntegerType*) {}

void DeriveType::visit(RealType*) {}

void DeriveType::visit(BooleanType*) {}

void DeriveType::visit(ArrayType* node) {

    node->length->accept(*this);

    // any primitive type can be convered to the int
    // if (!typeIsBase(node->length->type)) {
    //     error(node->begin, "invalid array length type, should be integer");
    // }

    node->elementType->accept(*this);
    if (m_searchArray) {
        m_arrayInnerType = node->elementType;
    }
}

void DeriveType::visit(RecordType* node) {
    for (auto field : node->fields) {
        field->accept(*this);
        if (m_searchRecord && field->name == m_recordField) {
            m_recordInnerType = field->type;
        }
    }
}

void DeriveType::visit(VariableDecl* node) {

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

void DeriveType::visit(TypeDecl* node) { node->type->accept(*this); }

void DeriveType::visit(Body* node) {

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

void DeriveType::visit(Statement*) {}

void DeriveType::visit(ReturnStatement*) {}

void DeriveType::visit(Assignment* node) { node->rhs->accept(*this); }

void DeriveType::visit(WhileLoop* node) {
    node->condition->accept(*this);
    node->body->accept(*this);
}

void DeriveType::visit(ForLoop* node) {
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

void DeriveType::visit(IfStatement* node) {
    node->condition->accept(*this);
    // check condition is of type boolean
    node->ifBody->accept(*this);
    if (node->elseBody != nullptr) {
        node->elseBody->accept(*this);
    }
}

void DeriveType::visit(Expression*) {}

void DeriveType::visit(UnaryExpression* node) {
    node->operand->accept(*this);
    node->type = node->operand->type;
}

void DeriveType::visit(BinaryExpression* node) {
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
        m_searchFiled = true;
        node->operand2->accept(*this);
        m_searchFiled = false;
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

void DeriveType::visit(Primary*) {}

void DeriveType::visit(IntegerLiteral*) {}

void DeriveType::visit(RealLiteral*) {}

void DeriveType::visit(BooleanLiteral*) {}

void DeriveType::visit(Identifier* node) {
    node->variable->accept(*this);

    if (m_searchFiled) {
        m_recordField = node->name;
    }
    node->type = node->variable->type;
}

// already has a type
void DeriveType::visit(RoutineCall*) {}

sPtr<ast::Type> DeriveType::getGreaterType(sPtr<ast::Type> type1,
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
bool DeriveType::typeIsBase(sPtr<ast::Type> type) {
    ast::TypeKind kind = type->getTypeKind();
    return (kind == ast::TypeKind::Integer) ||
           (kind == ast::TypeKind::Boolean) || (kind == ast::TypeKind::Real);
}

} // namespace san
