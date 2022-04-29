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
    ReleaseScope _(*this, decl.getScope());
    auto scope = scopes.top();
    auto classType = dynamic_cast<ClassType *>(decl.getType());

    if (decl.getBaseclass()) {
        auto baseclass = typeParser.parseReferencedClassType(decl.getBaseclass(), scope);
        classType->setBaseclass(baseclass);
    }

    for (auto interfaceRef: decl.getInterfaces()) {
        auto interface = typeParser.parseReferencedInterfaceType(interfaceRef, scope);
        classType->addInterface(interface);
    }

    for (auto nested: decl.getDecls()) nested->accept(this);
    for (auto field: decl.getFields()) field->accept(this);
    for (auto method: decl.getMethods()) method->accept(this);

    return {};
}

OpaqueType LexicalContextDeclTypePass::visit(InterfaceDecl &decl) {
    ReleaseScope _(*this, decl.getScope());
    auto scope = scopes.top();
    auto interfaceType = dynamic_cast<InterfaceType *>(decl.getType());

    for (auto interfaceRef: decl.getInterfaces()) {
        auto interface = typeParser.parseReferencedInterfaceType(interfaceRef, scope);
        interfaceType->addInterface(interface);
    }

    for (auto nested: decl.getDecls()) nested->accept(this);
    for (auto method: decl.getMethods()) method->accept(this);

    return {};
}

OpaqueType LexicalContextDeclTypePass::visit(VariableDecl &decl) {
    if (decl.isGlobalVariable()) {
        auto type = typeParser.parseTypeExpr(decl.getTypeDecl(), scopes.top());
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
    auto type = typeParser.parseTypeExpr(decl.getTypeDecl(), scopes.top());
    decl.setType(type);

    

    if (decl.getInitializer()) {
        decl.getInitializer()->accept(this);
    }
    return {};
}

OpaqueType LexicalContextDeclTypePass::visit(ParamDecl &decl) {
    auto type = typeParser.parseTypeExpr(decl.getTypeDecl(), scopes.top());
    decl.setType(type);
    return Generic<Type *>::Create(type);
}

OpaqueType LexicalContextDeclTypePass::visit(FunctionDecl &decl) {
    ReleaseScope _(*this, decl.getScope());
    auto scope = scopes.top();

    std::vector<Type *> paramTypes;
    for (auto param: decl.getParamDecls()) {
        auto paramType = param->accept(this);
        paramTypes.push_back(Generic<Type *>::Get(paramType));
    }
    Type *returnType = typeParser.parseTypeExpr(decl.getReturnTypeDecl(), scope);

    auto funcType = typeContext.getFunctionType(returnType, paramTypes);
    decl.setType(funcType);
    scope->resolve(decl.getDeclname())->setMemberType(funcType);

    return {};
}

OpaqueType LexicalContextDeclTypePass::visit(MethodDecl &decl) {
    ReleaseScope _(*this, decl.getScope());
    auto scope = scopes.top();
}

} // reflex
