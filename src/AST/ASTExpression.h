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
class Type;

class Expression : public ASTNode, public ASTExpressionVisitable {
  public:
    explicit Expression(const SourceLocation *loc);

    Type *getType() const { return type; }
    void setType(Type *typ) { Expression::type = typ; }

  private:
    Type *type = nullptr;
};

class DeclRefExpr : public Expression {
  public:
    DeclRefExpr(const SourceLocation *loc, Declaration *decl);

    [[nodiscard]] virtual std::string getReferenceName() const = 0;
    [[nodiscard]] virtual const std::string &getBaseRefName() const = 0;
    void setDecl(Declaration *ref) { DeclRefExpr::decl = ref; }

    ASTExprVisitorDispatcher
  private:
    Declaration *decl;
};

class ModuleSelector : public DeclRefExpr {
  public:
    ModuleSelector(const SourceLocation *loc, Declaration *decl,
                   std::string prefix, DeclRefExpr *child);

    [[nodiscard]] std::string getReferenceName() const override;
    const std::string &getBaseRefName() const override;
  private:
    std::string prefix;
    DeclRefExpr *child;
};

class UnaryExpr : public Expression {
  public:
    UnaryExpr(const SourceLocation *loc, Operator::UnaryOperator op, Expression *expr);

    Operator::UnaryOperator getUnaryOp() const { return op; }
    Expression *getExpr() const { return expr; }

    ASTExprVisitorDispatcher
  private:
    Operator::UnaryOperator op;
    Expression *expr;
};

class BinaryExpr : public Expression {
  public:
    BinaryExpr(const SourceLocation *loc, Operator::BinaryOperator op,
               Expression *lhs, Expression *rhs);

    Operator::BinaryOperator getBinaryOp() const { return op; }
    Expression *getLhs() const { return lhs; }
    Expression *getRhs() const { return rhs; }

    ASTExprVisitorDispatcher
  private:
    Operator::BinaryOperator op;
    Expression *lhs;
    Expression *rhs;
};

class NewExpr : public Expression {
  public:
    NewExpr(const SourceLocation *loc, ASTTypeExpr *instanceType);

    ASTTypeExpr *getInstanceType() const { return instanceType; }

    ASTExprVisitorDispatcher
  private:
    ASTTypeExpr *instanceType;
};

class ImplicitCastExpr : public Expression {
  public:
    ImplicitCastExpr(const SourceLocation *loc, Expression *from,
                     Operator::ImplicitConversion conversion)
        : Expression(loc), from(from), conversion(conversion) {}

    Expression *getFrom() const { return from; }
    Operator::ImplicitConversion getConversion() const { return conversion; }

    ASTExprVisitorDispatcher
  private:
    Expression *from;
    Operator::ImplicitConversion conversion;
};

class Identifier : public DeclRefExpr {
  public:
    Identifier(const SourceLocation *loc, Declaration *decl, std::string reference);

    [[nodiscard]] std::string getReferenceName() const override;
    const std::string &getBaseRefName() const override;
  private:
    std::string reference;
};

class CastExpr : public Expression {
  public:
    CastExpr(const SourceLocation *loc, ASTTypeExpr *resultType, Expression *from);

    Expression *getFrom() const { return from; }
    ASTTypeExpr *getResultType() const { return resultType; }

    ASTExprVisitorDispatcher
  private:
    ASTTypeExpr *resultType;
    Expression *from;
};

class IndexExpr : public Expression {
  public:
    IndexExpr(const SourceLocation *loc, Expression *expr, Expression *index);

    Expression *getBaseExpr() const { return expr; }
    Expression *getIndex() const { return index; }

    ASTExprVisitorDispatcher
  private:
    Expression *expr;
    Expression *index;
};

class SelectorExpr : public Expression {
  public:
    SelectorExpr(const SourceLocation *loc, Expression *expr, std::string aSelector);

    Expression *getBaseExpr() const { return expr; }
    const std::string &getSelector() const { return selector; }

    ASTExprVisitorDispatcher
  private:
    Expression *expr;
    std::string selector;
};

class ArgumentExpr : public Expression {
  public:
    ArgumentExpr(const SourceLocation *loc, Expression *expr, std::vector<Expression *> arguments);

    Expression *getBaseExpr() const { return expr; }
    const std::vector<Expression *> &getArguments() const { return arguments; }

    ASTExprVisitorDispatcher
  private:
    Expression *expr;
    std::vector<Expression *> arguments;
};

}
#endif //REFLEX_SRC_AST_ASTEXPRESSION_H_
