//
// Created by Admininstrator on 2022-04-23.
//

#include "ASTExpression.h"

#include <utility>

namespace reflex {

std::string Identifier::getReferenceName() const {
    return reference;
}

const std::string &Identifier::getBaseRefName() const {
    return reference;
}

std::string ModuleSelector::getReferenceName() const {
    return prefix + "::" + child->getReferenceName();
}

const std::string &ModuleSelector::getBaseRefName() const {
    return child->getBaseRefName();
}

Expression::Expression(const SourceLocation *loc) : ASTNode(loc) {}

DeclRefExpr::DeclRefExpr(const SourceLocation *loc, Declaration *decl) : Expression(loc), decl(decl) {}

Identifier::Identifier(const SourceLocation *loc, Declaration *decl, std::string reference)
    : DeclRefExpr(loc, decl),
      reference(std::move(reference)) {}

ModuleSelector::ModuleSelector(const SourceLocation *loc,
                               Declaration *decl,
                               std::string prefix,
                               DeclRefExpr *child)
    : DeclRefExpr(loc, decl), prefix(std::move(prefix)), child(child) {}

UnaryExpr::UnaryExpr(const SourceLocation *loc, Operator::UnaryOperator op, Expression *expr)
    : Expression(loc), op(op), expr(expr) {}

BinaryExpr::BinaryExpr(const SourceLocation *loc, Operator::BinaryOperator op,
                       Expression *lhs, Expression *rhs)
    : Expression(loc), op(op), lhs(lhs), rhs(rhs) {}

NewExpr::NewExpr(const SourceLocation *loc, ASTTypeExpr *instanceType)
    : Expression(loc), instanceType(instanceType) {}

CastExpr::CastExpr(const SourceLocation *loc, ASTTypeExpr *resultType, Expression *from)
    : Expression(loc), resultType(resultType), from(from) {}

IndexExpr::IndexExpr(const SourceLocation *loc, Expression *expr, Expression *index)
    : Expression(loc), expr(expr), index(index) {}

SelectorExpr::SelectorExpr(const SourceLocation *loc, Expression *expr, std::string aSelector)
    : Expression(loc), expr(expr), selector(std::move(aSelector)) {}

ArgumentExpr::ArgumentExpr(const SourceLocation *loc, Expression *expr, std::vector<Expression *> arguments)
    : Expression(loc), expr(expr), arguments(std::move(arguments)) {}

}
