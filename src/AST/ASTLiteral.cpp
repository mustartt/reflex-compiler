//
// Created by Admininstrator on 2022-04-23.
//

#include "ASTLiteral.h"

#include <utility>
#include <sstream>

namespace reflex {

BadLiteralError::BadLiteralError(const std::string &arg)
    : runtime_error("Bad Literal: " + arg) {}

Literal::Literal(const SourceLocation *loc) : Expression(loc) {}

BasicLiteral::BasicLiteral(const SourceLocation *loc, std::string value)
    : Literal(loc), value(std::move(value)) {}

NumberLiteral::NumberLiteral(const SourceLocation *loc, std::string str)
    : BasicLiteral(loc, std::move(str)) {
    std::stringstream ss(value);
    double i;
    if ((ss >> i).fail() || !(ss >> std::ws).eof()) {
        throw BadLiteralError{"Cannot parse " + value + " to a number"};
    }
    val = i;
}

StringLiteral::StringLiteral(const SourceLocation *loc, std::string value)
    : BasicLiteral(loc, std::move(value)) {}

BooleanLiteral::BooleanLiteral(const SourceLocation *loc, std::string value)
    : BasicLiteral(loc, std::move(value)) {
    std::stringstream ss(value);
    ss >> std::boolalpha >> literal;
}

NullLiteral::NullLiteral(const SourceLocation *loc, std::string value)
    : BasicLiteral(loc, std::move(value)) {}

ArrayLiteral::ArrayLiteral(const SourceLocation *loc, std::vector<Expression *> list)
    : Literal(loc), list(std::move(list)) {}

FunctionLiteral::FunctionLiteral(const SourceLocation *loc,
                                 std::vector<ParamDecl *> parameters,
                                 ASTTypeExpr *returnType,
                                 BlockStmt *body)
    : Literal(loc), parameters(std::move(parameters)), returnType(returnType), body(body) {}

}
