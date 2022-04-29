//
// Created by henry on 2022-04-28.
//

#include "LexicalContextDeclTypePass.h"

#include "LexicalScope.h"
#include "TypeContext.h"

#include "ASTDeclaration.h"
#include "ASTStatement.h"
#include "ASTExpression.h"
#include "ASTLiteral.h"

namespace reflex {

LexicalContextDeclTypePass::ReleaseScope::ReleaseScope(LexicalContextDeclTypePass &pass, LexicalScope *scope)
    : pass(pass) {
    pass.scopes.push(scope);
}

LexicalContextDeclTypePass::ReleaseScope::~ReleaseScope() { pass.scopes.pop(); }

void LexicalContextDeclTypePass::performPass(CompilationUnit *unit) {
    visit(*unit);
}

OpaqueType LexicalContextDeclTypePass::visit(CompilationUnit &unit) {
    ReleaseScope _(*this, unit.getScope());
    for (auto decl: unit.getDecls()) {
        decl->accept(this);
    }
    return {};
}

OpaqueType LexicalContextDeclTypePass::visit(ClassDecl &decl) {
    ReleaseScope _(*this, decl.getScope()->getChild());
    auto scope = scopes.top();
    auto classType = dynamic_cast<ClassType *>(decl.getType());

    if (decl.getBaseclass()) {
        auto baseclass = typeParser.parseClassType(decl.getBaseclass(), scope);
        classType->setBaseclass(baseclass);
    }

    for (auto interfaceRef: decl.getInterfaces()) {
        auto interface = typeParser.parseInterfaceType(interfaceRef, scope);
        classType->addInterface(interface);
    }

    for (auto nested: decl.getDecls()) nested->accept(this);
    for (auto field: decl.getFields()) field->accept(this);
    for (auto method: decl.getMethods()) method->accept(this);

    return {};
}

OpaqueType LexicalContextDeclTypePass::visit(InterfaceDecl &decl) {
    ReleaseScope _(*this, decl.getScope()->getChild());
    auto scope = scopes.top();
    auto interfaceType = dynamic_cast<InterfaceType *>(decl.getType());

    for (auto interfaceRef: decl.getInterfaces()) {
        auto interface = typeParser.parseInterfaceType(interfaceRef, scope);
        interfaceType->addInterface(interface);
    }

    for (auto nested: decl.getDecls()) nested->accept(this);
    for (auto method: decl.getMethods()) method->accept(this);

    return {};
}

OpaqueType LexicalContextDeclTypePass::visit(VariableDecl &decl) {
    if (decl.isGlobalVariable()) {
        auto type = typeParser.parseReferenceTypeExpr(decl.getTypeDecl(), scopes.top());
        auto member = scopes.top()->resolve(decl.getDeclname());
        decl.setType(type);
        member->setMemberType(type);
    }
    if (decl.getInitializer()) {
        decl.getInitializer()->accept(this);
    }
    return {};
}

OpaqueType LexicalContextDeclTypePass::visit(FieldDecl &decl) {
    auto type = typeParser.parseReferenceTypeExpr(decl.getTypeDecl(), scopes.top());
    decl.setType(type);

    if (decl.getVisibility() == Visibility::Static) {
        auto member = scopes.top()->resolve(decl.getDeclname());
        member->setMemberType(type);
    }

    if (decl.getInitializer()) {
        decl.getInitializer()->accept(this);
    }
    return {};
}

OpaqueType LexicalContextDeclTypePass::visit(ParamDecl &decl) {
    auto type = typeParser.parseReferenceTypeExpr(decl.getTypeDecl(), scopes.top());
    decl.setType(type);
    return Generic<Type *>::Create(type);
}

OpaqueType LexicalContextDeclTypePass::visit(FunctionDecl &decl) {
    ReleaseScope _(*this, decl.getScope()->getChild());
    auto scope = scopes.top();

    std::vector<Type *> paramTypes;
    for (auto param: decl.getParamDecls()) {
        auto paramType = param->accept(this);
        paramTypes.push_back(Generic<Type *>::Get(paramType));
    }
    Type *returnType = typeParser.parseReferenceTypeExpr(decl.getReturnTypeDecl(), scope);

    auto funcType = typeContext.getFunctionType(returnType, paramTypes);
    decl.setType(funcType);
    decl.getScope()->setMemberType(funcType);

    if (decl.getBody()) decl.getBody()->accept(this);

    return {};
}

OpaqueType LexicalContextDeclTypePass::visit(MethodDecl &decl) {
    ReleaseScope _(*this, decl.getScope()->getChild());
    auto scope = scopes.top();

    std::vector<Type *> paramTypes;
    for (auto param: decl.getParamDecls()) {
        auto paramType = param->accept(this);
        paramTypes.push_back(Generic<Type *>::Get(paramType));
    }
    Type *returnType = typeParser.parseReferenceTypeExpr(decl.getReturnTypeDecl(), scope);

    auto funcType = typeContext.getFunctionType(returnType, paramTypes);
    decl.setType(funcType);

    auto compositeScope = decl.getScope()->getParent()->getParentMember();
    if (auto interfaceType = dynamic_cast<InterfaceType *>(compositeScope->getMemberType())) {
        auto methodType = typeContext.getMemberType(decl.getVisibility(), interfaceType, funcType);
        interfaceType->addMethod(decl.getDeclname(), methodType);
        decl.setType(methodType);
        decl.getScope()->setMemberType(methodType);
    } else if (auto classType = dynamic_cast<ClassType *>(compositeScope->getMemberType())) {
        auto methodType = typeContext.getMemberType(decl.getVisibility(), classType, funcType);
        classType->addMethod(decl.getDeclname(), methodType);
        decl.setType(methodType);
        decl.getScope()->setMemberType(methodType);
    } else throw TypeError{"MethodDecl must be part of a Class or Interface LexicalScope"};

    if (decl.getBody()) decl.getBody()->accept(this);

    return {};
}

OpaqueType LexicalContextDeclTypePass::visit(BlockStmt &stmt) {
    ReleaseScope _(*this, stmt.getScope()->getChild());
    for (auto statement: stmt.getStmts()) {
        statement->accept(this);
    }
    return {};
}

OpaqueType LexicalContextDeclTypePass::visit(FunctionLiteral &literal) {
    ReleaseScope _(*this, literal.getScope()->getChild());
    auto scope = scopes.top();

    std::vector<Type *> paramTypes;
    for (auto param: literal.getParamDecls()) {
        auto paramType = param->accept(this);
        paramTypes.push_back(Generic<Type *>::Get(paramType));
    }
    Type *returnType = typeParser.parseReferenceTypeExpr(literal.getReturnType(), scope);
    auto funcType = typeContext.getFunctionType(returnType, paramTypes);

    literal.setType(funcType);
    literal.getScope()->setMemberType(funcType);

    if (literal.getBody()) literal.getBody()->accept(this);

    return {};
}

OpaqueType LexicalContextDeclTypePass::visit(NewExpr &expr) {
    expr.setType(typeParser.parseBaseType(expr.getInstanceType(), scopes.top()));
    return {};
}

OpaqueType LexicalContextDeclTypePass::visit(CastExpr &expr) {
    expr.setType(typeParser.parseReferenceTypeExpr(expr.getResultType(), scopes.top()));
    return {};
}

OpaqueType LexicalContextDeclTypePass::visit(UnaryExpr &expr) {
    expr.getExpr()->accept(this);
    return {};
}

OpaqueType LexicalContextDeclTypePass::visit(BinaryExpr &expr) {
    expr.getLhs()->accept(this);
    expr.getRhs()->accept(this);
    return {};
}

OpaqueType LexicalContextDeclTypePass::visit(IndexExpr &expr) {
    if (expr.getIndex()) expr.getIndex()->accept(this);
    expr.getBaseExpr()->accept(this);
    return {};
}

OpaqueType LexicalContextDeclTypePass::visit(SelectorExpr &expr) {
    expr.getBaseExpr()->accept(this);
    return {};
}

OpaqueType LexicalContextDeclTypePass::visit(ArgumentExpr &expr) {
    expr.getBaseExpr()->accept(this);
    for (auto arg: expr.getArguments()) arg->accept(this);
    return {};
}

OpaqueType LexicalContextDeclTypePass::visit(ReturnStmt &stmt) {
    stmt.getReturnValue()->accept(this);
    return {};
}

OpaqueType LexicalContextDeclTypePass::visit(IfStmt &stmt) {
    stmt.getCond()->accept(this);
    stmt.getPrimaryBlock()->accept(this);
    if (stmt.getElseBlock()) stmt.getElseBlock()->accept(this);
    return {};
}

OpaqueType LexicalContextDeclTypePass::visit(ForStmt &stmt) {
    // visit for clause
    stmt.getBody()->accept(this);
    return {};
}

OpaqueType LexicalContextDeclTypePass::visit(WhileStmt &stmt) {
    stmt.getCond()->accept(this);
    stmt.getBody()->accept(this);
    return {};
}

OpaqueType LexicalContextDeclTypePass::visit(AssignmentStmt &stmt) {
    stmt.getLhs()->accept(this);
    stmt.getRhs()->accept(this);
    return {};
}

OpaqueType LexicalContextDeclTypePass::visit(IncDecStmt &stmt) {
    stmt.getExpr()->accept(this);
    return {};
}

OpaqueType LexicalContextDeclTypePass::visit(ExpressionStmt &stmt) {
    stmt.getExpr()->accept(this);
    return {};
}

OpaqueType LexicalContextDeclTypePass::visit(DeclStmt &stmt) {
    stmt.getDecl()->accept(this);
    return {};
}

OpaqueType LexicalContextDeclTypePass::visit(ArrayLiteral &literal) {
    for (auto lit: literal.getInitList()) {
        lit->accept(this);
    }
    return {};
}

} // reflex
