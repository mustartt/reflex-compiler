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
#include "AstVisitor.h"

namespace reflex {

class Expression : public AstExpr {
  public:
    explicit Expression(const Loc &loc) : AstExpr(loc) {}
};

class IdentExpr : public Expression {
  public:
    explicit IdentExpr(const Loc &loc) : Expression(loc) {}
    [[nodiscard]] virtual std::string getIdent() const = 0;
};

class Identifier : public IdentExpr {
    std::string name;
  public:
    Identifier(const Loc &loc, std::string name) : IdentExpr(loc), name(std::move(name)) {}
    [[nodiscard]] std::string getIdent() const override { return name; }
    void accept(AstVisitor *visitor) override { visitor->visit(this); }
};

class ModuleSelector : public IdentExpr {
    IdentExpr *basename;
    std::string selector;
  public:
    ModuleSelector(const Loc &loc, IdentExpr *basename, std::string aSelector)
        : IdentExpr(loc), basename(basename), selector(std::move(aSelector)) {}
    [[nodiscard]] std::string getIdent() const override {
        return basename->getIdent() + "::" + selector;
    }
    void accept(AstVisitor *visitor) override { visitor->visit(this); }
};

class Literal : public Expression {
  public:
    explicit Literal(const Loc &loc) : Expression(loc) {}
};

class BasicLiteral : public Literal {
    std::string value;
  public:
    BasicLiteral(const Loc &loc, std::string value) : Literal(loc), value(std::move(value)) {}
    [[nodiscard]] const std::string &getValue() const { return value; }
};

class NumberLit : public BasicLiteral {
  public:
    NumberLit(const Loc &loc, const std::string &value) : BasicLiteral(loc, value) {}
    void accept(AstVisitor *visitor) override { visitor->visit(this); }
};

class StringLit : public BasicLiteral {
  public:
    StringLit(const Loc &loc, const std::string &value) : BasicLiteral(loc, value) {}
    void accept(AstVisitor *visitor) override { visitor->visit(this); }
};

class BoolLit : public BasicLiteral {
  public:
    BoolLit(const Loc &loc, const std::string &value) : BasicLiteral(loc, value) {}
    void accept(AstVisitor *visitor) override { visitor->visit(this); }
};

class NullLit : public BasicLiteral {
  public:
    NullLit(const Loc &loc, const std::string &value) : BasicLiteral(loc, value) {}
    void accept(AstVisitor *visitor) override { visitor->visit(this); }
};

class TypeExpr : public AstExpr {
  public:
    explicit TypeExpr(const Loc &loc) : AstExpr(loc) {}
};

class IdentifierType : public TypeExpr {
    IdentExpr *name;
  public:
    IdentifierType(const Loc &loc, IdentExpr *name) : TypeExpr(loc), name(name) {}
    void accept(AstVisitor *visitor) override { visitor->visit(this); }
    [[nodiscard]] IdentExpr *getTypename() const { return name; }
};

class Expression;

class ArrayType : public TypeExpr {
    AstExpr *elementTyp;
    Expression *lengthExpr;
  public:
    ArrayType(const Loc &loc, AstExpr *elementTyp, Expression *lengthExpr)
        : TypeExpr(loc), elementTyp(elementTyp), lengthExpr(lengthExpr) {}
    void accept(AstVisitor *visitor) override { visitor->visit(this); }
    [[nodiscard]] AstExpr *getElementTyp() const { return elementTyp; }
    [[nodiscard]] Expression *getLengthExpr() const { return lengthExpr; }
};

class FunctionType : public TypeExpr {
    TypeExpr *returnTyp;
    std::vector<TypeExpr *> paramList;
  public:
    FunctionType(const Loc &loc, TypeExpr *returnTyp, std::vector<TypeExpr *> paramList)
        : TypeExpr(loc), returnTyp(returnTyp), paramList(std::move(paramList)) {}
    void accept(AstVisitor *visitor) override { visitor->visit(this); }
    [[nodiscard]] TypeExpr *getReturnTyp() const { return returnTyp; }
    [[nodiscard]] const std::vector<TypeExpr *> &getParamList() const { return paramList; }
};

class ArrayLit : public Literal {
    std::vector<AstExpr *> initializerList;
  public:
    ArrayLit(const Loc &loc, std::vector<AstExpr *> initializerList)
        : Literal(loc), initializerList(std::move(initializerList)) {}
    void accept(AstVisitor *visitor) override { visitor->visit(this); }
    [[nodiscard]] const std::vector<AstExpr *> &getInitializerList() const { return initializerList; }
};

class Block;

class Parameter : public AstExpr {
    IdentExpr *name;
    AstExpr *typ;
  public:
    Parameter(const Loc &loc, IdentExpr *name, AstExpr *typ)
        : AstExpr(loc), name(name), typ(typ) {}
    void accept(AstVisitor *visitor) override { visitor->visit(this); }
    [[nodiscard]] IdentExpr *getName() const { return name; }
    [[nodiscard]] AstExpr *getTyp() const { return typ; }
};

class FunctionLit : public Literal {
    std::vector<Parameter *> parameters;
    TypeExpr *returnTyp;
    Block *body;
  public:
    FunctionLit(const Loc &loc, std::vector<Parameter *> parameters, TypeExpr *returnTyp, Block *body)
        : Literal(loc), parameters(std::move(parameters)), returnTyp(returnTyp), body(body) {}
    void accept(AstVisitor *visitor) override { visitor->visit(this); }
    [[nodiscard]] const std::vector<Parameter *> &getParameters() const { return parameters; }
    [[nodiscard]] TypeExpr *getReturnTyp() const { return returnTyp; }
    [[nodiscard]] Block *getBody() const { return body; }
};

class UnaryExpr : public Expression {
    UnaryOperator op;
    Expression *expr;
  public:
    UnaryExpr(const Loc &loc, UnaryOperator op, Expression *expr) : Expression(loc), op(op), expr(expr) {}
    void accept(AstVisitor *visitor) override { visitor->visit(this); }
    [[nodiscard]] UnaryOperator getOp() const { return op; }
    [[nodiscard]] Expression *getExpr() const { return expr; }
};

class BinaryExpr : public Expression {
    BinaryOperator op;
    Expression *lhs;
    Expression *rhs;
  public:
    BinaryExpr(const Loc &loc, BinaryOperator op, Expression *lhs, Expression *rhs)
        : Expression(loc), op(op), lhs(lhs), rhs(rhs) {}
    void accept(AstVisitor *visitor) override { visitor->visit(this); }
    [[nodiscard]] BinaryOperator getOp() const { return op; }
    [[nodiscard]] Expression *getLhs() const { return lhs; }
    [[nodiscard]] Expression *getRhs() const { return rhs; }
};

class NewExpr : public Expression {
    AstExpr *instanceTyp;
  public:
    NewExpr(const Loc &loc, AstExpr *instanceTyp) : Expression(loc), instanceTyp(instanceTyp) {}
    void accept(AstVisitor *visitor) override { visitor->visit(this); }
    [[nodiscard]] AstExpr *getInstanceTyp() const { return instanceTyp; }
};

class CastExpr : public Expression {
    AstExpr *targetTyp;
    Expression *from;
  public:
    CastExpr(const Loc &loc, AstExpr *targetTyp, Expression *from)
        : Expression(loc), targetTyp(targetTyp), from(from) {}
    void accept(AstVisitor *visitor) override { visitor->visit(this); }
    [[nodiscard]] AstExpr *getTargetTyp() const { return targetTyp; }
    [[nodiscard]] Expression *getFrom() const { return from; }
};

class IndexExpr : public Expression {
    Expression *baseExpr;
    Expression *index;
  public:
    IndexExpr(const Loc &loc, Expression *baseExpr, Expression *index)
        : Expression(loc), baseExpr(baseExpr), index(index) {}
    void accept(AstVisitor *visitor) override { visitor->visit(this); }
    [[nodiscard]] Expression *getBaseExpr() const { return baseExpr; }
    [[nodiscard]] Expression *getIndex() const { return index; }
};

class SelectorExpr : public Expression {
    Expression *baseExpr;
    IdentExpr *selector;
  public:
    SelectorExpr(const Loc &loc, Expression *baseExpr, IdentExpr *aSelector)
        : Expression(loc), baseExpr(baseExpr), selector(aSelector) {}
    void accept(AstVisitor *visitor) override { visitor->visit(this); }
    [[nodiscard]] Expression *getBaseExpr() const { return baseExpr; }
    [[nodiscard]] IdentExpr *getSelector() const { return selector; }
};

class ArgumentExpr : public Expression {
    Expression *baseExpr;
    std::vector<Expression *> arguments;
  public:
    ArgumentExpr(const Loc &loc, Expression *baseExpr, std::vector<Expression *> arguments)
        : Expression(loc), baseExpr(baseExpr), arguments(std::move(arguments)) {}
    void accept(AstVisitor *visitor) override { visitor->visit(this); }
    [[nodiscard]] Expression *getBaseExpr() const { return baseExpr; }
    [[nodiscard]] const std::vector<Expression *> &getArguments() const { return arguments; }
};

class Statement : public AstExpr {
  public:
    explicit Statement(const Loc &loc) : AstExpr(loc) {}
};

class SimpleStmt : public Statement {
  public:
    explicit SimpleStmt(const Loc &loc) : Statement(loc) {}
};

class Declaration : public Statement {
  public:
    explicit Declaration(const Loc &loc) : Statement(loc) {}
};

class VariableDecl : public Declaration {
    IdentExpr *name;
    AstExpr *typ;
    Expression *initializer;
  public:
    VariableDecl(const Loc &loc, IdentExpr *name, AstExpr *typ, Expression *initializer)
        : Declaration(loc), name(name), typ(typ), initializer(initializer) {}
    void accept(AstVisitor *visitor) override { visitor->visit(this); }
    [[nodiscard]] IdentExpr *getName() const { return name; }
    [[nodiscard]] AstExpr *getTyp() const { return typ; }
    [[nodiscard]] Expression *getInitializer() const { return initializer; }
};

class ClassDecl : public Declaration {
    IdentExpr *name;
    IdentExpr *baseClass;
    std::vector<IndexExpr *> interface;
  public:
    ClassDecl(const Loc &loc, IdentExpr *name, IdentExpr *baseClass, std::vector<IndexExpr *> anInterface)
        : Declaration(loc), name(name), baseClass(baseClass), interface(std::move(anInterface)) {}
};

class ReturnStmt : public Statement {
    Expression *returnValue;
  public:
    ReturnStmt(const Loc &loc, Expression *returnValue) : Statement(loc), returnValue(returnValue) {}
    void accept(AstVisitor *visitor) override { visitor->visit(this); }
    [[nodiscard]] Expression *getReturnValue() const { return returnValue; }
};

class BreakStmt : public Statement {
  public:
    explicit BreakStmt(const Loc &loc) : Statement(loc) {}
    void accept(AstVisitor *visitor) override { visitor->visit(this); }
};

class ContinueStmt : public Statement {
  public:
    explicit ContinueStmt(const Loc &loc) : Statement(loc) {}
    void accept(AstVisitor *visitor) override { visitor->visit(this); }
};

class Block;

class IfStmt : public Statement {
    SimpleStmt *cond;
    Block *primaryBlock;
    Block *elseBlock;
  public:
    IfStmt(const Loc &loc, SimpleStmt *cond, Block *primaryBlock, Block *elseBlock)
        : Statement(loc), cond(cond), primaryBlock(primaryBlock), elseBlock(elseBlock) {}
    void accept(AstVisitor *visitor) override { visitor->visit(this); }
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
    void accept(AstVisitor *visitor) override { visitor->visit(this); }
};

class ForNormalClause : public ForClause {
    AstExpr *init;
    Expression *cond;
    SimpleStmt *post;
  public:
    ForNormalClause(const Loc &loc, AstExpr *init, Expression *cond, SimpleStmt *post)
        : ForClause(loc), init(init), cond(cond), post(post) {}
    void accept(AstVisitor *visitor) override { visitor->visit(this); }
};

class ForStmt : public Statement {
    ForClause *clause;
    Block *body;
  public:
    ForStmt(const Loc &loc, ForClause *clause, Block *body) : Statement(loc), clause(clause), body(body) {}
    void accept(AstVisitor *visitor) override { visitor->visit(this); }
};

class WhileStmt : public Statement {
    SimpleStmt *cond;
    Block *body;
  public:
    WhileStmt(const Loc &loc, SimpleStmt *cond, Block *body) : Statement(loc), cond(cond), body(body) {}
    void accept(AstVisitor *visitor) override { visitor->visit(this); }
};

class EmptyStmt : public SimpleStmt {
  public:
    explicit EmptyStmt(const Loc &loc) : SimpleStmt(loc) {}
    void accept(AstVisitor *visitor) override { visitor->visit(this); }
};

class AssignmentStmt : public SimpleStmt {
    AssignOperator assignOp;
    Expression *lhs;
    Expression *rhs;
  public:
    AssignmentStmt(const Loc &loc, AssignOperator assignOp, Expression *lhs, Expression *rhs)
        : SimpleStmt(loc), assignOp(assignOp), lhs(lhs), rhs(rhs) {}
    void accept(AstVisitor *visitor) override { visitor->visit(this); }
    [[nodiscard]] AssignOperator getAssignOp() const { return assignOp; }
    [[nodiscard]] Expression *getLhs() const { return lhs; }
    [[nodiscard]] Expression *getRhs() const { return rhs; }
};

class IncDecStmt : public SimpleStmt {
    PostfixOperator postfixOp;
    Expression *expr;
  public:
    IncDecStmt(const Loc &loc, PostfixOperator postfixOp, Expression *expr)
        : SimpleStmt(loc), postfixOp(postfixOp), expr(expr) {}
    void accept(AstVisitor *visitor) override { visitor->visit(this); }
    [[nodiscard]] PostfixOperator getPostfixOp() const { return postfixOp; }
    [[nodiscard]] Expression *getExpr() const { return expr; }
};

class ExpressionStmt : public SimpleStmt {
    Expression *expr;
  public:
    ExpressionStmt(const Loc &loc, Expression *expr) : SimpleStmt(loc), expr(expr) {}
    void accept(AstVisitor *visitor) override { visitor->visit(this); }
    [[nodiscard]] Expression *getExpr() const { return expr; }
};

class Block : public Statement {
    std::vector<Statement *> stmts;
  public:
    Block(const Loc &loc, std::vector<Statement *> stmts)
        : Statement(loc), stmts(std::move(stmts)) {}
    void accept(AstVisitor *visitor) override { visitor->visit(this); }
    [[nodiscard]] const std::vector<Statement *> &getStmts() const { return stmts; }
};

}

#endif //REFLEX_SRC_AST_ASTNODES_H_
