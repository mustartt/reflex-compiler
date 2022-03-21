//
// Created by henry on 2022-03-20.
//

#ifndef REFLEX_SRC_AST_ASTNODES_H_
#define REFLEX_SRC_AST_ASTNODES_H_

#include "Ast.h"
#include <string>

#include <utility>
#include <vector>

#include "../Lexer/Token.h"
#include "Operator.h"

namespace reflex {

class IdentExpr : public AstExpr {};

class Identifier : public IdentExpr {
    std::string name;
  public:
    explicit Identifier(std::string name) : name(std::move(name)) {}
};

class ModuleSelector : public IdentExpr {
    IdentExpr *basename;
    std::string selector;
  public:
    ModuleSelector(IdentExpr *basename, std::string aSelector) : basename(basename), selector(std::move(aSelector)) {}
};

class BasicLiteral : AstExpr {
    std::string value;
  public:
    BasicLiteral(const Loc &loc, std::string value) : AstExpr(loc), value(std::move(value)) {}
};

class NumberLit : public BasicLiteral {
  public:
    NumberLit(const Loc &loc, const std::string &value) : BasicLiteral(loc, value) {}
};

class StringLit : public BasicLiteral {
  public:
    StringLit(const Loc &loc, const std::string &value) : BasicLiteral(loc, value) {}
};

class BoolLit : public BasicLiteral {
  public:
    BoolLit(const Loc &loc, const std::string &value) : BasicLiteral(loc, value) {}
};

class NullLit : public BasicLiteral {
  public:
    NullLit(const Loc &loc, const std::string &value) : BasicLiteral(loc, value) {}
};

class Expression;

class ArrayType : public AstExpr {
    AstExpr *elementTyp;
    Expression *lengthExpr;
  public:
    ArrayType(const Loc &loc, AstExpr *elementTyp, Expression *lengthExpr)
        : AstExpr(loc), elementTyp(elementTyp), lengthExpr(lengthExpr) {}
};

class FunctionType : public AstExpr {
    AstExpr *returnTyp;
    std::vector<AstExpr *> paramList;
  public:
    FunctionType(const Loc &loc, AstExpr *returnTyp, std::vector<AstExpr *> paramList)
        : AstExpr(loc), returnTyp(returnTyp), paramList(std::move(paramList)) {}
};

class ArrayLit : public AstExpr {
    std::vector<Expression *> initializerList;
  public:
    ArrayLit(const Loc &loc, std::vector<Expression *> initializerList)
        : AstExpr(loc), initializerList(std::move(initializerList)) {}
};

class Block;

class Parameter : public AstExpr {
    IdentExpr *name;
    AstExpr *typ;
  public:
    Parameter(const Loc &loc, IdentExpr *name, AstExpr *typ)
        : AstExpr(loc), name(name), typ(typ) {}
};

class FunctionLit : public AstExpr {
    std::vector<Parameter *> parameters;
    AstExpr *returnTyp;
    Block *body;
  public:
    FunctionLit(const Loc &loc, std::vector<Parameter *> parameters, AstExpr *returnTyp, Block *body)
        : AstExpr(loc), parameters(std::move(parameters)), returnTyp(returnTyp), body(body) {}
};

class Expression : AstExpr {};

class UnaryExpr : public Expression {
    UnaryOperator op;
    Expression *expr;
  public:
    UnaryExpr(UnaryOperator op, Expression *expr) : op(op), expr(expr) {}
};

class BinaryExpr : public Expression {
    BinaryOperator op;
    Expression *lhs;
    Expression *rhs;
  public:
    BinaryExpr(BinaryOperator op, Expression *lhs, Expression *rhs) : op(op), lhs(lhs), rhs(rhs) {}
};

class NewExpr : public Expression {
    AstExpr *instanceTyp;
  public:
    explicit NewExpr(AstExpr *instanceTyp) : instanceTyp(instanceTyp) {}
};

class CastExpr : public Expression {
    AstExpr *targetTyp;
    Expression *from;
  public:
    CastExpr(AstExpr *targetTyp, Expression *from) : targetTyp(targetTyp), from(from) {}
};

class SelectorExpr : public Expression {
    Expression *baseExpr;
    IdentExpr *selector;
  public:
    SelectorExpr(Expression *baseExpr, IdentExpr *aSelector) : baseExpr(baseExpr), selector(aSelector) {}
};

class ArgumentExpr : public Expression {
    Expression *baseExpr;
    std::vector<Expression *> arguments;
  public:
    ArgumentExpr(Expression *baseExpr, std::vector<Expression *> arguments)
        : baseExpr(baseExpr), arguments(std::move(arguments)) {}
};

class Statement : public AstExpr;

class SimpleStmt : public Statement;

class VariableDecl : public Statement {
    Identifier *name;
    AstExpr *typ;
    Expression *initializer;
  public:
    VariableDecl(Identifier *name, AstExpr *typ, Expression *initializer)
        : name(name), typ(typ), initializer(initializer) {}
};

class ReturnStmt : public Statement {
    Expression *returnValue;
  public:
    explicit ReturnStmt(Expression *returnValue) : returnValue(returnValue) {}
};

class BreakStmt : public Statement {};

class ContinueStmt : public Statement {};

class Block;

class IfStmt : public Statement {
    SimpleStmt *cond;
    Block *primaryBlock;
    Block *elseBlock;
  public:
    IfStmt(SimpleStmt *cond, Block *primaryBlock, Block *elseBlock)
        : cond(cond), primaryBlock(primaryBlock), elseBlock(elseBlock) {}
};

class ForClause : public AstExpr {};

class ForRangeClause : public ForClause {
    VariableDecl *variable;
    Expression *iterExpr;
  public:
    ForRangeClause(VariableDecl *variable, Expression *iterExpr) : variable(variable), iterExpr(iterExpr) {}
};

class ForNormalClause : public ForClause {
    AstExpr *init;
    Expression *cond;
    SimpleStmt *post;
  public:
    ForNormalClause(AstExpr *init, Expression *cond, SimpleStmt *post) : init(init), cond(cond), post(post) {}
};

class ForStmt : public Statement {
    ForClause *clause;
    Block *body;
  public:
    ForStmt(ForClause *clause, Block *body) : clause(clause), body(body) {}
};

class WhileStmt : public Statement {
    SimpleStmt *cond;
    Block *body;
  public:
    WhileStmt(SimpleStmt *cond, Block *body) : cond(cond), body(body) {}
};

class EmptyStmt : public SimpleStmt {};

class AssignmentStmt : public SimpleStmt {
    AssignOperator assignOp;
    Expression *lhs;
    Expression *rhs;
  public:
    AssignmentStmt(AssignOperator assignOp, Expression *lhs, Expression *rhs)
        : assignOp(assignOp), lhs(lhs), rhs(rhs) {}
};

class IncDecStmt : public SimpleStmt {
    PostfixOperator postfixOp;
    Expression *expr;
  public:
    IncDecStmt(PostfixOperator postfixOp, Expression *expr) : postfixOp(postfixOp), expr(expr) {}
};

class ExpressionStmt : public SimpleStmt {
    Expression *expr;
  public:
    explicit ExpressionStmt(Expression *expr) : expr(expr) {}
};

class Block : public AstExpr {
    std::vector<Statement *> stmts;
  public:
    Block(const Loc &loc, std::vector<Statement *> stmts)
        : AstExpr(loc), stmts(std::move(stmts)) {}
};

class ClassDecl : public AstExpr {};
class MemberDecl : public AstExpr {};

}

#endif //REFLEX_SRC_AST_ASTNODES_H_
