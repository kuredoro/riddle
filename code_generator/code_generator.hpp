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

class CodeGenerator : public ast::Visitor {
public:
    CodeGenerator(std::string name = "anonymous") : m_builder(m_context) {
        m_module = std::make_unique<llvm::Module>(name, m_context);
    }
    void visit(ast::Program* node) override;
    void visit(ast::RoutineDecl* node) override;
    void visit(ast::AliasedType* node) override;
    void visit(ast::IntegerType* node) override;
    void visit(ast::RealType* node) override;
    void visit(ast::BooleanType* node) override;
    void visit(ast::ArrayType* node) override;
    void visit(ast::RecordType* node) override;
    void visit(ast::VariableDecl* node) override;
    void visit(ast::TypeDecl* node) override;
    void visit(ast::Body* node) override;
    void visit(ast::ReturnStatement* node) override;
    void visit(ast::Assignment* node) override;
    void visit(ast::WhileLoop* node) override;
    void visit(ast::ForLoop* node) override;
    void visit(ast::IfStatement* node) override;
    void visit(ast::UnaryExpression* node) override;
    void visit(ast::BinaryExpression* node) override;
    void visit(ast::IntegerLiteral* node) override;
    void visit(ast::RealLiteral* node) override;
    void visit(ast::BooleanLiteral* node) override;
    void visit(ast::Identifier* node) override;
    void visit(ast::RoutineCall* node) override;

    void print() { m_module->print(llvm::errs(), nullptr); }

private:
    llvm::LLVMContext m_context;
    llvm::IRBuilder<> m_builder;
    std::unique_ptr<llvm::Module> m_module;
    std::map<std::string, llvm::Value*> m_namedValues;

    // used for holding values that should be returned by functions
    llvm::Value* tempVal = nullptr;
    void extractTempVal(llvm::Value*& insertInto) {
        insertInto = tempVal;
        tempVal = nullptr;
    }
};

} // namespace cg
