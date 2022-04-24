//
// Created by Admininstrator on 2022-04-23.
//

#ifndef REFLEX_SRC_AST_ASTSTATEMENT_H_
#define REFLEX_SRC_AST_ASTSTATEMENT_H_

#include "AST.h"

#include <string>
#include <vector>

#include "Operator.h"

namespace reflex {

class SourceLocation;
class Expression;
class VariableDecl;

class Statement : public ASTNode {
  public:
    explicit Statement(const SourceLocation *loc);
};

class BlockStmt : public Statement {
  public:
    BlockStmt(const SourceLocation *loc, std::vector<Statement *> stmts);

    [[nodiscard]] const std::vector<Statement *> &getStatements() const;
  private:
    std::vector<Statement *> statements;
};

class SimpleStmt : public Statement {
  public:
    explicit SimpleStmt(const SourceLocation *loc);
};

class ReturnStmt : public Statement {
    Expression *returnValue;
  public:
    ReturnStmt(const SourceLocation *loc, Expression *returnValue);
};

class BreakStmt : public Statement {
  public:
    explicit BreakStmt(const SourceLocation *loc);
};

class ContinueStmt : public Statement {
  public:
    explicit ContinueStmt(const SourceLocation *loc);
};

class IfStmt : public Statement {
  public:
    IfStmt(const SourceLocation *loc, SimpleStmt *cond, BlockStmt *primaryBlock, BlockStmt *elseBlock);
    [[nodiscard]] SimpleStmt *getCond() const { return cond; }
    [[nodiscard]] BlockStmt *getPrimaryBlock() const { return primaryBlock; }
    [[nodiscard]] BlockStmt *getElseBlock() const { return elseBlock; }
  private:
    SimpleStmt *cond;
    BlockStmt *primaryBlock;
    BlockStmt *elseBlock;
};

class ForClause : public ASTNode {
  public:
    explicit ForClause(const SourceLocation *loc) : ASTNode(loc) {}
};

class ForRangeClause : public ForClause {
  public:
    ForRangeClause(const SourceLocation *loc, VariableDecl *variable, Expression *iterExpr);
    [[nodiscard]] VariableDecl *getVariable() const { return variable; }
    [[nodiscard]] Expression *getIterExpr() const { return iterExpr; }
  private:
    VariableDecl *variable;
    Expression *iterExpr;
};

class ForNormalClause : public ForClause {
  public:
    ForNormalClause(const SourceLocation *loc, Statement *init, Expression *cond, SimpleStmt *post);
    [[nodiscard]] Statement *getInit() const { return init; }
    [[nodiscard]] Expression *getCond() const { return cond; }
    [[nodiscard]] SimpleStmt *getPost() const { return post; }
  private:
    Statement *init;
    Expression *cond;
    SimpleStmt *post;
};

class ForStmt : public Statement {
  public:
    ForStmt(const SourceLocation *loc, ForClause *clause, BlockStmt *body);
    [[nodiscard]] ForClause *getClause() const { return clause; }
    [[nodiscard]] BlockStmt *getBody() const { return body; }
  private:
    ForClause *clause;
    BlockStmt *body;
};

class WhileStmt : public Statement {
  public:
    WhileStmt(const SourceLocation *loc, SimpleStmt *cond, BlockStmt *body);
    [[nodiscard]] SimpleStmt *getCond() const { return cond; }
    [[nodiscard]] BlockStmt *getBody() const { return body; }
  private:
    SimpleStmt *cond;
    BlockStmt *body;
};

class EmptyStmt : public SimpleStmt {
  public:
    explicit EmptyStmt(const SourceLocation *loc);
};

class AssignmentStmt : public SimpleStmt {
  public:
    AssignmentStmt(const SourceLocation *loc, Operator::AssignOperator assignOp, Expression *lhs, Expression *rhs);
    [[nodiscard]] Operator::AssignOperator getAssignOp() const { return assignOp; }
    [[nodiscard]] Expression *getLhs() const { return lhs; }
    [[nodiscard]] Expression *getRhs() const { return rhs; }
  private:
    Operator::AssignOperator assignOp;
    Expression *lhs;
    Expression *rhs;
};

class IncDecStmt : public SimpleStmt {
  public:
    IncDecStmt(const SourceLocation *loc, Operator::PostfixOperator postfixOp, Expression *expr);
    [[nodiscard]] Operator::PostfixOperator getPostfixOp() const { return postfixOp; }
    [[nodiscard]] Expression *getExpr() const { return expr; }
  private:
    Operator::PostfixOperator postfixOp;
    Expression *expr;
};

class ExpressionStmt : public SimpleStmt {
  public:
    ExpressionStmt(const SourceLocation *loc, Expression *expr);
    [[nodiscard]] Expression *getExpr() const { return expr; }
  private:
    Expression *expr;
};

}

#endif //REFLEX_SRC_AST_ASTSTATEMENT_H_
