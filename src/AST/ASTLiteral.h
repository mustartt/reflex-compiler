//
// Created by Admininstrator on 2022-04-23.
//

#ifndef REFLEX_SRC_AST_ASTLITERAL_H_
#define REFLEX_SRC_AST_ASTLITERAL_H_

#include "ASTExpression.h"

namespace reflex {

class BadLiteralError : public std::runtime_error {
  public:
    explicit BadLiteralError(const std::string &arg);
};

class ParamDecl;
class BlockStmt;

class Literal : public Expression {
  public:
    explicit Literal(const SourceLocation *loc);
};

class BasicLiteral : public Literal {
  public:
    BasicLiteral(const SourceLocation *loc, std::string value);

    const std::string &getLiteral() const { return value; }

  protected:
    std::string value;
};

class NumberLiteral : public BasicLiteral {
  public:
    NumberLiteral(const SourceLocation *loc, std::string str);

    double getValue() const { return val; }

    ASTExprVisitorDispatcher
  private:
    double val;
};

class StringLiteral : public BasicLiteral {
  public:
    StringLiteral(const SourceLocation *loc, std::string value);

    ASTExprVisitorDispatcher
};

class BooleanLiteral : public BasicLiteral {
  public:
    BooleanLiteral(const SourceLocation *loc, std::string value);

    bool getValue() const { return literal; }

    ASTExprVisitorDispatcher
  private:
    bool literal{};
};

class NullLiteral : public BasicLiteral {
  public:
    NullLiteral(const SourceLocation *loc, std::string value);

    ASTExprVisitorDispatcher
};

class ArrayLiteral : public Literal {
  public:
    ArrayLiteral(const SourceLocation *loc, std::vector<Expression *> list);

    const std::vector<Expression *> &getInitList() const { return list; }

    ASTExprVisitorDispatcher
  private:
    std::vector<Expression *> list;
};

class FunctionLiteral : public Literal {
  public:
    FunctionLiteral(const SourceLocation *loc,
                    std::vector<ParamDecl *> parameters,
                    ASTTypeExpr *returnType,
                    BlockStmt *body);

    const std::vector<ParamDecl *> &getParamDecls() const { return parameters; }
    ASTTypeExpr *getReturnType() const { return returnType; }
    BlockStmt *getBody() const { return body; }

    ASTExprVisitorDispatcher
  private:
    std::vector<ParamDecl *> parameters;
    ASTTypeExpr *returnType;
    BlockStmt *body;
};

}

#endif //REFLEX_SRC_AST_ASTLITERAL_H_
