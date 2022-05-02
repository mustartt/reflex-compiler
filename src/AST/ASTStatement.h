//
// Created by Admininstrator on 2022-04-23.
//

#ifndef REFLEX_SRC_AST_ASTSTATEMENT_H_
#define REFLEX_SRC_AST_ASTSTATEMENT_H_

#include "AST.h"

#include <string>
#include <vector>

#include <Operator.h>
#include <ASTVisitor.h>

namespace reflex {

class SourceLocation;
class Expression;
class VariableDecl;
class Declaration;
class Type;

class Statement : public ASTNode, public ASTStmtVisitable {
  public:
    explicit Statement(const SourceLocation *loc);
};

class BlockStmt : public Statement {
  public:
    BlockStmt(const SourceLocation *loc, std::vector<Statement *> stmts);

    const std::vector<Statement *> &getStmts() const { return statements; }

    ScopeMember *getScope() const { return scope; }
    void setScope(ScopeMember *lexicalScope) { BlockStmt::scope = lexicalScope; }

    ASTStmtVisitorDispatcher
  private:
    std::vector<Statement *> statements;
    ScopeMember *scope;
};

class DeclStmt : public Statement {
  public:
    DeclStmt(const SourceLocation *loc, Declaration *decl)
        : Statement(loc), decl(decl) {}

    Declaration *getDecl() const { return decl; }

    ASTStmtVisitorDispatcher
  private:
    Declaration *decl;
};

class SimpleStmt : public Statement {
  public:
    explicit SimpleStmt(const SourceLocation *loc);
};

class ReturnStmt : public Statement {
  public:
    ReturnStmt(const SourceLocation *loc, Expression *returnValue);

    Expression *getReturnValue() const { return returnValue; }
    Type *getReturnType() const { return type; }
    void setReturnType(Type *typ) { ReturnStmt::type = typ; }
    void setReturnValue(Expression *expr) { ReturnStmt::returnValue = expr; }

    ASTStmtVisitorDispatcher
  private:
    Expression *returnValue;
    Type *type;
};

class BreakStmt : public Statement {
  public:
    explicit BreakStmt(const SourceLocation *loc);

    ASTStmtVisitorDispatcher
};

class ContinueStmt : public Statement {
  public:
    explicit ContinueStmt(const SourceLocation *loc);

    ASTStmtVisitorDispatcher
};

class IfStmt : public Statement {
  public:
    IfStmt(const SourceLocation *loc, SimpleStmt *cond, BlockStmt *primaryBlock, BlockStmt *elseBlock);
    [[nodiscard]] SimpleStmt *getCond() const { return cond; }
    [[nodiscard]] BlockStmt *getPrimaryBlock() const { return primaryBlock; }
    [[nodiscard]] BlockStmt *getElseBlock() const { return elseBlock; }

    ASTStmtVisitorDispatcher
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

    ASTStmtVisitorDispatcher
  private:
    ForClause *clause;
    BlockStmt *body;
};

class WhileStmt : public Statement {
  public:
    WhileStmt(const SourceLocation *loc, SimpleStmt *cond, BlockStmt *body);
    [[nodiscard]] SimpleStmt *getCond() const { return cond; }
    [[nodiscard]] BlockStmt *getBody() const { return body; }

    ASTStmtVisitorDispatcher
  private:
    SimpleStmt *cond;
    BlockStmt *body;
};

class EmptyStmt : public SimpleStmt {
  public:
    explicit EmptyStmt(const SourceLocation *loc);

    ASTStmtVisitorDispatcher
};

class AssignmentStmt : public SimpleStmt {
  public:
    AssignmentStmt(const SourceLocation *loc, Operator::AssignOperator assignOp, Expression *lhs, Expression *rhs);
    [[nodiscard]] Operator::AssignOperator getAssignOp() const { return assignOp; }
    [[nodiscard]] Expression *getLhs() const { return lhs; }
    [[nodiscard]] Expression *getRhs() const { return rhs; }

    ASTStmtVisitorDispatcher
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

    ASTStmtVisitorDispatcher
  private:
    Operator::PostfixOperator postfixOp;
    Expression *expr;
};

class ExpressionStmt : public SimpleStmt {
  public:
    ExpressionStmt(const SourceLocation *loc, Expression *expr);
    [[nodiscard]] Expression *getExpr() const { return expr; }

    ASTStmtVisitorDispatcher
  private:
    Expression *expr;
};

}

#endif //REFLEX_SRC_AST_ASTSTATEMENT_H_
