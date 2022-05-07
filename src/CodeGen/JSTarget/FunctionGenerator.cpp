//
// Created by henry on 2022-05-03.
//

#include "FunctionGenerator.h"

#include <sstream>
#include <ostream>

#include "ASTDeclaration.h"
#include "ASTStatement.h"
#include "ASTExpression.h"
#include "ASTLiteral.h"

#include "SourceOutputStream.h"

namespace reflex {

void FunctionGenerator::emit() {

}

void FunctionGenerator::registerLocalDecl(VariableDecl *decl) {
    refTable[decl] = refTable.size();
}

size_t FunctionGenerator::getLocalDeclRef(VariableDecl *decl) const {
    return refTable.at(decl);
}

std::string FunctionGenerator::getVariableName(VariableDecl *decl) {
    if (!refTable.contains(decl)) {
        registerLocalDecl(decl);
    }
    return "_x" + std::to_string(getLocalDeclRef(decl));
}

OpaqueType FunctionGenerator::visit(ParamDecl &decl) {
    os.stream() << getVariableName(&decl);
    return {};
}

OpaqueType FunctionGenerator::visit(VariableDecl &decl) {
    os.stream() << getVariableName(&decl);
    return {};
}

OpaqueType FunctionGenerator::visit(FunctionDecl &decl) {
    os.printIndent() << "function " << decl.getDeclname() << "(";
    auto &params = decl.getParamDecls();
    if (!params.empty()) {
        params[0]->accept(this);
        for (size_t i = 1; i < params.size(); ++i) {
            os.stream() << ", ";
            params[i]->accept(this);
        }
    }
    os.printIndent() << ") {" << std::endl;
    if (decl.getBody()) {
        auto _ = os.createScopeIndent();
        for (auto stmt: decl.getBody()->getStmts()) {
            stmt->accept(this);
        }
    }
    os.printIndent() << "}" << std::endl;
    return {};
}

OpaqueType FunctionGenerator::visit(BlockStmt &stmt) {
    os.printIndent() << "(function(){" << std::endl;
    auto _ = os.createScopeIndent();
    for (auto statement: stmt.getStmts()) {
        statement->accept(this);
    }
    os.printIndent() << "})();" << std::endl;
    return {};
}

using StringRet = Generic<std::string>;

OpaqueType FunctionGenerator::visit(NumberLiteral &literal) {
    return StringRet::Create(std::to_string(literal.getValue()));
}

OpaqueType FunctionGenerator::visit(StringLiteral &literal) {
    return StringRet::Create("\"" + literal.getLiteral() + "\"");
}

OpaqueType FunctionGenerator::visit(BooleanLiteral &literal) {
    return StringRet::Create(literal.getLiteral());
}

OpaqueType FunctionGenerator::visit(NullLiteral &literal) {
    return StringRet::Create("null");
}

OpaqueType FunctionGenerator::visit(ArrayLiteral &literal) {
    std::stringstream arr;
    arr << "[ ";
    for (auto expr: literal.getInitList()) {
        arr << StringRet::Get(expr->accept(this)) << ", ";
    }
    arr << " ]";
    return StringRet::Create(arr.str());
}

OpaqueType FunctionGenerator::visit(DeclRefExpr &expr) {
    auto referencedDecl = dynamic_cast<VariableDecl *>(expr.getDecl());
    return StringRet::Create(getVariableName(referencedDecl));
}

OpaqueType FunctionGenerator::visit(UnaryExpr &expr) {
    switch (expr.getUnaryOp()) {
        case Operator::UnaryOperator::Negative: {
            return StringRet::Create("(-" + StringRet::Get(expr.getExpr()->accept(this)) + ")");
        }
        case Operator::UnaryOperator::LogicalNot: {
            return StringRet::Create("(!" + StringRet::Get(expr.getExpr()->accept(this)) + ")");
        }
    }
    return {};
}

OpaqueType FunctionGenerator::visit(BinaryExpr &expr) {
    using namespace Operator;
    std::unordered_map<BinaryOperator, std::string> mappedOp{
        {BinaryOperator::Or, "|"},
        {BinaryOperator::And, "&"},
        {BinaryOperator::Compare, "==="},
        {BinaryOperator::CompareNot, "!=="},
        {BinaryOperator::Less, "<"},
        {BinaryOperator::Greater, ">"},
        {BinaryOperator::CompareLessThanEqual, "<="},
        {BinaryOperator::CompareGreaterThanEqual, ">="},
        {BinaryOperator::Add, "+"},
        {BinaryOperator::Sub, "-"},
        {BinaryOperator::LogicalOr, "||"},
        {BinaryOperator::Mult, "*"},
        {BinaryOperator::Div, "/"},
        {BinaryOperator::Mod, "%"},
        {BinaryOperator::LogicalAnd, "&&"}
    };

    return StringRet::Create(
        "(" + StringRet::Get(expr.getLhs()->accept(this)) + ") " +
            mappedOp[expr.getBinaryOp()] +
            " (" + StringRet::Get(expr.getRhs()->accept(this)) + ")");
}

} // reflex
