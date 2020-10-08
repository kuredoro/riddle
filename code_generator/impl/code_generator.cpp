#include "code_generator.hpp"

using namespace llvm;

namespace cg {

Value* CodeGenerator::visit(ast::Program* node) {
    for (auto& routine : node->routines) {
        routine->accept(*this);
    }
}

Value* CodeGenerator::visit(ast::RoutineDecl* node) {
    // Types of the parameters
    std::vector<Type*> paramTypes;
    // for (auto& param : node->parameters) {
    //     paramTypes.push_back(param->type->accept(*this));
    // }
    // The routine's return type
    Type* returnType = Type::getVoidTy(m_context);
    if (node->returnType != nullptr) {
        // returnType = node->returnType->accept(*this);
    }

    // The function's type: (return type, parameter types, varargs?)
    FunctionType* FT = FunctionType::get(returnType, paramTypes, false);

    // The actual function from the type, local to this module, with the
    //  given name
    Function* F = Function::Create(FT, Function::InternalLinkage, node->name,
                                   m_module.get());

    // Give the parameters their names
    unsigned idx = 0;
    for (auto& arg : F->args()) {
        arg.setName(node->parameters[idx++]->name);
    }

    // Create a new basic block to start inserting into
    BasicBlock* BB = BasicBlock::Create(m_context, "entry", F);
    // Tell the builder that upcoming instructions should go into this block
    m_builder.SetInsertPoint(BB);

    // Record the function arguments in the NamedValues map
    m_namedValues.clear();
    for (auto& Arg : F->args()) {
        m_namedValues[Arg.getName()] = &Arg;
    }

    node->body->accept(*this);

    verifyFunction(*F);

    return F;
}

Value* CodeGenerator::visit(ast::AliasedType* node) {}

Value* CodeGenerator::visit(ast::IntegerType* node) {}

Value* CodeGenerator::visit(ast::RealType* node) {}

Value* CodeGenerator::visit(ast::BooleanType* node) {}

Value* CodeGenerator::visit(ast::ArrayType* node) {}

Value* CodeGenerator::visit(ast::RecordType* node) {}

Value* CodeGenerator::visit(ast::VariableDecl* node) {}

Value* CodeGenerator::visit(ast::TypeDecl* node) {}

Value* CodeGenerator::visit(ast::Body* node) {
    for (auto& statement : node->statements) {
        std::dynamic_pointer_cast<ast::ReturnStatement>(statement)->accept(
            *this);
    }
}

Value* CodeGenerator::visit(ast::ReturnStatement* node) {
    Value* returnValue = nullptr;
    if (node->expression != nullptr) {
        returnValue = node->expression->accept(*this);
    }
    return m_builder.CreateRet(returnValue);
}

Value* CodeGenerator::visit(ast::Assignment* node) {}

Value* CodeGenerator::visit(ast::WhileLoop* node) {}

Value* CodeGenerator::visit(ast::ForLoop* node) {}

Value* CodeGenerator::visit(ast::IfStatement* node) {}

Value* CodeGenerator::visit(ast::UnaryExpression* node) {}

Value* CodeGenerator::visit(ast::BinaryExpression* node) {
    auto L = node->operand1->accept(*this);
    auto R = node->operand2->accept(*this);
    if (L == nullptr || R == nullptr) {
        error(node->begin, "a binary expression needs both operands");
        return nullptr;
    }

    switch (node->operation) {
    case lexer::TokenType::Add:
        return m_builder.CreateFAdd(L, R, "addtmp");
    case lexer::TokenType::Sub:
        return m_builder.CreateFSub(L, R, "subtmp");
    case lexer::TokenType::Mul:
        return m_builder.CreateFMul(L, R, "multmp");
    case lexer::TokenType::Less:
        L = m_builder.CreateFCmpULT(L, R, "cmptmp");
        // Convert bool 0/1 to double 0.0 or 1.0
        return m_builder.CreateUIToFP(L, Type::getDoubleTy(m_context),
                                      "booltmp");
    // TODO: /, %, =, !=, ., [], <=, >, >=
    default:
        error(node->begin, "invalid binary operator");
        return nullptr;
    }
}

Value* CodeGenerator::visit(ast::IntegerLiteral* node) {
    // return ConstantInt::get(m_context, APInt(node->value));
}

ConstantFP* CodeGenerator::visit(ast::RealLiteral* node) {
    return ConstantFP::get(m_context, APFloat(node->value));
}

Value* CodeGenerator::visit(ast::BooleanLiteral* node) {}

Value* CodeGenerator::visit(ast::Identifier* node) {
    auto V = m_namedValues[node->name];
    if (V == nullptr) {
        error(node->begin, "Unknown variable name");
    }
    return V;
}

Value* CodeGenerator::visit(ast::RoutineCall* node) {
    Function* CalleeF = m_module->getFunction(node->routineName);
    if (CalleeF == nullptr) {
        error(node->begin, "unknown function referenced");
        return nullptr;
    }

    if (CalleeF->arg_size() != node->args.size()) {
        error(node->begin, "expected {} but got {} parameters",
              CalleeF->arg_size(), node->args.size());
        return nullptr;
    }

    std::vector<Value*> args;
    for (auto& arg : node->args) {
        auto argCode = arg->accept(*this);
        if (argCode == nullptr) {
            error(arg->begin, "what is this?");
            return nullptr;
        }
        args.push_back(argCode);
    }

    return m_builder.CreateCall(CalleeF, args, "calltmp");
}

} // namespace cg
