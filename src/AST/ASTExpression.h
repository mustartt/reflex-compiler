//
// Created by Admininstrator on 2022-04-23.
//

#ifndef REFLEX_SRC_AST_ASTEXPRESSION_H_
#define REFLEX_SRC_AST_ASTEXPRESSION_H_

#include "AST.h"

#include <string>
#include <vector>

#include <Operator.h>
#include <ASTVisitor.h>

namespace reflex {

class SourceLocation;
class Declaration;
class ASTTypeExpr;

class Expression : public ASTNode, public ASTExpressionVisitable {
  public:
    explicit Expression(const SourceLocation *loc);
};

class ReferenceExpr : public Expression {
  public:
    ReferenceExpr(const SourceLocation *loc, Declaration *decl);

    [[nodiscard]] virtual std::string getReferenceName() const = 0;

    ASTExpressionVisitorDispatcher
  private:
    Declaration *decl;
};

class Identifier : public ReferenceExpr {
  public:
    Identifier(const SourceLocation *loc, Declaration *decl, std::string reference);

    [[nodiscard]] std::string getReferenceName() const override;
  private:
    std::string reference;
};

class ModuleSelector : public ReferenceExpr {
  public:
    ModuleSelector(const SourceLocation *loc, Declaration *decl,
                   std::string prefix, ReferenceExpr *child);

    [[nodiscard]] std::string getReferenceName() const override;
  private:
    std::string prefix;
    ReferenceExpr *child;
};

class UnaryExpr : public Expression {
  public:
    UnaryExpr(const SourceLocation *loc, Operator::UnaryOperator op, Expression *expr);

    ASTExpressionVisitorDispatcher
  private:
    Operator::UnaryOperator op;
    Expression *expr;
};

class BinaryExpr : public Expression {
  public:
    BinaryExpr(const SourceLocation *loc, Operator::BinaryOperator op,
               Expression *lhs, Expression *rhs);

    ASTExpressionVisitorDispatcher
  private:
    Operator::BinaryOperator op;
    Expression *lhs;
    Expression *rhs;
};

class NewExpr : public Expression {
  public:
    NewExpr(const SourceLocation *loc, ASTTypeExpr *instanceType);

    ASTExpressionVisitorDispatcher
  private:
    ASTTypeExpr *instanceType;
};

class CastExpr : public Expression {
  public:
    CastExpr(const SourceLocation *loc, ASTTypeExpr *resultType, Expression *from);

    ASTExpressionVisitorDispatcher
  private:
    ASTTypeExpr *resultType;
    Expression *from;
};

class IndexExpr : public Expression {
  public:
    IndexExpr(const SourceLocation *loc, Expression *expr, Expression *index);

    ASTExpressionVisitorDispatcher
  private:
    Expression *expr;
    Expression *index;
};

class SelectorExpr : public Expression {
  public:
    SelectorExpr(const SourceLocation *loc, Expression *expr, std::string aSelector);

    ASTExpressionVisitorDispatcher
  private:
    Expression *expr;
    std::string selector;
};

class ArgumentExpr : public Expression {
  public:
    ArgumentExpr(const SourceLocation *loc, Expression *expr, std::vector<Expression *> arguments);

    ASTExpressionVisitorDispatcher
  private:
    Expression *expr;
    std::vector<Expression *> arguments;
};

}
#endif //REFLEX_SRC_AST_ASTEXPRESSION_H_
