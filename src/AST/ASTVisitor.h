//
// Created by Admininstrator on 2022-04-25.
//

#ifndef REFLEX_SRC_AST_ASTVISITOR_H_
#define REFLEX_SRC_AST_ASTVISITOR_H_

#include <Utils/Generics.h>

namespace reflex {

class DeclRefExpr;
class UnaryExpr;
class BinaryExpr;
class NewExpr;
class ImplicitCastExpr;
class CastExpr;
class IndexExpr;
class SelectorExpr;
class ArgumentExpr;

class NumberLiteral;
class StringLiteral;
class BooleanLiteral;
class NullLiteral;
class ArrayLiteral;
class FunctionLiteral;

class ASTExprVisitor {
  public:
    virtual ~ASTExprVisitor() = default;

    virtual OpaqueType visit(DeclRefExpr &) = 0;
    virtual OpaqueType visit(UnaryExpr &) = 0;
    virtual OpaqueType visit(BinaryExpr &) = 0;
    virtual OpaqueType visit(NewExpr &) = 0;
    virtual OpaqueType visit(ImplicitCastExpr &) = 0;
    virtual OpaqueType visit(CastExpr &) = 0;
    virtual OpaqueType visit(IndexExpr &) = 0;
    virtual OpaqueType visit(SelectorExpr &) = 0;
    virtual OpaqueType visit(ArgumentExpr &) = 0;

    virtual OpaqueType visit(NumberLiteral &) = 0;
    virtual OpaqueType visit(StringLiteral &) = 0;
    virtual OpaqueType visit(BooleanLiteral &) = 0;
    virtual OpaqueType visit(NullLiteral &) = 0;
    virtual OpaqueType visit(ArrayLiteral &) = 0;
    virtual OpaqueType visit(FunctionLiteral &) = 0;
};

class ASTExpressionVisitable {
  public:
    virtual ~ASTExpressionVisitable() = default;

    virtual OpaqueType accept(ASTExprVisitor *v) = 0;
};

#define ASTExprVisitorDispatcher \
OpaqueType accept(ASTExprVisitor *v) override { return v->visit(*this); }

class ClassDecl;
class InterfaceDecl;
class VariableDecl;
class FieldDecl;
class ParamDecl;
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
    virtual OpaqueType visit(ParamDecl &) = 0;
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
class DeclStmt;

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
    virtual OpaqueType visit(DeclStmt &) = 0;
};

class ASTStmtVisitable {
  public:
    virtual ~ASTStmtVisitable() = default;

    virtual OpaqueType accept(ASTStmtVisitor *v) = 0;
};

#define ASTStmtVisitorDispatcher \
OpaqueType accept(ASTStmtVisitor *v) override { return v->visit(*this); }

#define ASTVisitorDefImpl \
{ return {}; }

#define ASTVisitorDefRetVal {}

}

#endif //REFLEX_SRC_AST_ASTVISITOR_H_
