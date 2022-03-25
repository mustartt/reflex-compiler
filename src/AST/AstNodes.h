//
// Created by henry on 2022-03-20.
//

#ifndef REFLEX_SRC_AST_ASTNODES_H_
#define REFLEX_SRC_AST_ASTNODES_H_

#include "Ast.h"
#include <string>

#include <utility>
#include <vector>
#include <optional>

#include "../Lexer/Token.h"
#include "Operator.h"
#include "AstVisitor.h"

namespace reflex {

class Expression : public AstExpr {
  public:
    explicit Expression(const Loc &loc);
};

class IdentExpr : public Expression {
  public:
    explicit IdentExpr(const Loc &loc);
    [[nodiscard]] virtual std::string getIdent() const = 0;
};

class Identifier : public IdentExpr {
    std::string name;
  public:
    Identifier(const Loc &loc, std::string name);
    [[nodiscard]] std::string getIdent() const override;
    void accept(AstVisitor *visitor) override;
};

class ModuleSelector : public IdentExpr {
    IdentExpr *basename;
    std::string selector;
  public:
    ModuleSelector(const Loc &loc, IdentExpr *basename, std::string aSelector);
    [[nodiscard]] std::string getIdent() const override;
    void accept(AstVisitor *visitor) override;
    [[nodiscard]] const std::string &getSelector() const;
    [[nodiscard]] IdentExpr *getBasename() const;
};

class Literal : public Expression {
  public:
    explicit Literal(const Loc &loc);
};

class BasicLiteral : public Literal {
    std::string value;
  public:
    BasicLiteral(const Loc &loc, std::string value);
    [[nodiscard]] const std::string &getValue() const;
};

class NumberLit : public BasicLiteral {
  public:
    NumberLit(const Loc &loc, const std::string &value);
    void accept(AstVisitor *visitor) override;
};

class StringLit : public BasicLiteral {
  public:
    StringLit(const Loc &loc, const std::string &value);
    void accept(AstVisitor *visitor) override;
};

class BoolLit : public BasicLiteral {
  public:
    BoolLit(const Loc &loc, const std::string &value);
    void accept(AstVisitor *visitor) override;
};

class NullLit : public BasicLiteral {
  public:
    NullLit(const Loc &loc, const std::string &value);
    void accept(AstVisitor *visitor) override;
};

class TypeExpr : public AstExpr {
  public:
    explicit TypeExpr(const Loc &loc);
};

class IdentifierTypeExpr : public TypeExpr {
    IdentExpr *name;
  public:
    IdentifierTypeExpr(const Loc &loc, IdentExpr *name);
    void accept(AstVisitor *visitor) override;
    [[nodiscard]] IdentExpr *getTypename() const;
};

class Expression;

class ArrayTypeExpr : public TypeExpr {
    TypeExpr *elementTyp;
    Expression *lengthExpr;
  public:
    ArrayTypeExpr(const Loc &loc, TypeExpr *elementTyp, Expression *lengthExpr);
    void accept(AstVisitor *visitor) override;
    [[nodiscard]] TypeExpr *getElementTyp() const;
    [[nodiscard]] Expression *getLengthExpr() const;
};

class FunctionTypeExpr : public TypeExpr {
    TypeExpr *returnTyp;
    std::vector<TypeExpr *> paramList;
  public:
    FunctionTypeExpr(const Loc &loc, TypeExpr *returnTyp, std::vector<TypeExpr *> paramList);
    void accept(AstVisitor *visitor) override;
    [[nodiscard]] TypeExpr *getReturnTyp() const;
    [[nodiscard]] const std::vector<TypeExpr *> &getParamList() const;
};

class ArrayLit : public Literal {
    std::vector<AstExpr *> initializerList;
  public:
    ArrayLit(const Loc &loc, std::vector<AstExpr *> initializerList);
    void accept(AstVisitor *visitor) override;
    [[nodiscard]] const std::vector<AstExpr *> &getInitializerList() const;
};

class ParamDecl : public AstExpr {
    IdentExpr *name;
    TypeExpr *typ;
  public:
    ParamDecl(const Loc &loc, IdentExpr *name, TypeExpr *typ);
    void accept(AstVisitor *visitor) override;
    [[nodiscard]] IdentExpr *getName() const;
    [[nodiscard]] TypeExpr *getParamType() const;
};

class Block;
class FunctionLit : public Literal {
    std::vector<ParamDecl *> parameters;
    TypeExpr *returnTyp;
    Block *body;
  public:
    FunctionLit(const Loc &loc, std::vector<ParamDecl *> parameters, TypeExpr *returnTyp, Block *body);
    void accept(AstVisitor *visitor) override;
    [[nodiscard]] const std::vector<ParamDecl *> &getParameters() const;
    [[nodiscard]] TypeExpr *getReturnTyp() const;
    [[nodiscard]] Block *getBody() const;
};

class UnaryExpr : public Expression {
    UnaryOperator op;
    Expression *expr;
  public:
    UnaryExpr(const Loc &loc, UnaryOperator op, Expression *expr);
    void accept(AstVisitor *visitor) override;
    [[nodiscard]] UnaryOperator getOp() const;
    [[nodiscard]] Expression *getExpr() const;
};

class BinaryExpr : public Expression {
    BinaryOperator op;
    Expression *lhs;
    Expression *rhs;
  public:
    BinaryExpr(const Loc &loc, BinaryOperator op, Expression *lhs, Expression *rhs);
    void accept(AstVisitor *visitor) override;
    [[nodiscard]] BinaryOperator getOp() const;
    [[nodiscard]] Expression *getLhs() const;
    [[nodiscard]] Expression *getRhs() const;
};

class NewExpr : public Expression {
    AstExpr *instanceTyp;
  public:
    NewExpr(const Loc &loc, AstExpr *instanceTyp);
    void accept(AstVisitor *visitor) override;
    [[nodiscard]] AstExpr *getInstanceTyp() const;
};

class CastExpr : public Expression {
    AstExpr *targetTyp;
    Expression *from;
  public:
    CastExpr(const Loc &loc, AstExpr *targetTyp, Expression *from);
    void accept(AstVisitor *visitor) override;
    [[nodiscard]] AstExpr *getTargetTyp() const;
    [[nodiscard]] Expression *getFrom() const;
};

class IndexExpr : public Expression {
    Expression *baseExpr;
    Expression *index;
  public:
    IndexExpr(const Loc &loc, Expression *baseExpr, Expression *index);
    void accept(AstVisitor *visitor) override;
    [[nodiscard]] Expression *getBaseExpr() const;
    [[nodiscard]] Expression *getIndex() const;
};

class SelectorExpr : public Expression {
    Expression *baseExpr;
    IdentExpr *selector;
  public:
    SelectorExpr(const Loc &loc, Expression *baseExpr, IdentExpr *aSelector);
    void accept(AstVisitor *visitor) override;
    [[nodiscard]] Expression *getBaseExpr() const;
    [[nodiscard]] IdentExpr *getSelector() const;
};

class ArgumentExpr : public Expression {
    Expression *baseExpr;
    std::vector<Expression *> arguments;
  public:
    ArgumentExpr(const Loc &loc, Expression *baseExpr, std::vector<Expression *> arguments);
    void accept(AstVisitor *visitor) override;
    [[nodiscard]] Expression *getBaseExpr() const;
    [[nodiscard]] const std::vector<Expression *> &getArguments() const;
};

class Statement : public AstExpr {
  public:
    explicit Statement(const Loc &loc);
};

class SimpleStmt : public Statement {
  public:
    explicit SimpleStmt(const Loc &loc);
};

class Declaration : public Statement {
  public:
    explicit Declaration(const Loc &loc);
};

class VariableDecl : public Declaration {
    IdentExpr *name;
    TypeExpr *typ;
    Expression *initializer;
  public:
    VariableDecl(const Loc &loc, IdentExpr *name, TypeExpr *typ, Expression *initializer);
    void accept(AstVisitor *visitor) override;
    [[nodiscard]] IdentExpr *getName() const;
    [[nodiscard]] TypeExpr *getVariableType() const;
    [[nodiscard]] Expression *getInitializer() const;
};

class ReturnStmt : public Statement {
    Expression *returnValue;
  public:
    ReturnStmt(const Loc &loc, Expression *returnValue);
    void accept(AstVisitor *visitor) override;
    [[nodiscard]] Expression *getReturnValue() const;
};

class BreakStmt : public Statement {
  public:
    explicit BreakStmt(const Loc &loc);
    void accept(AstVisitor *visitor) override;
};

class ContinueStmt : public Statement {
  public:
    explicit ContinueStmt(const Loc &loc);
    void accept(AstVisitor *visitor) override;
};

class Block;

class IfStmt : public Statement {
    SimpleStmt *cond;
    Block *primaryBlock;
    Block *elseBlock;
  public:
    IfStmt(const Loc &loc, SimpleStmt *cond, Block *primaryBlock, Block *elseBlock);
    void accept(AstVisitor *visitor) override;
    [[nodiscard]] SimpleStmt *getCond() const;
    [[nodiscard]] Block *getPrimaryBlock() const;
    [[nodiscard]] Block *getElseBlock() const;
};

class ForClause : public AstExpr {
  public:
    explicit ForClause(const Loc &loc) : AstExpr(loc) {}
};

class ForRangeClause : public ForClause {
    VariableDecl *variable;
    Expression *iterExpr;
  public:
    ForRangeClause(const Loc &loc, VariableDecl *variable, Expression *iterExpr);
    void accept(AstVisitor *visitor) override;
    [[nodiscard]] VariableDecl *getVariable() const;
    [[nodiscard]] Expression *getIterExpr() const;
};

class ForNormalClause : public ForClause {
    Statement *init;
    Expression *cond;
    SimpleStmt *post;
  public:
    ForNormalClause(const Loc &loc, Statement *init, Expression *cond, SimpleStmt *post);
    void accept(AstVisitor *visitor) override;
    [[nodiscard]] Statement *getInit() const;
    [[nodiscard]] Expression *getCond() const;
    [[nodiscard]] SimpleStmt *getPost() const;
};

class ForStmt : public Statement {
    ForClause *clause;
    Block *body;
  public:
    ForStmt(const Loc &loc, ForClause *clause, Block *body);
    void accept(AstVisitor *visitor) override;
    [[nodiscard]] ForClause *getClause() const;
    [[nodiscard]] Block *getBody() const;
};

class WhileStmt : public Statement {
    SimpleStmt *cond;
    Block *body;
  public:
    WhileStmt(const Loc &loc, SimpleStmt *cond, Block *body);
    void accept(AstVisitor *visitor) override;
    [[nodiscard]] SimpleStmt *getCond() const;
    [[nodiscard]] Block *getBody() const;
};

class EmptyStmt : public SimpleStmt {
  public:
    explicit EmptyStmt(const Loc &loc);
    void accept(AstVisitor *visitor) override;
};

class AssignmentStmt : public SimpleStmt {
    AssignOperator assignOp;
    Expression *lhs;
    Expression *rhs;
  public:
    AssignmentStmt(const Loc &loc, AssignOperator assignOp, Expression *lhs, Expression *rhs);
    void accept(AstVisitor *visitor) override;
    [[nodiscard]] AssignOperator getAssignOp() const;
    [[nodiscard]] Expression *getLhs() const;
    [[nodiscard]] Expression *getRhs() const;
};

class IncDecStmt : public SimpleStmt {
    PostfixOperator postfixOp;
    Expression *expr;
  public:
    IncDecStmt(const Loc &loc, PostfixOperator postfixOp, Expression *expr);
    void accept(AstVisitor *visitor) override;
    [[nodiscard]] PostfixOperator getPostfixOp() const;
    [[nodiscard]] Expression *getExpr() const;
};

class ExpressionStmt : public SimpleStmt {
    Expression *expr;
  public:
    ExpressionStmt(const Loc &loc, Expression *expr);
    void accept(AstVisitor *visitor) override;
    [[nodiscard]] Expression *getExpr() const;
};

class Block : public Statement {
    std::vector<Statement *> stmts;
  public:
    Block(const Loc &loc, std::vector<Statement *> stmts);
    void accept(AstVisitor *visitor) override;
    [[nodiscard]] const std::vector<Statement *> &getStmts() const;
};

class FunctionDecl : public Declaration {
    IdentExpr *name;
    std::vector<ParamDecl *> params;
    TypeExpr *retTyp;
    Block *body;
  public:
    FunctionDecl(const Loc &loc, IdentExpr *name, std::vector<ParamDecl *> params, TypeExpr *retTyp, Block *body);
    void accept(AstVisitor *visitor) override;
    [[nodiscard]] IdentExpr *getName() const;
    [[nodiscard]] const std::vector<ParamDecl *> &getParams() const;
    [[nodiscard]] TypeExpr *getRetTyp() const;
    [[nodiscard]] Block *getBody() const;
};

class MemberDecl : public Declaration {
    IdentExpr *modifier;
    Declaration *declaration;
  public:
    MemberDecl(const Loc &loc, IdentExpr *modifier, Declaration *declaration);
    void accept(AstVisitor *visitor) override;
    [[nodiscard]] IdentExpr *getModifier() const;
    [[nodiscard]] Declaration *getDeclaration() const;
};

class ClassDecl : public Declaration {
    IdentExpr *name;
    IdentExpr *baseclass;
    std::vector<IdentExpr *> interfaces;
    std::vector<MemberDecl *> members;
  public:
    ClassDecl(const Loc &loc,
              IdentExpr *name,
              IdentExpr *baseclass,
              std::vector<IdentExpr *> interfaces,
              std::vector<MemberDecl *> members);
    void accept(AstVisitor *visitor) override;
    [[nodiscard]] IdentExpr *getName() const;
    [[nodiscard]] IdentExpr *getBaseclass() const { return baseclass; }
    [[nodiscard]] const std::vector<IdentExpr *> &getInterfaces() const;
    [[nodiscard]] const std::vector<MemberDecl *> &getMembers() const;
    [[nodiscard]] std::optional<std::string> getBaseClassname() const;
    [[nodiscard]] std::string getClassname() const;
    [[nodiscard]] bool hasBaseclass() const;
};

class InterfaceDecl : public Declaration {
    IdentExpr *name;
    std::vector<IdentExpr *> interfaces;
    std::vector<MemberDecl *> signatures;
  public:
    InterfaceDecl(const Loc &loc,
                  IdentExpr *name,
                  std::vector<IdentExpr *> interfaces,
                  std::vector<MemberDecl *> signatures);
    void accept(AstVisitor *visitor) override;
    [[nodiscard]] IdentExpr *getName() const;
    [[nodiscard]] std::string getInterfaceName() const;
    [[nodiscard]] const std::vector<IdentExpr *> &getInterfaces() const;
    [[nodiscard]] const std::vector<MemberDecl *> &getSignatures() const;
};

class CompilationUnit : public AstExpr {
    std::vector<Declaration *> decls;
  public:
    CompilationUnit(const Loc &loc, std::vector<Declaration *> decls);
    void accept(AstVisitor *visitor) override;
    [[nodiscard]] const std::vector<Declaration *> &getDecls() const;
};

}

#endif //REFLEX_SRC_AST_ASTNODES_H_
