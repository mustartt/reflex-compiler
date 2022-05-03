//
// Created by henry on 2022-05-03.
//

#ifndef REFLEX_SRC_LEXICALCONTEXT_SEMANTICANALYSISPASS_H_
#define REFLEX_SRC_LEXICALCONTEXT_SEMANTICANALYSISPASS_H_

#include "ASTVisitor.h"

namespace reflex {

class TypeContext;
class ASTContext;
class LexicalScope;

/// run @class SemanticAnalyzer on all decls
class SemanticAnalysisPass : public ASTDeclVisitor,
                             public ASTStmtVisitor,
                             public ASTExprVisitor {
  public:
    SemanticAnalysisPass(TypeContext &typeContext, ASTContext &context, LexicalScope *global)
        : typeContext(typeContext), context(context), globalScope(global) {}

    OpaqueType visit(CompilationUnit &unit) override;

    OpaqueType visit(VariableDecl &decl) override;
    OpaqueType visit(ClassDecl &decl) override;
    OpaqueType visit(InterfaceDecl &decl) override ASTVisitorDefImpl
    OpaqueType visit(FieldDecl &decl) override ASTVisitorDefImpl
    OpaqueType visit(ParamDecl &decl) override ASTVisitorDefImpl
    OpaqueType visit(FunctionDecl &decl) override;
    OpaqueType visit(MethodDecl &decl) override ASTVisitorDefImpl;

    OpaqueType visit(BlockStmt &stmt) override;
    OpaqueType visit(ReturnStmt &stmt) override;
    OpaqueType visit(BreakStmt &stmt) override ASTVisitorDefImpl
    OpaqueType visit(ContinueStmt &stmt) override ASTVisitorDefImpl
    OpaqueType visit(IfStmt &stmt) override;
    OpaqueType visit(ForStmt &stmt) override;
    OpaqueType visit(WhileStmt &stmt) override;
    OpaqueType visit(EmptyStmt &stmt) override ASTVisitorDefImpl
    OpaqueType visit(AssignmentStmt &stmt) override;
    OpaqueType visit(IncDecStmt &stmt) override;
    OpaqueType visit(ExpressionStmt &stmt) override;
    OpaqueType visit(DeclStmt &stmt) override;

    OpaqueType visit(DeclRefExpr &expr) override ASTVisitorDefImpl
    OpaqueType visit(UnaryExpr &expr) override;
    OpaqueType visit(BinaryExpr &expr) override;
    OpaqueType visit(NewExpr &expr) override ASTVisitorDefImpl
    OpaqueType visit(ImplicitCastExpr &expr) override;
    OpaqueType visit(CastExpr &expr) override;
    OpaqueType visit(IndexExpr &expr) override;
    OpaqueType visit(SelectorExpr &expr) override;
    OpaqueType visit(ArgumentExpr &expr) override;
    OpaqueType visit(NumberLiteral &literal) override ASTVisitorDefImpl
    OpaqueType visit(StringLiteral &literal) override ASTVisitorDefImpl
    OpaqueType visit(BooleanLiteral &literal) override ASTVisitorDefImpl
    OpaqueType visit(NullLiteral &literal) override ASTVisitorDefImpl
    OpaqueType visit(ArrayLiteral &literal) override;
    OpaqueType visit(FunctionLiteral &literal) override;

  private:
    TypeContext &typeContext;
    ASTContext &context;
    LexicalScope *globalScope;
};

} // reflex

#endif //REFLEX_SRC_LEXICALCONTEXT_SEMANTICANALYSISPASS_H_
