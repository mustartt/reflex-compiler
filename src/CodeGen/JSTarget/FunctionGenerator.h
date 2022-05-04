//
// Created by henry on 2022-05-03.
//

#ifndef REFLEX_SRC_CODEGEN_JSTARGET_FUNCTIONGENERATOR_H_
#define REFLEX_SRC_CODEGEN_JSTARGET_FUNCTIONGENERATOR_H_

#include <string>
#include <unordered_map>

#include "ASTDeclaration.h"
#include "ASTVisitor.h"

namespace reflex {

class FunctionGenerator : public ASTStmtVisitor, public ASTDeclVisitor, public ASTExprVisitor {
    class ScopeIndent {
      public:
        explicit ScopeIndent(FunctionGenerator &gen);
        ~ScopeIndent();
      private:
        FunctionGenerator &gen;
    };
    friend class ScopeIndent;
  public:
    FunctionGenerator(std::ostream &os, std::ostream &declos, FunctionDecl *funcdecl)
        : os(os), declos(declos), funcdecl(funcdecl) {}

    virtual void emit();

  protected:
    std::string getVariableName(VariableDecl *decl);

    OpaqueType visit(ClassDecl &decl) override ASTVisitorDefImpl
    OpaqueType visit(InterfaceDecl &decl) override ASTVisitorDefImpl
    OpaqueType visit(VariableDecl &decl) override;
    OpaqueType visit(FieldDecl &decl) override ASTVisitorDefImpl
    OpaqueType visit(ParamDecl &decl) override;
    OpaqueType visit(FunctionDecl &decl) override;
    OpaqueType visit(MethodDecl &decl) override ASTVisitorDefImpl
    OpaqueType visit(CompilationUnit &unit) override ASTVisitorDefImpl

    OpaqueType visit(BlockStmt &stmt) override;
    OpaqueType visit(ReturnStmt &stmt) override;
    OpaqueType visit(BreakStmt &stmt) override;
    OpaqueType visit(ContinueStmt &stmt) override;
    OpaqueType visit(IfStmt &stmt) override;
    OpaqueType visit(ForStmt &stmt) override;
    OpaqueType visit(WhileStmt &stmt) override;
    OpaqueType visit(EmptyStmt &stmt) override;
    OpaqueType visit(AssignmentStmt &stmt) override;
    OpaqueType visit(IncDecStmt &stmt) override;
    OpaqueType visit(ExpressionStmt &stmt) override;
    OpaqueType visit(DeclStmt &stmt) override;

    OpaqueType visit(DeclRefExpr &expr) override;
    OpaqueType visit(UnaryExpr &expr) override;
    OpaqueType visit(BinaryExpr &expr) override;
    OpaqueType visit(NewExpr &expr) override;
    OpaqueType visit(ImplicitCastExpr &expr) override;
    OpaqueType visit(CastExpr &expr) override;
    OpaqueType visit(IndexExpr &expr) override;
    OpaqueType visit(SelectorExpr &expr) override;
    OpaqueType visit(ArgumentExpr &expr) override;

    OpaqueType visit(NumberLiteral &literal) override;
    OpaqueType visit(StringLiteral &literal) override;
    OpaqueType visit(BooleanLiteral &literal) override;
    OpaqueType visit(NullLiteral &literal) override;
    OpaqueType visit(ArrayLiteral &literal) override;
    OpaqueType visit(FunctionLiteral &literal) override;

  private:
    void registerLocalDecl(VariableDecl *decl);
    size_t getLocalDeclRef(VariableDecl *decl) const;
    std::ostream &printIndent();

    FunctionDecl *funcdecl;
    std::ostream &os;
    std::ostream &declos;
    std::unordered_map<VariableDecl *, size_t> refTable;
    size_t indent = 0;
};

class MethodGenerator : public FunctionGenerator {

};

} // reflex

#endif //REFLEX_SRC_CODEGEN_JSTARGET_FUNCTIONGENERATOR_H_
