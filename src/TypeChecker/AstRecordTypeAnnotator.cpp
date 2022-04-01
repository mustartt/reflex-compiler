//
// Created by henry on 2022-03-31.
//

#include "AstRecordTypeAnnotator.h"
#include "ScopeSymbolTypeTable.h"
#include "AstNodes.h"
#include "TypeContextManager.h"

namespace reflex {

std::unique_ptr<ScopeSymbolTypeTable> AstRecordTypeAnnotator::annotate(CompilationUnit *unit) {
    visit(unit);
    return std::move(root);
}

void AstRecordTypeAnnotator::visit(ArrayLit *visitable) {
    for (auto lit: visitable->getInitializerList()) {
        lit->accept(this);
    }
}

void AstRecordTypeAnnotator::visit(FunctionLit *visitable) {
    auto scopename = "_l" + std::to_string(prefixCounter);
    auto parent = parentStack.top();
    auto scope = std::make_unique<ScopeSymbolTypeTable>(parent, scopename);

    parentStack.push(scope.get());
    if (visitable->getBody()) visitable->getBody()->accept(this);
    parentStack.pop();

    parent->addNamedScope(scopename, std::move(scope));
    ++prefixCounter;
}

void AstRecordTypeAnnotator::visit(UnaryExpr *visitable) {
    visitable->getExpr()->accept(this);
}

void AstRecordTypeAnnotator::visit(BinaryExpr *visitable) {
    visitable->getLhs()->accept(this);
    visitable->getRhs()->accept(this);
}

void AstRecordTypeAnnotator::visit(CastExpr *visitable) {
    visitable->getFrom()->accept(this);
}

void AstRecordTypeAnnotator::visit(SelectorExpr *visitable) {
    visitable->getBaseExpr()->accept(this);
}

void AstRecordTypeAnnotator::visit(IndexExpr *visitable) {
    visitable->getBaseExpr()->accept(this);
    visitable->getIndex()->accept(this);
}

void AstRecordTypeAnnotator::visit(ArgumentExpr *visitable) {
    visitable->getBaseExpr()->accept(this);
    for (auto arg: visitable->getArguments()) {
        arg->accept(this);
    }
}

void AstRecordTypeAnnotator::visit(VariableDecl *visitable) {
    if (visitable->getInitializer()) visitable->getInitializer()->accept(this);
}

void AstRecordTypeAnnotator::visit(ReturnStmt *visitable) {
    visitable->getReturnValue()->accept(this);
}

void AstRecordTypeAnnotator::visit(IfStmt *visitable) {
    visitable->getCond()->accept(this);
    visitable->getPrimaryBlock()->accept(this);
    if (visitable->getElseBlock()) visitable->getElseBlock()->accept(this);
}

void AstRecordTypeAnnotator::visit(ForRangeClause *visitable) {
    visitable->getIterExpr()->accept(this);
}

void AstRecordTypeAnnotator::visit(ForNormalClause *visitable) {
    visitable->getInit()->accept(this);
    visitable->getCond()->accept(this);
    visitable->getPost()->accept(this);
}

void AstRecordTypeAnnotator::visit(ForStmt *visitable) {
    visitable->getBody()->accept(this);
    visitable->getClause()->accept(this);
}

void AstRecordTypeAnnotator::visit(WhileStmt *visitable) {
    visitable->getCond()->accept(this);
    visitable->getBody()->accept(this);
}

void AstRecordTypeAnnotator::visit(AssignmentStmt *visitable) {
    visitable->getLhs()->accept(this);
    visitable->getRhs()->accept(this);
}

void AstRecordTypeAnnotator::visit(IncDecStmt *visitable) {
    visitable->getExpr()->accept(this);
}

void AstRecordTypeAnnotator::visit(ExpressionStmt *visitable) {
    visitable->getExpr()->accept(this);
}

void AstRecordTypeAnnotator::visit(Block *visitable) {
    auto scopename = "_b" + std::to_string(prefixCounter);
    auto parent = parentStack.top();
    auto scope = std::make_unique<ScopeSymbolTypeTable>(parent, scopename);

    parentStack.push(scope.get());
    for (auto stmt: visitable->getStmts()) {
        stmt->accept(this);
    }
    parentStack.pop();

    parent->addNamedScope(scopename, std::move(scope));
    ++prefixCounter;

}

void AstRecordTypeAnnotator::visit(FunctionDecl *visitable) {
    auto parent = parentStack.top();
    auto funcName = visitable->getName()->getIdent();
    auto scope = std::make_unique<ScopeSymbolTypeTable>(parent, funcName);

    parentStack.push(scope.get());
    if (visitable->getBody()) visitable->getBody()->accept(this);
    parentStack.pop();

    parent->addNamedScope(funcName, std::move(scope));
}

void AstRecordTypeAnnotator::visit(InterfaceDecl *visitable) {
    auto parent = parentStack.top();
    auto interfaceName = visitable->getInterfaceName();
    auto scope = std::make_unique<ScopeSymbolTypeTable>(parent, interfaceName);
    auto interfaceTy = typeContext.createOrGetInterfaceTy(parent->getScopePrefix() + interfaceName, {});
    visitable->setTyp(interfaceTy);

    parentStack.push(scope.get());
    for (auto member: visitable->getInterfaceMembers()) {
        member->accept(this);
    }
    parentStack.pop();

    parent->addScopeMember(interfaceName, interfaceTy);
    parent->addNamedScope(interfaceName, std::move(scope));
}

void AstRecordTypeAnnotator::visit(MemberDecl *visitable) {
    visitable->getDeclaration()->accept(this);
}

void AstRecordTypeAnnotator::visit(ClassDecl *visitable) {
    auto parent = parentStack.top();
    auto classname = visitable->getClassname();
    auto scope = std::make_unique<ScopeSymbolTypeTable>(parent, classname);
    auto classTy = typeContext.createOrGetClassTy(parent->getScopePrefix() + classname, nullptr, {});
    visitable->setTyp(classTy);

    parentStack.push(scope.get());
    for (auto member: visitable->getMembers()) {
        member->accept(this);
    }
    parentStack.pop();

    parent->addScopeMember(classname, classTy);
    parent->addNamedScope(classname, std::move(scope));
}

void AstRecordTypeAnnotator::visit(CompilationUnit *visitable) {
    auto scope = std::make_unique<ScopeSymbolTypeTable>(nullptr, "");
    parentStack.push(scope.get());
    for (auto decl: visitable->getDecls()) {
        decl->accept(this);
    }
    parentStack.pop();
    root = std::move(scope);
}

}
