#include "code_generator.hpp"

using namespace llvm;

namespace cg {

void CodeGenerator::visit(ast::Program* node) {
    for (auto& routine : node->routines) {
        routine->accept(*this);
    }
}

void CodeGenerator::visit(ast::RoutineDecl* node) {
    // Types of the parameters
    std::vector<Type*> paramTypes;
    // for (auto& param : node->parameters) {
    //     paramTypes.push_back(param->type->accept(*this));
    // }
    // The routine's return type
    Type* returnType = Type::getVoidTy(m_context);
    if (node->returnType != nullptr) {
        // node->returnType->accept(*this);
        // extractTempVal(returnType);
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

    tempVal = F;
}

void CodeGenerator::visit(ast::AliasedType* node) {}

void CodeGenerator::visit(ast::IntegerType* node) {}

void CodeGenerator::visit(ast::RealType* node) {}

void CodeGenerator::visit(ast::BooleanType* node) {}

void CodeGenerator::visit(ast::ArrayType* node) {}

void CodeGenerator::visit(ast::RecordType* node) {}

void CodeGenerator::visit(ast::VariableDecl* node) {}

void CodeGenerator::visit(ast::TypeDecl* node) {}

void CodeGenerator::visit(ast::Body* node) {
    for (auto& statement : node->statements) {
        std::dynamic_pointer_cast<ast::ReturnStatement>(statement)->accept(
            *this);
    }
}

void CodeGenerator::visit(ast::ReturnStatement* node) {
    Value* returnValue = nullptr;
    if (node->expression != nullptr) {
        node->expression->accept(*this);
        returnValue = extractTempVal();
    }
    tempVal = m_builder.CreateRet(returnValue);
}

void CodeGenerator::visit(ast::Assignment* node) {}

void CodeGenerator::visit(ast::WhileLoop* node) {}

void CodeGenerator::visit(ast::ForLoop* node) {}

void CodeGenerator::visit(ast::IfStatement* node) {}

void CodeGenerator::visit(ast::UnaryExpression* node) {}

void CodeGenerator::visit(ast::BinaryExpression* node) {
    Value *L, *R;
    node->operand1->accept(*this);
    L = extractTempVal();

    node->operand2->accept(*this);
    R = extractTempVal();

    if (L == nullptr || R == nullptr) {
        error(node->begin, "a binary expression needs both operands");
        return;
    }

    switch (node->operation) {
    case lexer::TokenType::Add:
        tempVal = m_builder.CreateFAdd(L, R, "addtmp");
        return;
    case lexer::TokenType::Sub:
        tempVal = m_builder.CreateFSub(L, R, "subtmp");
        return;
    case lexer::TokenType::Mul:
        tempVal = m_builder.CreateFMul(L, R, "multmp");
        return;
    case lexer::TokenType::Less:
        L = m_builder.CreateFCmpULT(L, R, "cmptmp");
        // Convert bool 0/1 to double 0.0 or 1.0
        tempVal =
            m_builder.CreateUIToFP(L, Type::getDoubleTy(m_context), "booltmp");
        return;
    // TODO: /, %, =, !=, ., [], <=, >, >=
    default:
        error(node->begin, "invalid binary operator");
        return;
    }
}

void CodeGenerator::visit(ast::IntegerLiteral* node) {
    // return ConstantInt::get(m_context, APInt(node->value));
}

void CodeGenerator::visit(ast::RealLiteral* node) {
    tempVal = ConstantFP::get(m_context, APFloat(node->value));
}

void CodeGenerator::visit(ast::BooleanLiteral* node) {}

void CodeGenerator::visit(ast::Identifier* node) {
    auto V = m_namedValues[node->name];
    if (V == nullptr) {
        error(node->begin, "Unknown variable name");
    }
    tempVal = V;
}

void CodeGenerator::visit(ast::RoutineCall* node) {
    Function* CalleeF = m_module->getFunction(node->routineName);
    if (CalleeF == nullptr) {
        error(node->begin, "unknown function referenced");
        return;
    }

    if (CalleeF->arg_size() != node->args.size()) {
        error(node->begin, "expected {} but got {} parameters",
              CalleeF->arg_size(), node->args.size());
        return;
    }

    std::vector<Value*> args;
    for (auto& arg : node->args) {

        arg->accept(*this);
        Value* argCode = extractTempVal();
        if (argCode == nullptr) {
            error(arg->begin, "what is this?");
            return;
        }
        args.push_back(argCode);
    }

    tempVal = m_builder.CreateCall(CalleeF, args, "calltmp");
}

} // namespace cg
