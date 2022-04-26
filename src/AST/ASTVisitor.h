//
// Created by Admininstrator on 2022-04-25.
//

#ifndef REFLEX_SRC_AST_ASTVISITOR_H_
#define REFLEX_SRC_AST_ASTVISITOR_H_

#include <Utils/Generics.h>

namespace reflex {

class ReferenceExpr;
class UnaryExpr;
class BinaryExpr;
class NewExpr;
class CastExpr;
class IndexExpr;
class SelectorExpr;
class ArgumentExpr;

class ASTExpressionVisitor {
  public:
    virtual ~ASTExpressionVisitor() = default;

    virtual OpaqueType visit(ReferenceExpr &) = 0;
    virtual OpaqueType visit(UnaryExpr &) = 0;
    virtual OpaqueType visit(BinaryExpr &) = 0;
    virtual OpaqueType visit(NewExpr &) = 0;
    virtual OpaqueType visit(CastExpr &) = 0;
    virtual OpaqueType visit(IndexExpr &) = 0;
    virtual OpaqueType visit(SelectorExpr &) = 0;
    virtual OpaqueType visit(ArgumentExpr &) = 0;
};

class ASTExpressionVisitable {
  public:
    virtual ~ASTExpressionVisitable() = default;

    virtual OpaqueType accept(ASTExpressionVisitor *v) = 0;
};

#define ASTExpressionVisitorDispatcher \
OpaqueType accept(ASTExpressionVisitor *v) override { return v->visit(*this); }

class ClassDecl;
class InterfaceDecl;
class VariableDecl;
class FieldDecl;
class FunctionDecl;
class MethodDecl;
class CompilationUnit;

class ASTDeclVisitor {
  public:
    virtual ~ASTDeclVisitor() = default;

    virtual OpaqueType visit(ClassDecl &) = 0;
    virtual OpaqueType visit(InterfaceDecl &) = 0;
    virtual OpaqueType visit(VariableDecl &) = 0;
    virtual OpaqueType visit(FieldDecl &) = 0;
    virtual OpaqueType visit(FunctionDecl &) = 0;
    virtual OpaqueType visit(MethodDecl &) = 0;
    virtual OpaqueType visit(CompilationUnit &) = 0;
};

class ASTDeclVisitable {
  public:
    virtual ~ASTDeclVisitable() = default;

    virtual OpaqueType accept(ASTDeclVisitor *v) = 0;
};

#define ASTDeclVisitorDispatcher \
OpaqueType accept(ASTDeclVisitor *v) override { return v->visit(*this); }

class BlockStmt;
class ReturnStmt;
class BreakStmt;
class ContinueStmt;
class IfStmt;
class ForStmt;
class WhileStmt;
class EmptyStmt;
class AssignmentStmt;
class IncDecStmt;
class ExpressionStmt;

class ASTStmtVisitor {
  public:
    virtual ~ASTStmtVisitor() = default;

    virtual OpaqueType visit(BlockStmt &) = 0;
    virtual OpaqueType visit(ReturnStmt &) = 0;
    virtual OpaqueType visit(BreakStmt &) = 0;
    virtual OpaqueType visit(ContinueStmt &) = 0;
    virtual OpaqueType visit(IfStmt &) = 0;
    virtual OpaqueType visit(ForStmt &) = 0;
    virtual OpaqueType visit(WhileStmt &) = 0;
    virtual OpaqueType visit(EmptyStmt &) = 0;
    virtual OpaqueType visit(AssignmentStmt &) = 0;
    virtual OpaqueType visit(IncDecStmt &) = 0;
    virtual OpaqueType visit(ExpressionStmt &) = 0;
};

class ASTStmtVisitable {
  public:
    virtual ~ASTStmtVisitable() = default;

    virtual OpaqueType accept(ASTStmtVisitor *v) = 0;
};

#define ASTStmtVisitorDispatcher \
OpaqueType accept(ASTStmtVisitor *v) override { return v->visit(*this); }

}

#endif //REFLEX_SRC_AST_ASTVISITOR_H_
