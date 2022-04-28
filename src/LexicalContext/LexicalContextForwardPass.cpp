//
// Created by henry on 2022-04-28.
//

#include "LexicalContextForwardPass.h"

#include "TypeContext.h"
#include "LexicalContext.h"
#include "ASTExpression.h"
#include "ASTStatement.h"
#include "ASTDeclaration.h"
#include "ASTLiteral.h"

namespace reflex {

LexicalScope *LexicalContextForwardPass::performPass(CompilationUnit *unit) {
    return Generic<LexicalScope *>::Get(visit(*unit));
}

OpaqueType LexicalContextForwardPass::visit(CompilationUnit &unit) {
    auto globalScope = context.createGlobalScope(&unit);
    unit.setScope(globalScope);
    scope.push(globalScope);
    for (auto decl: unit.getDecls()) {
        decl->accept(this);
    }
    scope.pop();
    return Generic<LexicalScope *>::Create(globalScope);
}

OpaqueType LexicalContextForwardPass::visit(ClassDecl &decl) {
    auto classScope = context.createCompositeScope(&decl, scope.top());
    decl.setScope(classScope);
    auto &member = scope.top()->addScopeMember(
        decl.getDeclname(),
        nullptr,
        classScope
    );
    member.setMemberType(typeContext.getClassType(
        member.getStringQualifier(), &decl));
    scope.push(classScope);

    for (auto nested: decl.getDecls()) nested->accept(this);
    for (auto field: decl.getFields()) field->accept(this);
    for (auto method: decl.getMethods()) method->accept(this);

    scope.pop();
    return {};
}

OpaqueType LexicalContextForwardPass::visit(InterfaceDecl &decl) {
    auto interfaceScope = context.createCompositeScope(&decl, scope.top());
    decl.setScope(interfaceScope);
    auto &member = scope.top()->addScopeMember(
        decl.getDeclname(),
        nullptr,
        interfaceScope
    );
    member.setMemberType(typeContext.getInterfaceType(
        member.getStringQualifier(), &decl));
    scope.push(interfaceScope);

    for (auto nested: decl.getDecls()) nested->accept(this);
    for (auto method: decl.getMethods()) method->accept(this);

    scope.pop();
    return {};
}

OpaqueType LexicalContextForwardPass::visit(VariableDecl &decl) {
    if (decl.isGlobalVariable()) {
        scope.top()->addScopeMember(decl.getDeclname(), nullptr);
    }
    if (decl.getInitializer()) decl.getInitializer()->accept(this);
    return {};
}

OpaqueType LexicalContextForwardPass::visit(FieldDecl &decl) {
    if (decl.getVisibility() == Visibility::Static) {
        scope.top()->addScopeMember(decl.getDeclname(), nullptr);
    }
    if (decl.getInitializer()) decl.getInitializer()->accept(this);
    return {};
}

OpaqueType LexicalContextForwardPass::visit(ParamDecl &decl) {
//    scope.top()->addScopeMember(decl.getDeclname(), nullptr);
    return {};
}

OpaqueType LexicalContextForwardPass::visit(FunctionDecl &decl) {
    auto funcScope = context.createFunctionScope(&decl, scope.top());
    decl.setScope(funcScope);
    scope.top()->addScopeMember(decl.getDeclname(), nullptr, funcScope);
    scope.push(funcScope);
    generateBlockScope = false;

    for (auto param: decl.getParamDecls()) param->accept(this);
    if (decl.getBody()) decl.getBody()->accept(this);

    generateBlockScope = true;
    scope.pop();
    return {};
}

OpaqueType LexicalContextForwardPass::visit(MethodDecl &decl) {
    auto funcScope = context.createMethodScope(&decl, scope.top());
    decl.setScope(funcScope);
    scope.top()->addScopeMember(decl.getDeclname(), nullptr, funcScope);
    scope.push(funcScope);
    generateBlockScope = false;

    for (auto param: decl.getParamDecls()) param->accept(this);
    if (decl.getBody()) decl.getBody()->accept(this);

    generateBlockScope = true;
    scope.pop();
    return {};
}

OpaqueType LexicalContextForwardPass::visit(FunctionLiteral &literal) {
    auto lambdaScope = context.createLambdaScope(&literal, scope.top()->incLambdaCount(), scope.top());
    literal.setScope(lambdaScope);
    scope.top()->addScopeMember(lambdaScope->getScopename(), nullptr, lambdaScope);
    scope.push(lambdaScope);
    generateBlockScope = false;

    for (auto param: literal.getParamDecls()) param->accept(this);
    if (literal.getBody()) literal.getBody()->accept(this);

    generateBlockScope = true;
    scope.pop();
    return {};
}

OpaqueType LexicalContextForwardPass::visit(BlockStmt &stmt) {
    if (generateBlockScope) {
        auto blockScope = context.createBlockScope(&stmt, scope.top()->incBlockCount(), scope.top());
        stmt.setScope(blockScope);
        scope.top()->addScopeMember(blockScope->getScopename(), nullptr, blockScope);
        scope.push(blockScope);
        generateBlockScope = true;
        for (auto statement: stmt.getStmts()) statement->accept(this);
        generateBlockScope = false;
        scope.pop();
    } else {
        generateBlockScope = true;
        stmt.setScope(scope.top());
        for (auto statement: stmt.getStmts()) statement->accept(this);
        generateBlockScope = false;
    }
    return {};
}

OpaqueType LexicalContextForwardPass::visit(UnaryExpr &expr) {
    expr.getExpr()->accept(this);
    return {};
}

OpaqueType LexicalContextForwardPass::visit(BinaryExpr &expr) {
    expr.getLhs()->accept(this);
    expr.getRhs()->accept(this);
    return {};
}

OpaqueType LexicalContextForwardPass::visit(CastExpr &expr) {
    expr.getFrom()->accept(this);
    return {};
}

OpaqueType LexicalContextForwardPass::visit(IndexExpr &expr) {
    if (expr.getIndex()) expr.getIndex()->accept(this);
    expr.getBaseExpr()->accept(this);
    return {};
}

OpaqueType LexicalContextForwardPass::visit(SelectorExpr &expr) {
    expr.getBaseExpr()->accept(this);
    return {};
}

OpaqueType LexicalContextForwardPass::visit(ArgumentExpr &expr) {
    expr.getBaseExpr()->accept(this);
    for (auto arg: expr.getArguments()) arg->accept(this);
    return {};
}

OpaqueType LexicalContextForwardPass::visit(ReturnStmt &stmt) {
    stmt.getReturnValue()->accept(this);
    return {};
}
OpaqueType LexicalContextForwardPass::visit(IfStmt &stmt) {
    stmt.getCond()->accept(this);
    stmt.getPrimaryBlock()->accept(this);
    if (stmt.getElseBlock()) stmt.getElseBlock()->accept(this);
    return {};
}

OpaqueType LexicalContextForwardPass::visit(ForStmt &stmt) {
    // visit for clause
    stmt.getBody()->accept(this);
    return {};
}

OpaqueType LexicalContextForwardPass::visit(WhileStmt &stmt) {
    stmt.getCond()->accept(this);
    stmt.getBody()->accept(this);
    return {};
}

OpaqueType LexicalContextForwardPass::visit(AssignmentStmt &stmt) {
    stmt.getLhs()->accept(this);
    stmt.getRhs()->accept(this);
    return {};
}

OpaqueType LexicalContextForwardPass::visit(IncDecStmt &stmt) {
    stmt.getExpr()->accept(this);
    return {};
}

OpaqueType LexicalContextForwardPass::visit(ExpressionStmt &stmt) {
    stmt.getExpr()->accept(this);
    return {};
}

OpaqueType LexicalContextForwardPass::visit(DeclStmt &stmt) {
    stmt.getDecl()->accept(this);
    return {};
}

OpaqueType LexicalContextForwardPass::visit(ArrayLiteral &literal) {
    for (auto lit: literal.getInitList()) {
        lit->accept(this);
    }
    return {};
}

}
