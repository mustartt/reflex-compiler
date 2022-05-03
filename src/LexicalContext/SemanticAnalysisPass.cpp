//
// Created by henry on 2022-05-03.
//

#include "SemanticAnalysisPass.h"

#include "ASTDeclaration.h"
#include "ASTExpression.h"
#include "ASTStatement.h"
#include "ASTLiteral.h"
#include "LexicalScope.h"
#include "SemanticAnalyzer.h"

namespace reflex {

OpaqueType SemanticAnalysisPass::visit(CompilationUnit &unit) {
    for (auto decl: unit.getDecls()) {
        decl->accept(this);
    }
    return {};
}

OpaqueType SemanticAnalysisPass::visit(VariableDecl &decl) {
    if (decl.isGlobalVariable()) {
        SemanticAnalyzer analyzer(typeContext, context);
        analyzer.analyzeStaticVars(&decl, globalScope);
    }
    return {};
}

OpaqueType SemanticAnalysisPass::visit(ClassDecl &decl) {
    auto classScope = decl.getScope()->getChild();
    for (auto nested: decl.getDecls()) {
        nested->accept(this);
    }
    for (auto field: decl.getFields()) {
        SemanticAnalyzer analyzer(typeContext, context);
        if (field->getVisibility() == Visibility::Static) {
            analyzer.analyzeStaticVars(field, classScope);
        } else {
            analyzer.analyzeField(field);
        }
    }
    for (auto method: decl.getMethods()) {
        SemanticAnalyzer analyzer(typeContext, context);
        if (method->getVisibility() == Visibility::Static) {
            analyzer.analyzeFunction(method);
        } else {
            analyzer.analyzeMethod(method);
        }
    }
    return {};
}

OpaqueType SemanticAnalysisPass::visit(FunctionDecl &decl) {
    SemanticAnalyzer analyzer(typeContext, context);
    analyzer.analyzeFunction(&decl);
    return {};
}

OpaqueType SemanticAnalysisPass::visit(FunctionLiteral &literal) {
    SemanticAnalyzer analyzer(typeContext, context);
    analyzer.analyzeLambda(&literal);
    return {};
}

OpaqueType SemanticAnalysisPass::visit(DeclStmt &stmt) {
    stmt.getDecl()->accept(this);
    return {};
}

OpaqueType SemanticAnalysisPass::visit(BlockStmt &stmt) {
    for (auto statement: stmt.getStmts()) {
        statement->accept(this);
    }
    return {};
}

OpaqueType SemanticAnalysisPass::visit(ReturnStmt &stmt) {
    if (stmt.getReturnValue()) {
        stmt.getReturnValue()->accept(this);
    }
    return {};
}

OpaqueType SemanticAnalysisPass::visit(IfStmt &stmt) {
    stmt.getCond()->accept(this);
    stmt.getPrimaryBlock()->accept(this);
    if (stmt.getElseBlock()) stmt.getElseBlock()->accept(this);
    return {};
}

OpaqueType SemanticAnalysisPass::visit(ForStmt &stmt) {
    // visit for clause
    stmt.getBody()->accept(this);
    return {};
}

OpaqueType SemanticAnalysisPass::visit(WhileStmt &stmt) {
    stmt.getCond()->accept(this);
    stmt.getBody()->accept(this);
    return {};
}

OpaqueType SemanticAnalysisPass::visit(AssignmentStmt &stmt) {
    stmt.getLhs()->accept(this);
    stmt.getRhs()->accept(this);
    return {};
}

OpaqueType SemanticAnalysisPass::visit(IncDecStmt &stmt) {
    stmt.getExpr()->accept(this);
    return {};
}

OpaqueType SemanticAnalysisPass::visit(ExpressionStmt &stmt) {
    stmt.getExpr()->accept(this);
    return {};
}

OpaqueType SemanticAnalysisPass::visit(ArrayLiteral &literal) {
    for (auto lit: literal.getInitList()) {
        lit->accept(this);
    }
    return {};
}

OpaqueType SemanticAnalysisPass::visit(UnaryExpr &expr) {
    expr.getExpr()->accept(this);
    return {};
}

OpaqueType SemanticAnalysisPass::visit(BinaryExpr &expr) {
    expr.getLhs()->accept(this);
    expr.getRhs()->accept(this);
    return {};
}

OpaqueType SemanticAnalysisPass::visit(CastExpr &expr) {
    expr.getFrom()->accept(this);
    return {};
}

OpaqueType SemanticAnalysisPass::visit(IndexExpr &expr) {
    if (expr.getIndex()) expr.getIndex()->accept(this);
    expr.getBaseExpr()->accept(this);
    return {};
}

OpaqueType SemanticAnalysisPass::visit(ImplicitCastExpr &expr) {
    expr.getFrom()->accept(this);
    return {};
}

OpaqueType SemanticAnalysisPass::visit(SelectorExpr &expr) {
    expr.getBaseExpr()->accept(this);
    return {};
}

OpaqueType SemanticAnalysisPass::visit(ArgumentExpr &expr) {
    expr.getBaseExpr()->accept(this);
    for (auto arg: expr.getArguments()) {
        arg->accept(this);
    }
    return {};
}

} // reflex
