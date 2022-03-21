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

class IdentExpr : public AstExpr {
  public:
    explicit IdentExpr(const Loc &loc) : AstExpr(loc) {}
    [[nodiscard]] virtual std::string getIdent() const = 0;
};

class Identifier : public IdentExpr {
    std::string name;
  public:
    Identifier(const Loc &loc, std::string name) : IdentExpr(loc), name(std::move(name)) {}
    [[nodiscard]] std::string getIdent() const override { return name; }
};

class ModuleSelector : public IdentExpr {
    IdentExpr *basename;
    std::string selector;
  public:
    ModuleSelector(const Loc &loc, IdentExpr *basename, std::string aSelector)
        : IdentExpr(loc), basename(basename), selector(std::move(aSelector)) {}
    [[nodiscard]] std::string getIdent() const override {
        if (auto ident = dynamic_cast<Identifier *>(basename); ident) {
            return ident->getIdent();
        }
        return basename->getIdent() + "::" + selector;
    }
};

class Literal : public AstExpr {
  public:
    explicit Literal(const Loc &loc) : AstExpr(loc) {}
};

class BasicLiteral : public Literal {
    std::string value;
  public:
    BasicLiteral(const Loc &loc, std::string value) : Literal(loc), value(std::move(value)) {}
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

class TypeExpr : public AstExpr {
  public:
    explicit TypeExpr(const Loc &loc) : AstExpr(loc) {}
};

class IdentifierType : public TypeExpr {
    IdentExpr *name;
  public:
    IdentifierType(const Loc &loc, IdentExpr *name) : TypeExpr(loc), name(name) {}
};

class Expression;

class ArrayType : public TypeExpr {
    AstExpr *elementTyp;
    Expression *lengthExpr;
  public:
    ArrayType(const Loc &loc, AstExpr *elementTyp, Expression *lengthExpr)
        : TypeExpr(loc), elementTyp(elementTyp), lengthExpr(lengthExpr) {}
};

class FunctionType : public TypeExpr {
    TypeExpr *returnTyp;
    std::vector<TypeExpr *> paramList;
  public:
    FunctionType(const Loc &loc, TypeExpr *returnTyp, std::vector<TypeExpr *> paramList)
        : TypeExpr(loc), returnTyp(returnTyp), paramList(std::move(paramList)) {}
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

class Expression : public AstExpr {
  public:
    explicit Expression(const Loc &loc) : AstExpr(loc) {}
};

class UnaryExpr : public Expression {
    UnaryOperator op;
    Expression *expr;
  public:
    UnaryExpr(const Loc &loc, UnaryOperator op, Expression *expr) : Expression(loc), op(op), expr(expr) {}
};

class BinaryExpr : public Expression {
    BinaryOperator op;
    Expression *lhs;
    Expression *rhs;
  public:
    BinaryExpr(const Loc &loc, BinaryOperator op, Expression *lhs, Expression *rhs)
        : Expression(loc), op(op), lhs(lhs), rhs(rhs) {}
};

class NewExpr : public Expression {
    AstExpr *instanceTyp;
  public:
    NewExpr(const Loc &loc, AstExpr *instanceTyp) : Expression(loc), instanceTyp(instanceTyp) {}
};

class CastExpr : public Expression {
    AstExpr *targetTyp;
    Expression *from;
  public:
    CastExpr(const Loc &loc, AstExpr *targetTyp, Expression *from)
        : Expression(loc), targetTyp(targetTyp), from(from) {}
};

class SelectorExpr : public Expression {
    Expression *baseExpr;
    IdentExpr *selector;
  public:
    SelectorExpr(const Loc &loc, Expression *baseExpr, IdentExpr *aSelector)
        : Expression(loc), baseExpr(baseExpr), selector(aSelector) {}
};

class ArgumentExpr : public Expression {
    Expression *baseExpr;
    std::vector<Expression *> arguments;
  public:
    ArgumentExpr(const Loc &loc, Expression *baseExpr, std::vector<Expression *> arguments)
        : Expression(loc), baseExpr(baseExpr), arguments(std::move(arguments)) {}
};

class Statement : public AstExpr {
  public:
    explicit Statement(const Loc &loc) : AstExpr(loc) {}
};

class SimpleStmt : public Statement {
  public:
    explicit SimpleStmt(const Loc &loc) : Statement(loc) {}
};

class VariableDecl : public Statement {
    Identifier *name;
    AstExpr *typ;
    Expression *initializer;
  public:
    VariableDecl(const Loc &loc, Identifier *name, AstExpr *typ, Expression *initializer)
        : Statement(loc), name(name), typ(typ), initializer(initializer) {}
};

class ReturnStmt : public Statement {
    Expression *returnValue;
  public:
    ReturnStmt(const Loc &loc, Expression *returnValue) : Statement(loc), returnValue(returnValue) {}
};

class BreakStmt : public Statement {};

class ContinueStmt : public Statement {};

class Block;

class IfStmt : public Statement {
    SimpleStmt *cond;
    Block *primaryBlock;
    Block *elseBlock;
  public:
    IfStmt(const Loc &loc, SimpleStmt *cond, Block *primaryBlock, Block *elseBlock)
        : Statement(loc), cond(cond), primaryBlock(primaryBlock), elseBlock(elseBlock) {}
};

class ForClause : public AstExpr {
  public:
    explicit ForClause(const Loc &loc) : AstExpr(loc) {}
};

class ForRangeClause : public ForClause {
    VariableDecl *variable;
    Expression *iterExpr;
  public:
    ForRangeClause(const Loc &loc, VariableDecl *variable, Expression *iterExpr)
        : ForClause(loc), variable(variable), iterExpr(iterExpr) {}
};

class ForNormalClause : public ForClause {
    AstExpr *init;
    Expression *cond;
    SimpleStmt *post;
  public:
    ForNormalClause(const Loc &loc, AstExpr *init, Expression *cond, SimpleStmt *post)
        : ForClause(loc), init(init), cond(cond), post(post) {}
};

class ForStmt : public Statement {
    ForClause *clause;
    Block *body;
  public:
    ForStmt(const Loc &loc, ForClause *clause, Block *body) : Statement(loc), clause(clause), body(body) {}
};

class WhileStmt : public Statement {
    SimpleStmt *cond;
    Block *body;
  public:
    WhileStmt(const Loc &loc, SimpleStmt *cond, Block *body) : Statement(loc), cond(cond), body(body) {}
};

class EmptyStmt : public SimpleStmt {};

class AssignmentStmt : public SimpleStmt {
    AssignOperator assignOp;
    Expression *lhs;
    Expression *rhs;
  public:
    AssignmentStmt(const Loc &loc, AssignOperator assignOp, Expression *lhs, Expression *rhs)
        : SimpleStmt(loc), assignOp(assignOp), lhs(lhs), rhs(rhs) {}
};

class IncDecStmt : public SimpleStmt {
    PostfixOperator postfixOp;
    Expression *expr;
  public:
    IncDecStmt(const Loc &loc, PostfixOperator postfixOp, Expression *expr)
        : SimpleStmt(loc), postfixOp(postfixOp), expr(expr) {}
};

class ExpressionStmt : public SimpleStmt {
    Expression *expr;
  public:
    ExpressionStmt(const Loc &loc, Expression *expr) : SimpleStmt(loc), expr(expr) {}
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
