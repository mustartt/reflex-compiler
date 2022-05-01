//
// Created by henry on 2022-04-28.
//

#ifndef REFLEX_SRC_LEXICALCONTEXT_LEXICALCONTEXTDECLTYPEPASS_H_
#define REFLEX_SRC_LEXICALCONTEXT_LEXICALCONTEXTDECLTYPEPASS_H_

#include <stack>

#include "ASTVisitor.h"
#include "TypeParser.h"

namespace reflex {

class TypeContext;
class LexicalScope;
class LexicalContextDeclTypePass : public ASTDeclVisitor,
                                   public ASTStmtVisitor,
                                   public ASTExprVisitor {
  private:
    class ReleaseScope {
      public:
        ReleaseScope(LexicalContextDeclTypePass &pass, LexicalScope *scope);
        ~ReleaseScope();
      private:
        LexicalContextDeclTypePass &pass;
    };
    friend class ReleaseScope;
  public:
    explicit LexicalContextDeclTypePass(TypeContext &typeContext)
        : typeContext(typeContext), typeParser(typeContext) {}

    void performPass(CompilationUnit *unit);

    OpaqueType visit(CompilationUnit &unit) override;
    OpaqueType visit(ClassDecl &decl) override;
    OpaqueType visit(InterfaceDecl &decl) override;
    OpaqueType visit(VariableDecl &decl) override;
    OpaqueType visit(FieldDecl &decl) override;
    OpaqueType visit(ParamDecl &decl) override;
    OpaqueType visit(FunctionDecl &decl) override;
    OpaqueType visit(MethodDecl &decl) override;

    OpaqueType visit(BlockStmt &stmt) override;
    OpaqueType visit(ReturnStmt &stmt) override;
    OpaqueType visit(BreakStmt &stmt) override ASTVisitorDefImpl;
    OpaqueType visit(ContinueStmt &stmt) override ASTVisitorDefImpl;
    OpaqueType visit(IfStmt &stmt) override;
    OpaqueType visit(ForStmt &stmt) override;
    OpaqueType visit(WhileStmt &stmt) override;
    OpaqueType visit(EmptyStmt &stmt) override ASTVisitorDefImpl;
    OpaqueType visit(AssignmentStmt &stmt) override;
    OpaqueType visit(IncDecStmt &stmt) override;
    OpaqueType visit(ExpressionStmt &stmt) override;
    OpaqueType visit(DeclStmt &stmt) override;

    OpaqueType visit(DeclRefExpr &expr) override ASTVisitorDefImpl;
    OpaqueType visit(UnaryExpr &expr) override;
    OpaqueType visit(BinaryExpr &expr) override;
    OpaqueType visit(NewExpr &expr) override;
    OpaqueType visit(ImplicitCastExpr &expr) override ASTVisitorDefImpl;
    OpaqueType visit(CastExpr &expr) override;
    OpaqueType visit(IndexExpr &expr) override;
    OpaqueType visit(SelectorExpr &expr) override;
    OpaqueType visit(ArgumentExpr &expr) override;

    OpaqueType visit(NumberLiteral &literal) override ASTVisitorDefImpl;
    OpaqueType visit(StringLiteral &literal) override ASTVisitorDefImpl;
    OpaqueType visit(BooleanLiteral &literal) override ASTVisitorDefImpl;
    OpaqueType visit(NullLiteral &literal) override ASTVisitorDefImpl;
    OpaqueType visit(ArrayLiteral &literal) override;
    OpaqueType visit(FunctionLiteral &literal) override;

  private:
    TypeContext &typeContext;
    std::stack<LexicalScope *> scopes;
    TypeParser typeParser;
};

} // reflex

#endif //REFLEX_SRC_LEXICALCONTEXT_LEXICALCONTEXTDECLTYPEPASS_H_
