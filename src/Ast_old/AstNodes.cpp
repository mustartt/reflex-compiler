//
// Created by henry on 2022-03-23.
//

#include "AstNodes.h"

namespace reflex {

Expression::Expression(const SourceLocation &loc) : AstExpr(loc) {}
IdentExpr::IdentExpr(const SourceLocation &loc) : Expression(loc) {}
Identifier::Identifier(const SourceLocation &loc, std::string name) : IdentExpr(loc), name(std::move(name)) {}
void Identifier::accept(AstVisitor *visitor) { visitor->visit(this); }
std::string Identifier::getIdent() const { return name; }

ModuleSelector::ModuleSelector(const SourceLocation &loc, IdentExpr *basename, std::string aSelector)
    : IdentExpr(loc), basename(basename), selector(std::move(aSelector)) {}
std::string ModuleSelector::getIdent() const {
    return basename->getIdent() + "::" + selector;
}
void ModuleSelector::accept(AstVisitor *visitor) { visitor->visit(this); }
const std::string &ModuleSelector::getSelector() const { return selector; }
IdentExpr *ModuleSelector::getBasename() const { return basename; }

Literal::Literal(const SourceLocation &loc) : Expression(loc) {}

BasicLiteral::BasicLiteral(const SourceLocation &loc, std::string value) : Literal(loc), value(std::move(value)) {}
const std::string &BasicLiteral::getValue() const { return value; }

NumberLit::NumberLit(const SourceLocation &loc, const std::string &value) : BasicLiteral(loc, value) {}
void NumberLit::accept(AstVisitor *visitor) { visitor->visit(this); }

StringLit::StringLit(const SourceLocation &loc, const std::string &value) : BasicLiteral(loc, value) {}
void StringLit::accept(AstVisitor *visitor) { visitor->visit(this); }

BoolLit::BoolLit(const SourceLocation &loc, const std::string &value) : BasicLiteral(loc, value) {}
void BoolLit::accept(AstVisitor *visitor) { visitor->visit(this); }

NullLit::NullLit(const SourceLocation &loc, const std::string &value) : BasicLiteral(loc, value) {}
void NullLit::accept(AstVisitor *visitor) { visitor->visit(this); }

TypeExpr::TypeExpr(const SourceLocation &loc) : AstExpr(loc) {}

IdentifierTypeExpr::IdentifierTypeExpr(const SourceLocation &loc, IdentExpr *name) : TypeExpr(loc), name(name) {}
void IdentifierTypeExpr::accept(AstVisitor *visitor) { visitor->visit(this); }
IdentExpr *IdentifierTypeExpr::getTypename() const { return name; }

ArrayTypeExpr::ArrayTypeExpr(const SourceLocation &loc, TypeExpr *elementTyp, Expression *lengthExpr)
    : TypeExpr(loc), elementTyp(elementTyp), lengthExpr(lengthExpr) {}
void ArrayTypeExpr::accept(AstVisitor *visitor) { visitor->visit(this); }
TypeExpr *ArrayTypeExpr::getElementTyp() const { return elementTyp; }
Expression *ArrayTypeExpr::getLengthExpr() const { return lengthExpr; }

FunctionTypeExpr::FunctionTypeExpr(const SourceLocation &loc, TypeExpr *returnTyp, std::vector<TypeExpr *> paramList)
    : TypeExpr(loc), returnTyp(returnTyp), paramList(std::move(paramList)) {}
void FunctionTypeExpr::accept(AstVisitor *visitor) { visitor->visit(this); }
TypeExpr *FunctionTypeExpr::getReturnTyp() const { return returnTyp; }
const std::vector<TypeExpr *> &FunctionTypeExpr::getParamList() const { return paramList; }

ArrayLit::ArrayLit(const SourceLocation &loc, std::vector<AstExpr *> initializerList)
    : Literal(loc), initializerList(std::move(initializerList)) {}
void ArrayLit::accept(AstVisitor *visitor) { visitor->visit(this); }
const std::vector<AstExpr *> &ArrayLit::getInitializerList() const { return initializerList; }

ParamDecl::ParamDecl(const SourceLocation &loc, IdentExpr *name, TypeExpr *typ)
    : AstExpr(loc), name(name), typ(typ) {}
void ParamDecl::accept(AstVisitor *visitor) { visitor->visit(this); }
IdentExpr *ParamDecl::getName() const { return name; }
TypeExpr *ParamDecl::getParamType() const { return typ; }

FunctionLit::FunctionLit(const SourceLocation &loc, std::vector<ParamDecl *> parameters, TypeExpr *returnTyp, Block *body)
    : Literal(loc), parameters(std::move(parameters)), returnTyp(returnTyp), body(body) {}
void FunctionLit::accept(AstVisitor *visitor) { visitor->visit(this); }
const std::vector<ParamDecl *> &FunctionLit::getParameters() const { return parameters; }
TypeExpr *FunctionLit::getReturnTyp() const { return returnTyp; }
Block *FunctionLit::getBody() const { return body; }

UnaryExpr::UnaryExpr(const SourceLocation &loc, UnaryOperator op, Expression *expr) : Expression(loc), op(op), expr(expr) {}
void UnaryExpr::accept(AstVisitor *visitor) { visitor->visit(this); }
UnaryOperator UnaryExpr::getOp() const { return op; }
Expression *UnaryExpr::getExpr() const { return expr; }

BinaryExpr::BinaryExpr(const SourceLocation &loc, BinaryOperator op, Expression *lhs, Expression *rhs)
    : Expression(loc), op(op), lhs(lhs), rhs(rhs) {}
void BinaryExpr::accept(AstVisitor *visitor) { visitor->visit(this); }
BinaryOperator BinaryExpr::getOp() const { return op; }
Expression *BinaryExpr::getLhs() const { return lhs; }
Expression *BinaryExpr::getRhs() const { return rhs; }

NewExpr::NewExpr(const SourceLocation &loc, TypeExpr *instanceTyp) : Expression(loc), instanceTyp(instanceTyp) {}
void NewExpr::accept(AstVisitor *visitor) { visitor->visit(this); }
TypeExpr *NewExpr::getInstanceTyp() const { return instanceTyp; }

CastExpr::CastExpr(const SourceLocation &loc, TypeExpr *targetTyp, Expression *from)
    : Expression(loc), targetTyp(targetTyp), from(from) {}
void CastExpr::accept(AstVisitor *visitor) { visitor->visit(this); }
TypeExpr *CastExpr::getTargetTyp() const { return targetTyp; }
Expression *CastExpr::getFrom() const { return from; }

IndexExpr::IndexExpr(const SourceLocation &loc, Expression *baseExpr, Expression *index)
    : Expression(loc), baseExpr(baseExpr), index(index) {}
void IndexExpr::accept(AstVisitor *visitor) { visitor->visit(this); }
Expression *IndexExpr::getBaseExpr() const { return baseExpr; }
Expression *IndexExpr::getIndex() const { return index; }

SelectorExpr::SelectorExpr(const SourceLocation &loc, Expression *baseExpr, IdentExpr *aSelector)
    : Expression(loc), baseExpr(baseExpr), selector(aSelector) {}
void SelectorExpr::accept(AstVisitor *visitor) { visitor->visit(this); }
Expression *SelectorExpr::getBaseExpr() const { return baseExpr; }
IdentExpr *SelectorExpr::getSelector() const { return selector; }

ArgumentExpr::ArgumentExpr(const SourceLocation &loc, Expression *baseExpr, std::vector<Expression *> arguments)
    : Expression(loc), baseExpr(baseExpr), arguments(std::move(arguments)) {}
void ArgumentExpr::accept(AstVisitor *visitor) { visitor->visit(this); }
Expression *ArgumentExpr::getBaseExpr() const { return baseExpr; }
const std::vector<Expression *> &ArgumentExpr::getArguments() const { return arguments; }

Statement::Statement(const SourceLocation &loc) : AstExpr(loc) {}
SimpleStmt::SimpleStmt(const SourceLocation &loc) : Statement(loc) {}
Declaration::Declaration(const SourceLocation &loc, Identifier *name) : Statement(loc), name(name) {}
Identifier *Declaration::getName() const { return name; }

VariableDecl::VariableDecl(const SourceLocation &loc, Identifier *name, TypeExpr *typ, Expression *initializer)
    : Declaration(loc, name), typ(typ), initializer(initializer) {}
void VariableDecl::accept(AstVisitor *visitor) { visitor->visit(this); }
TypeExpr *VariableDecl::getVariableType() const { return typ; }
Expression *VariableDecl::getInitializer() const { return initializer; }

ReturnStmt::ReturnStmt(const SourceLocation &loc, Expression *returnValue) : Statement(loc), returnValue(returnValue) {}
void ReturnStmt::accept(AstVisitor *visitor) { visitor->visit(this); }
Expression *ReturnStmt::getReturnValue() const { return returnValue; }

BreakStmt::BreakStmt(const SourceLocation &loc) : Statement(loc) {}
void BreakStmt::accept(AstVisitor *visitor) { visitor->visit(this); }

ContinueStmt::ContinueStmt(const SourceLocation &loc) : Statement(loc) {}
void ContinueStmt::accept(AstVisitor *visitor) { visitor->visit(this); }

IfStmt::IfStmt(const SourceLocation &loc, SimpleStmt *cond, Block *primaryBlock, Block *elseBlock)
    : Statement(loc), cond(cond), primaryBlock(primaryBlock), elseBlock(elseBlock) {}
void IfStmt::accept(AstVisitor *visitor) { visitor->visit(this); }
SimpleStmt *IfStmt::getCond() const { return cond; }
Block *IfStmt::getPrimaryBlock() const { return primaryBlock; }
Block *IfStmt::getElseBlock() const { return elseBlock; }

ForRangeClause::ForRangeClause(const SourceLocation &loc, VariableDecl *variable, Expression *iterExpr)
    : ForClause(loc), variable(variable), iterExpr(iterExpr) {}
void ForRangeClause::accept(AstVisitor *visitor) { visitor->visit(this); }
VariableDecl *ForRangeClause::getVariable() const { return variable; }
Expression *ForRangeClause::getIterExpr() const { return iterExpr; }

ForNormalClause::ForNormalClause(const SourceLocation &loc, Statement *init, Expression *cond, SimpleStmt *post)
    : ForClause(loc), init(init), cond(cond), post(post) {}
void ForNormalClause::accept(AstVisitor *visitor) { visitor->visit(this); }
Statement *ForNormalClause::getInit() const { return init; }
Expression *ForNormalClause::getCond() const { return cond; }
SimpleStmt *ForNormalClause::getPost() const { return post; }

ForStmt::ForStmt(const SourceLocation &loc, ForClause *clause, Block *body) : Statement(loc), clause(clause), body(body) {}
void ForStmt::accept(AstVisitor *visitor) { visitor->visit(this); }
ForClause *ForStmt::getClause() const { return clause; }
Block *ForStmt::getBody() const { return body; }

WhileStmt::WhileStmt(const SourceLocation &loc, SimpleStmt *cond, Block *body) : Statement(loc), cond(cond), body(body) {}
void WhileStmt::accept(AstVisitor *visitor) { visitor->visit(this); }
SimpleStmt *WhileStmt::getCond() const { return cond; }
Block *WhileStmt::getBody() const { return body; }

EmptyStmt::EmptyStmt(const SourceLocation &loc) : SimpleStmt(loc) {}
void EmptyStmt::accept(AstVisitor *visitor) { visitor->visit(this); }

AssignmentStmt::AssignmentStmt(const SourceLocation &loc, AssignOperator assignOp, Expression *lhs, Expression *rhs)
    : SimpleStmt(loc), assignOp(assignOp), lhs(lhs), rhs(rhs) {}
void AssignmentStmt::accept(AstVisitor *visitor) { visitor->visit(this); }
AssignOperator AssignmentStmt::getAssignOp() const { return assignOp; }
Expression *AssignmentStmt::getLhs() const { return lhs; }
Expression *AssignmentStmt::getRhs() const { return rhs; }

IncDecStmt::IncDecStmt(const SourceLocation &loc, PostfixOperator postfixOp, Expression *expr)
    : SimpleStmt(loc), postfixOp(postfixOp), expr(expr) {}
void IncDecStmt::accept(AstVisitor *visitor) { visitor->visit(this); }
PostfixOperator IncDecStmt::getPostfixOp() const { return postfixOp; }
Expression *IncDecStmt::getExpr() const { return expr; }

ExpressionStmt::ExpressionStmt(const SourceLocation &loc, Expression *expr) : SimpleStmt(loc), expr(expr) {}
void ExpressionStmt::accept(AstVisitor *visitor) { visitor->visit(this); }
Expression *ExpressionStmt::getExpr() const { return expr; }

Block::Block(const SourceLocation &loc, std::vector<Statement *> stmts)
    : Statement(loc), stmts(std::move(stmts)) {}
void Block::accept(AstVisitor *visitor) { visitor->visit(this); }
const std::vector<Statement *> &Block::getStmts() const { return stmts; }

FunctionDecl::FunctionDecl(const SourceLocation &loc,
                           Identifier *name,
                           std::vector<ParamDecl *> params,
                           TypeExpr *retTyp,
                           Block *body)
    : Declaration(loc, name), params(std::move(params)), retTyp(retTyp), body(body) {}
void FunctionDecl::accept(AstVisitor *visitor) { visitor->visit(this); }
const std::vector<ParamDecl *> &FunctionDecl::getParams() const { return params; }
TypeExpr *FunctionDecl::getRetTyp() const { return retTyp; }
Block *FunctionDecl::getBody() const { return body; }

MemberDecl::MemberDecl(const SourceLocation &loc, IdentExpr *modifier, Declaration *declaration)
    : Declaration(loc, declaration->getName()), modifier(modifier), declaration(declaration) {}
void MemberDecl::accept(AstVisitor *visitor) { visitor->visit(this); }
IdentExpr *MemberDecl::getModifier() const { return modifier; }
Declaration *MemberDecl::getDeclaration() const { return declaration; }

ClassDecl::ClassDecl(const SourceLocation &loc,
                     Identifier *name,
                     IdentExpr *baseclass,
                     std::vector<IdentExpr *> interfaces,
                     std::vector<MemberDecl *> members)
    : Declaration(loc, name),
      baseclass(baseclass),
      interfaces(std::move(interfaces)),
      members(std::move(members)) {}
void ClassDecl::accept(AstVisitor *visitor) { visitor->visit(this); }
const std::vector<IdentExpr *> &ClassDecl::getInterfaces() const { return interfaces; }
const std::vector<MemberDecl *> &ClassDecl::getMembers() const { return members; }
std::optional<std::string> ClassDecl::getBaseClassname() const {
    if (baseclass) return baseclass->getIdent();
    return std::nullopt;
}
std::string ClassDecl::getClassname() const { return name->getIdent(); }
bool ClassDecl::hasBaseclass() const { return baseclass; }

InterfaceDecl::InterfaceDecl(const SourceLocation &loc,
                             Identifier *name,
                             std::vector<IdentExpr *> interfaces,
                             std::vector<MemberDecl *> signatures)
    : Declaration(loc, name),
      interfaces(std::move(interfaces)),
      signatures(std::move(signatures)) {}
void InterfaceDecl::accept(AstVisitor *visitor) { visitor->visit(this); }
const std::vector<IdentExpr *> &InterfaceDecl::getInterfaces() const { return interfaces; }
const std::vector<MemberDecl *> &InterfaceDecl::getInterfaceMembers() const { return signatures; }
std::string InterfaceDecl::getInterfaceName() const { return name->getIdent(); }

CompilationUnit::CompilationUnit(const SourceLocation &loc, std::vector<Declaration *> decls)
    : AstExpr(loc), decls(std::move(decls)) {}
void CompilationUnit::accept(AstVisitor *visitor) { visitor->visit(this); }
const std::vector<Declaration *> &CompilationUnit::getDecls() const { return decls; }

}

