//
// Created by Admininstrator on 2022-04-23.
//

#ifndef REFLEX_SRC_AST_ASTEXPRESSION_H_
#define REFLEX_SRC_AST_ASTEXPRESSION_H_

#include "AST.h"

#include <string>
#include <vector>

#include "Operator.h"

namespace reflex {

class SourceLocation;
class Declaration;
class ASTTypeExpr;

class Expression : public ASTNode {
  public:
    explicit Expression(const SourceLocation *loc);
};

class ReferenceExpr : public Expression {
  public:
    ReferenceExpr(const SourceLocation *loc, Declaration *decl);

    [[nodiscard]] virtual std::string getReferenceName() const = 0;

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
  private:
    Operator::UnaryOperator op;
    Expression *expr;
};

class BinaryExpr : public Expression {
  public:
    BinaryExpr(const SourceLocation *loc, Operator::BinaryOperator op,
               Expression *lhs, Expression *rhs);
  private:
    Operator::BinaryOperator op;
    Expression *lhs;
    Expression *rhs;
};

class NewExpr : public Expression {
  public:
    NewExpr(const SourceLocation *loc, ASTTypeExpr *instanceType);
  private:
    ASTTypeExpr *instanceType;
};

class CastExpr : public Expression {
  public:
    CastExpr(const SourceLocation *loc, ASTTypeExpr *resultType, Expression *from);
  private:
    ASTTypeExpr *resultType;
    Expression *from;
};

class IndexExpr : public Expression {
  public:
    IndexExpr(const SourceLocation *loc, Expression *expr, Expression *index);
  private:
    Expression *expr;
    Expression *index;
};

class SelectorExpr : public Expression {
  public:
    SelectorExpr(const SourceLocation *loc, Expression *expr, std::string aSelector);
  private:
    Expression *expr;
    std::string selector;
};

class ArgumentExpr : public Expression {
  public:
    ArgumentExpr(const SourceLocation *loc, Expression *expr, std::vector<Expression *> arguments);
  private:
    Expression *expr;
    std::vector<Expression *> arguments;
};

}
#endif //REFLEX_SRC_AST_ASTEXPRESSION_H_
