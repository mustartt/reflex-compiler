//
// Created by Admininstrator on 2022-04-23.
//

#include "ASTStatement.h"

namespace reflex {

Statement::Statement(const SourceLocation *loc) : ASTNode(loc) {}

BlockStmt::BlockStmt(const SourceLocation *loc, std::vector<Statement *> stmts)
    : Statement(loc), statements(std::move(stmts)) {}

SimpleStmt::SimpleStmt(const SourceLocation *loc) : Statement(loc) {}

ReturnStmt::ReturnStmt(const SourceLocation *loc, Expression *returnValue)
    : Statement(loc), returnValue(returnValue) {}

BreakStmt::BreakStmt(const SourceLocation *loc) : Statement(loc) {}

ContinueStmt::ContinueStmt(const SourceLocation *loc) : Statement(loc) {}

IfStmt::IfStmt(const SourceLocation *loc, SimpleStmt *cond,
               BlockStmt *primaryBlock, BlockStmt *elseBlock)
    : Statement(loc), cond(cond), primaryBlock(primaryBlock), elseBlock(elseBlock) {}

ForRangeClause::ForRangeClause(const SourceLocation *loc, VariableDecl *variable, Expression *iterExpr)
    : ForClause(loc), variable(variable), iterExpr(iterExpr) {}

ForNormalClause::ForNormalClause(const SourceLocation *loc, Statement *init, Expression *cond, SimpleStmt *post)
    : ForClause(loc), init(init), cond(cond), post(post) {}

ForStmt::ForStmt(const SourceLocation *loc, ForClause *clause, BlockStmt *body)
    : Statement(loc), clause(clause), body(body) {}

WhileStmt::WhileStmt(const SourceLocation *loc, SimpleStmt *cond, BlockStmt *body)
    : Statement(loc), cond(cond), body(body) {}

EmptyStmt::EmptyStmt(const SourceLocation *loc) : SimpleStmt(loc) {}

AssignmentStmt::AssignmentStmt(const SourceLocation *loc,
                               Operator::AssignOperator assignOp,
                               Expression *lhs,
                               Expression *rhs)
    : SimpleStmt(loc), assignOp(assignOp), lhs(lhs), rhs(rhs) {}

IncDecStmt::IncDecStmt(const SourceLocation *loc, Operator::PostfixOperator postfixOp, Expression *expr)
    : SimpleStmt(loc), postfixOp(postfixOp), expr(expr) {}

ExpressionStmt::ExpressionStmt(const SourceLocation *loc, Expression *expr)
    : SimpleStmt(loc), expr(expr) {}

}
