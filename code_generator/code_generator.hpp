#include "ast.hpp"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#pragma GCC diagnostic pop
#include <fstream>

namespace cg {

class CodeGenerator : public ast::Visitor<llvm::Value*> {
public:
    CodeGenerator(std::string name = "anonymous") : m_builder(m_context) {
        m_module = std::make_unique<llvm::Module>(name, m_context);
    }
    llvm::Value* visit(ast::Program* node) override;
    llvm::Value* visit(ast::RoutineDecl* node) override;
    llvm::Value* visit(ast::AliasedType* node) override;
    llvm::Value* visit(ast::IntegerType* node) override;
    llvm::Value* visit(ast::RealType* node) override;
    llvm::Value* visit(ast::BooleanType* node) override;
    llvm::Value* visit(ast::ArrayType* node) override;
    llvm::Value* visit(ast::RecordType* node) override;
    llvm::Value* visit(ast::VariableDecl* node) override;
    llvm::Value* visit(ast::TypeDecl* node) override;
    llvm::Value* visit(ast::Body* node) override;
    llvm::Value* visit(ast::ReturnStatement* node) override;
    llvm::Value* visit(ast::Assignment* node) override;
    llvm::Value* visit(ast::WhileLoop* node) override;
    llvm::Value* visit(ast::ForLoop* node) override;
    llvm::Value* visit(ast::IfStatement* node) override;
    llvm::Value* visit(ast::UnaryExpression* node) override;
    llvm::Value* visit(ast::BinaryExpression* node) override;
    llvm::Value* visit(ast::IntegerLiteral* node) override;
    llvm::ConstantFP* visit(ast::RealLiteral* node) override;
    llvm::Value* visit(ast::BooleanLiteral* node) override;
    llvm::Value* visit(ast::Identifier* node) override;
    llvm::Value* visit(ast::RoutineCall* node) override;

    void print() { m_module->print(llvm::errs(), nullptr); }

private:
    llvm::LLVMContext m_context;
    llvm::IRBuilder<> m_builder;
    std::unique_ptr<llvm::Module> m_module;
    std::map<std::string, llvm::Value*> m_namedValues;
};

} // namespace cg
