#include "visitors.hpp"

std::unordered_map<lexer::TokenType, std::string> op_to_string{
    {lexer::TokenType::Add, "+"},     {lexer::TokenType::Sub, "-"},
    {lexer::TokenType::Mul, "*"},     {lexer::TokenType::Div, "/"},
    {lexer::TokenType::Mod, "%"},     {lexer::TokenType::Or, "or"},
    {lexer::TokenType::Xor, "xor"},   {lexer::TokenType::And, "and"},
    {lexer::TokenType::Eq, "="},      {lexer::TokenType::Neq, "/="},
    {lexer::TokenType::Less, "<"},    {lexer::TokenType::Leq, "<="},
    {lexer::TokenType::Greater, ">"}, {lexer::TokenType::Geq, ">="},
    {lexer::TokenType::Not, "not"},   {lexer::TokenType::OpenParen, "()"},
    {lexer::TokenType::Dot, "."},     {lexer::TokenType::OpenBrack, "[]"},
};

void PrintVisitor::visit(Program* node) {
    if (node == nullptr) {
        fmt::print("{:|>{}}- [Program]> null\n", "", depth);
        return;
    }
    fmt::print("{:|>{}}- [Program]>\n", "", depth);
    depth++;
    for (auto type : node->types) {
        type->accept(*this);
    }
    for (auto variableDecl : node->variables) {
        variableDecl->accept(*this);
    }
    for (auto routine : node->routines) {
        routine->accept(*this);
    }
    depth--;
}
void PrintVisitor::visit(RoutineDecl* node) {
    if (node == nullptr) {
        fmt::print("{:|>{}}- [RoutineDecl]> null\n", "", depth);
        return;
    }
    fmt::print("{:|>{}}- [RoutineDecl]> {}\n", "", depth, node->name);
    depth++;
    for (auto parameter : node->parameters) {
        parameter->accept(*this);
    }
    node->body->accept(*this);
    if (node->returnType != nullptr) {
        node->returnType->accept(*this);
    }
    depth--;
}
void PrintVisitor::visit(Parameter* node) {
    if (node == nullptr) {
        fmt::print("{:|>{}}- [Parameter]> null\n", "", depth);
        return;
    }
    fmt::print("{:|>{}}- [Parameter]> {}\n", "", depth, node->name);
    depth++;
    node->type->accept(*this);
    depth--;
}
void PrintVisitor::visit(Type* node) {
    if (ast::PrimitiveType* specific = dynamic_cast<ast::PrimitiveType*>(node);
        specific != nullptr) {
        specific->accept(*this);
    } else if (ast::ArrayType* specific = dynamic_cast<ast::ArrayType*>(node);
               specific != nullptr) {
        specific->accept(*this);
    } else if (ast::RecordType* specific = dynamic_cast<ast::RecordType*>(node);
               specific != nullptr) {
        specific->accept(*this);
    } else if (ast::AliasedType* specific =
                   dynamic_cast<ast::AliasedType*>(node);
               specific != nullptr) {
        fmt::print("{:|>{}}- [Type Identifier]> {}\n", "", depth,
                   specific->name);
    } else {
        fmt::print("{:|>{}}- [Type]> (unknown type)\n", "", depth);
    }
}
void PrintVisitor::visit(PrimitiveType* node) {
    if (node == nullptr) {
        fmt::print("{:|>{}}- [PrimitiveType]> null\n", "", depth);
        return;
    }
    fmt::print("{:|>{}}- [PrimitiveType]> unknown\n", "", depth);
}
void PrintVisitor::visit(IntegerType* node) {
    if (node == nullptr) {
        fmt::print("{:|>{}}- [IntegerType]> null\n", "", depth);
        return;
    }
    fmt::print("{:|>{}}- [IntegerType]\n", "", depth);
}
void PrintVisitor::visit(RealType* node) {
    if (node == nullptr) {
        fmt::print("{:|>{}}- [RealType]> null\n", "", depth);
        return;
    }
    fmt::print("{:|>{}}- [RealType]\n", "", depth);
}
void PrintVisitor::visit(BooleanType* node) {
    if (node == nullptr) {
        fmt::print("{:|>{}}- [BooleanType]> null\n", "", depth);
        return;
    }
    fmt::print("{:|>{}}- [BooleanType]\n", "", depth);
}
void PrintVisitor::visit(ArrayType* node) {
    if (node == nullptr) {
        fmt::print("{:|>{}}- [ArrayType]> null\n", "", depth);
        return;
    }
    fmt::print("{:|>{}}- [ArrayType]>\n", "", depth);
    depth++;
    if (node->length != nullptr) {
        node->length->accept(*this);
    }
    if (node->elementType != nullptr) {
        node->elementType->accept(*this);
    }
    depth--;
}
void PrintVisitor::visit(RecordType* node) {
    if (node == nullptr) {
        fmt::print("{:|>{}}- [RecordType]> null\n", "", depth);
        return;
    }
    fmt::print("{:|>{}}- [RecordType]>\n", "", depth);
    depth++;
    for (auto field : node->fields) {
        field->accept(*this);
    }
    depth--;
}
void PrintVisitor::visit(VariableDecl* node) {
    if (node == nullptr) {
        fmt::print("{:|>{}}- [VariableDecl]> null\n", "", depth);
    }
    fmt::print("{:|>{}}- [VariableDecl]> {}\n", "", depth, node->name);
    depth++;
    if (node->type != nullptr) {
        node->type->accept(*this);
    }
    if (node->expression != nullptr) {
        node->expression->accept(*this);
    }
    depth--;
}
void PrintVisitor::visit(TypeDecl* node) {
    if (node == nullptr) {
        fmt::print("{:|>{}}- [TypeDecl]> null\n", "", depth);
    }
    fmt::print("{:|>{}}- [TypeDecl]> {}\n", "", depth, node->name);
    depth++;
    if (node->type != nullptr) {
        node->type->accept(*this);
    }
    depth--;
}
void PrintVisitor::visit(Body* node) {
    if (node == nullptr) {
        fmt::print("{:|>{}}- [Body]> null\n", "", depth);
        return;
    }
    fmt::print("{:|>{}}- [Body]>\n", "", depth);
    depth++;
    for (auto type : node->types) {
        type->accept(*this);
    }
    for (auto variableDecl : node->variables) {
        variableDecl->accept(*this);
    }
    for (auto statement : node->statements) {
        statement->accept(*this);
    }
    depth--;
}
void PrintVisitor::visit(Statement* node) {
    if (node == nullptr) {
        fmt::print("{:|>{}}- [Statement]> null\n", "", depth);
        return;
    }
    fmt::print("{:|>{}}- [Statement]> Unknown\n", "", depth);
}
void PrintVisitor::visit(ReturnStatement* node) {
    if (node == nullptr) {
        fmt::print("{:|>{}}- [Return]> null\n", "", depth);
        return;
    }
    fmt::print("{:|>{}}- [Return]>\n", "", depth);
    depth++;
    if (node->expression != nullptr) {
        node->expression->accept(*this);
    }
    depth--;
}
void PrintVisitor::visit(Assignment* node) {
    if (node == nullptr) {
        fmt::print("{:|>{}}- [Assignment]> null\n", "", depth);
        return;
    }
    fmt::print("{:|>{}}- [Assignment]>\n", "", depth);
    depth++;
    node->lhs->accept(*this);
    node->rhs->accept(*this);
    depth--;
}
void PrintVisitor::visit(WhileLoop* node) {
    if (node == nullptr) {
        fmt::print("{:|>{}}- [WhileLoop]> null\n", "", depth);
        return;
    }
    fmt::print("{:|>{}}- [WhileLoop]>\n", "", depth);
    depth++;
    node->condition->accept(*this);
    node->body->accept(*this);
    depth--;
}
void PrintVisitor::visit(ForLoop* node) {
    if (node == nullptr) {
        fmt::print("{:|>{}}- [ForLoop]> null\n", "", depth);
        return;
    }
    fmt::print("{:|>{}}- [ForLoop]>\n", "", depth);
    depth++;
    // node->loopVar->accept(*this);
    node->rangeFrom->accept(*this);
    node->rangeTo->accept(*this);
    node->body->accept(*this);
    depth--;
}
void PrintVisitor::visit(IfStatement* node) {
    if (node == nullptr) {
        fmt::print("{:|>{}}- [IfStatement]> null\n", "", depth);
        return;
    }
    fmt::print("{:|>{}}- [IfStatement]>\n", "", depth);
    depth++;
    node->condition->accept(*this);
    node->ifBody->accept(*this);
    if (node->elseBody) {
        node->elseBody->accept(*this);
    }
    depth--;
}
void PrintVisitor::visit(Expression* node) {
    if (node == nullptr) {
        fmt::print("{:|>{}}- [Expression]> null\n", "", depth);
        return;
    }
    fmt::print("{:|>{}}- [Expression]>\n", "", depth);
}
void PrintVisitor::visit(UnaryExpression* node) {
    if (node == nullptr) {
        fmt::print("{:|>{}}- [UnaryExpression]> null\n", "", depth);
        return;
    }
    fmt::print("{:|>{}}- [UnaryExpression]> {}\n", "", depth,
               op_to_string[node->operation]);
    depth++;
    node->operand->accept(*this);
    depth--;
}
void PrintVisitor::visit(BinaryExpression* node) {
    if (node == nullptr) {
        fmt::print("{:|>{}}- [BinaryExpression]> null\n", "", depth);
        return;
    }
    fmt::print("{:|>{}}- [BinaryExpression]> {}\n", "", depth,
               op_to_string[node->operation]);
    depth++;
    node->operand1->accept(*this);
    node->operand2->accept(*this);
    depth--;
}
void PrintVisitor::visit(Primary* node) {
    if (node == nullptr) {
        fmt::print("{:|>{}}- [Primary]> null\n", "", depth);
        return;
    }
    fmt::print("{:|>{}}- [Primary]> unknown\n", "", depth);
}
void PrintVisitor::visit(IntegerLiteral* node) {
    if (node == nullptr) {
        fmt::print("{:|>{}}- [IntegerLiteral]> null\n", "", depth);
        return;
    }
    fmt::print("{:|>{}}- [IntegerLiteral]> {}\n", "", depth, node->value);
}
void PrintVisitor::visit(RealLiteral* node) {
    if (node == nullptr) {
        fmt::print("{:|>{}}- [RealLiteral]> null\n", "", depth);
        return;
    }
    fmt::print("{:|>{}}- [RealLiteral]> {}\n", "", depth, node->value);
}
void PrintVisitor::visit(BooleanLiteral* node) {
    if (node == nullptr) {
        fmt::print("{:|>{}}- [BooleanLiteral]> null\n", "", depth);
        return;
    }
    fmt::print("{:|>{}}- [BooleanLiteral]> {}\n", "", depth, node->value);
}
void PrintVisitor::visit(Identifier* node) {
    if (node == nullptr) {
        fmt::print("{:|>{}}- [Identifier]> null\n", "", depth);
        return;
    }
    fmt::print("{:|>{}}- [Identifier]> {}\n", "", depth, node->name);
}
void PrintVisitor::visit(RoutineCall* node) {
    if (node == nullptr) {
        fmt::print("{:|>{}}- [RoutineCall]> null\n", "", depth);
        return;
    }
    fmt::print("{:|>{}}- [RoutineCall]> {}\n", "", depth, node->routineName);
    depth++;
    for (size_t i = 0; i < node->args.size(); i++) {
        node->args[i]->accept(*this);
    }
    depth--;
}
