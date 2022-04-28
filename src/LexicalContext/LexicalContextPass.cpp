//
// Created by henry on 2022-04-28.
//

#include "LexicalContextPass.h"

#include "TypeContext.h"
#include "LexicalContext.h"
#include "ASTExpression.h"
#include "ASTStatement.h"
#include "ASTDeclaration.h"
#include "ASTLiteral.h"

namespace reflex {

LexicalScope *LexicalContextPass::performPass(CompilationUnit *unit) {
    return Generic<LexicalScope *>::Get(visit(*unit));
}

OpaqueType LexicalContextPass::visit(CompilationUnit &unit) {
    auto globalScope = context.createGlobalScope(&unit);
    scope.push(globalScope);
    for (auto decl: unit.getDecls()) {
        decl->accept(this);
    }
    scope.pop();
    return Generic<LexicalScope *>::Create(globalScope);
}

OpaqueType LexicalContextPass::visit(ClassDecl &decl) {
    auto classScope = context.createCompositeScope(&decl, scope.top());
    scope.top()->addScopeMember(
        decl.getDeclname(),
        typeContext.getClassType(decl.getDeclname(), &decl),
        classScope
    );
    scope.push(classScope);

    for (auto nested: decl.getDecls()) nested->accept(this);
    for (auto field: decl.getFields()) field->accept(this);
    for (auto method: decl.getMethods()) method->accept(this);

    scope.pop();
    return {};
}

OpaqueType LexicalContextPass::visit(InterfaceDecl &decl) {
    auto interfaceScope = context.createCompositeScope(&decl, scope.top());
    scope.top()->addScopeMember(
        decl.getDeclname(),
        typeContext.getClassType(decl.getDeclname(), &decl),
        interfaceScope
    );
    scope.push(interfaceScope);

    for (auto nested: decl.getDecls()) nested->accept(this);
    for (auto method: decl.getMethods()) method->accept(this);

    scope.pop();
    return {};
}

OpaqueType LexicalContextPass::visit(VariableDecl &decl) {
    scope.top()->addScopeMember(decl.getDeclname(), nullptr);
    if (decl.getInitializer()) decl.getInitializer()->accept(this);
    return {};
}

OpaqueType LexicalContextPass::visit(FieldDecl &decl) {
    scope.top()->addScopeMember(decl.getDeclname(), nullptr);
    if (decl.getInitializer()) decl.getInitializer()->accept(this);
    return {};
}

OpaqueType LexicalContextPass::visit(ParamDecl &decl) {
    scope.top()->addScopeMember(decl.getDeclname(), nullptr);
    return {};
}

OpaqueType LexicalContextPass::visit(FunctionDecl &decl) {
    auto funcScope = context.createFunctionScope(&decl, scope.top());
    scope.top()->addScopeMember(decl.getDeclname(), nullptr, funcScope);
    scope.push(funcScope);
    generateBlockScope = false;

    for (auto param: decl.getParamDecls()) param->accept(this);
    if (decl.getBody()) decl.getBody()->accept(this);

    generateBlockScope = true;
    scope.pop();
    return {};
}

OpaqueType LexicalContextPass::visit(MethodDecl &decl) {
    auto funcScope = context.createMethodScope(&decl, scope.top());
    scope.top()->addScopeMember(decl.getDeclname(), nullptr, funcScope);
    scope.push(funcScope);
    generateBlockScope = false;

    for (auto param: decl.getParamDecls()) param->accept(this);
    if (decl.getBody()) decl.getBody()->accept(this);

    generateBlockScope = true;
    scope.pop();
    return {};
}

OpaqueType LexicalContextPass::visit(FunctionLiteral &literal) {
    auto lambdaScope = context.createLambdaScope(&literal, scope.top()->incLambdaCount(), scope.top());
    scope.top()->addScopeMember(lambdaScope->getScopename(), nullptr, lambdaScope);
    scope.push(lambdaScope);
    generateBlockScope = false;

    for (auto param: literal.getParamDecls()) param->accept(this);
    if (literal.getBody()) literal.getBody()->accept(this);

    generateBlockScope = true;
    scope.pop();
    return {};
}

OpaqueType LexicalContextPass::visit(BlockStmt &stmt) {
    if (generateBlockScope) {
        auto blockScope = context.createBlockScope(&stmt, scope.top()->incBlockCount(), scope.top());
        scope.top()->addScopeMember(blockScope->getScopename(), nullptr, blockScope);
        scope.push(blockScope);
        generateBlockScope = true;
        for (auto statement: stmt.getStmts()) statement->accept(this);
        generateBlockScope = false;
        scope.pop();
    } else {
        generateBlockScope = true;
        for (auto statement: stmt.getStmts()) statement->accept(this);
        generateBlockScope = false;
    }
    return {};
}

OpaqueType LexicalContextPass::visit(UnaryExpr &expr) {
    expr.getExpr()->accept(this);
    return {};
}

OpaqueType LexicalContextPass::visit(BinaryExpr &expr) {
    expr.getLhs()->accept(this);
    expr.getRhs()->accept(this);
    return {};
}

OpaqueType LexicalContextPass::visit(CastExpr &expr) {
    expr.getFrom()->accept(this);
    return {};
}

OpaqueType LexicalContextPass::visit(IndexExpr &expr) {
    if (expr.getIndex()) expr.getIndex()->accept(this);
    expr.getBaseExpr()->accept(this);
    return {};
}

OpaqueType LexicalContextPass::visit(SelectorExpr &expr) {
    expr.getBaseExpr()->accept(this);
    return {};
}

OpaqueType LexicalContextPass::visit(ArgumentExpr &expr) {
    expr.getBaseExpr()->accept(this);
    for (auto arg: expr.getArguments()) arg->accept(this);
    return {};
}

OpaqueType LexicalContextPass::visit(ReturnStmt &stmt) {
    stmt.getReturnValue()->accept(this);
    return {};
}
OpaqueType LexicalContextPass::visit(IfStmt &stmt) {
    stmt.getCond()->accept(this);
    stmt.getPrimaryBlock()->accept(this);
    if (stmt.getElseBlock()) stmt.getElseBlock()->accept(this);
    return {};
}

OpaqueType LexicalContextPass::visit(ForStmt &stmt) {
    // visit for clause
    stmt.getBody()->accept(this);
    return {};
}

OpaqueType LexicalContextPass::visit(WhileStmt &stmt) {
    stmt.getCond()->accept(this);
    stmt.getBody()->accept(this);
    return {};
}

OpaqueType LexicalContextPass::visit(AssignmentStmt &stmt) {
    stmt.getLhs()->accept(this);
    stmt.getRhs()->accept(this);
    return {};
}

OpaqueType LexicalContextPass::visit(IncDecStmt &stmt) {
    stmt.getExpr()->accept(this);
    return {};
}

OpaqueType LexicalContextPass::visit(ExpressionStmt &stmt) {
    stmt.getExpr()->accept(this);
    return {};
}

OpaqueType LexicalContextPass::visit(DeclStmt &stmt) {
    stmt.getDecl()->accept(this);
    return {};
}

OpaqueType LexicalContextPass::visit(ArrayLiteral &literal) {
    for (auto lit: literal.getInitList()) {
        lit->accept(this);
    }
    return {};
}

}
