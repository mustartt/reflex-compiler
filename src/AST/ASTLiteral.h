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
  protected:
    std::string value;
};

class NumberLiteral : public BasicLiteral {
  public:
    NumberLiteral(const SourceLocation *loc, std::string str);
  private:
    long val;
};

class StringLiteral : public BasicLiteral {
  public:
    StringLiteral(const SourceLocation *loc, std::string value);
};

class BooleanLiteral : public BasicLiteral {
  public:
    BooleanLiteral(const SourceLocation *loc, std::string value);
  private:
    bool literal;
};

class NullLiteral : public BasicLiteral {
  public:
    NullLiteral(const SourceLocation *loc, std::string value);
};

class ArrayLiteral : public Literal {
  public:
    ArrayLiteral(const SourceLocation *loc, std::vector<Expression *> list);
  private:
    std::vector<Expression *> list;
};

class FunctionLiteral : public Literal {
  public:
    FunctionLiteral(const SourceLocation *loc,
                    std::vector<ParamDecl *> parameters,
                    ASTTypeExpr *returnType,
                    BlockStmt *body);
  private:
    std::vector<ParamDecl *> parameters;
    ASTTypeExpr *returnType;
    BlockStmt *body;
};

}

#endif //REFLEX_SRC_AST_ASTLITERAL_H_
