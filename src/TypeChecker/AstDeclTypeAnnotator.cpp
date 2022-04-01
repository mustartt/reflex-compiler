//
// Created by henry on 2022-04-01.
//

#include "AstDeclTypeAnnotator.h"
#include "ScopeSymbolTypeTable.h"
#include "TypeContextManager.h"
#include "AstNodes.h"
#include "SemanticError.h"
#include "Utils/TypeCheckerUtils.h"

namespace reflex {

AstDeclTypeAnnotator::AstDeclTypeAnnotator(TypeContextManager &typeContext,
                                           std::unique_ptr<ScopeSymbolTypeTable> &root)
    : typeContext(typeContext), root(root), typeParser(typeContext) {}

void AstDeclTypeAnnotator::visit(TypeExpr *visitable) {
}
void AstDeclTypeAnnotator::visit(Identifier *visitable) {
}
void AstDeclTypeAnnotator::visit(ModuleSelector *visitable) {
}
void AstDeclTypeAnnotator::visit(NumberLit *visitable) {
}
void AstDeclTypeAnnotator::visit(StringLit *visitable) {
}
void AstDeclTypeAnnotator::visit(BoolLit *visitable) {
}
void AstDeclTypeAnnotator::visit(NullLit *visitable) {
}
void AstDeclTypeAnnotator::visit(IdentifierTypeExpr *visitable) {
}
void AstDeclTypeAnnotator::visit(ArrayTypeExpr *visitable) {
}
void AstDeclTypeAnnotator::visit(FunctionTypeExpr *visitable) {
}
void AstDeclTypeAnnotator::visit(ArrayLit *visitable) {
}
void AstDeclTypeAnnotator::visit(FunctionLit *visitable) {
    auto scopename = "_l" + std::to_string(prefixCounter);
    auto parentScope = parentStack.top();
    auto scope = parentStack.top()->getNestedScope(scopename);
    parentStack.push(scope);
    std::vector<Type *> paramTyps;
    for (auto param: visitable->getParameters()) {
        param->accept(this);
        paramTyps.push_back(param->getTyp());
    }
    Type *retTyp = typeParser.parseTypeExpr(visitable->getReturnTyp(), parentScope);
    if (!retTyp) throw SemanticError{"Unable to parse function lambda " + scope->getScopePrefix() + " return type"};
    auto fnTyp = typeContext.getFunctionTy(retTyp, paramTyps);
    visitable->setTyp(fnTyp);

    visitable->getBody()->accept(this);
    parentStack.pop();
    ++prefixCounter;
}
void AstDeclTypeAnnotator::visit(UnaryExpr *visitable) {
}
void AstDeclTypeAnnotator::visit(BinaryExpr *visitable) {
}
void AstDeclTypeAnnotator::visit(NewExpr *visitable) {
}
void AstDeclTypeAnnotator::visit(CastExpr *visitable) {
}
void AstDeclTypeAnnotator::visit(SelectorExpr *visitable) {
}
void AstDeclTypeAnnotator::visit(IndexExpr *visitable) {
}
void AstDeclTypeAnnotator::visit(ArgumentExpr *visitable) {
}

void AstDeclTypeAnnotator::visit(VariableDecl *visitable) {
    auto scope = parentStack.top();
    if (visitable->getInitializer()) visitable->getInitializer()->accept(this);
    if (!visitable->getVariableType()) return;
    auto name = visitable->getName()->getIdent();
    auto type = typeParser.parseTypeExpr(visitable->getVariableType(), parentStack.top());
    visitable->setTyp(type);
    visitable->getName()->setTyp(type);
    scope->addScopeMember(name, type);
}

void AstDeclTypeAnnotator::visit(ReturnStmt *visitable) {
}
void AstDeclTypeAnnotator::visit(BreakStmt *visitable) {
}
void AstDeclTypeAnnotator::visit(ContinueStmt *visitable) {
}
void AstDeclTypeAnnotator::visit(IfStmt *visitable) {
}
void AstDeclTypeAnnotator::visit(ForRangeClause *visitable) {
}
void AstDeclTypeAnnotator::visit(ForNormalClause *visitable) {
}
void AstDeclTypeAnnotator::visit(ForStmt *visitable) {
}
void AstDeclTypeAnnotator::visit(WhileStmt *visitable) {
}
void AstDeclTypeAnnotator::visit(EmptyStmt *visitable) {
}
void AstDeclTypeAnnotator::visit(AssignmentStmt *visitable) {
}
void AstDeclTypeAnnotator::visit(IncDecStmt *visitable) {
}
void AstDeclTypeAnnotator::visit(ExpressionStmt *visitable) {
}

void AstDeclTypeAnnotator::visit(Block *visitable) {
    auto scopename = "_b" + std::to_string(prefixCounter);
    auto scope = parentStack.top()->getNestedScope(scopename);

    parentStack.push(scope);
    for (auto stmt: visitable->getStmts()) {
        stmt->accept(this);
    }
    parentStack.pop();

    ++prefixCounter;
}

void AstDeclTypeAnnotator::visit(MemberDecl *visitable) {
    visitable->getDeclaration()->accept(this);
}

void AstDeclTypeAnnotator::visit(ParamDecl *visitable) {
    auto scope = parentStack.top();
    auto paramname = visitable->getName()->getIdent();
    auto paramTy = typeParser.parseTypeExpr(visitable->getParamType(), scope);
    if (!paramTy) throw SemanticError{"Unable to parse param decl " + paramname + " type"};
    visitable->setTyp(paramTy);
    visitable->getName()->setTyp(paramTy);

    scope->addScopeMember(paramname, paramTy);
}

void AstDeclTypeAnnotator::visit(FunctionDecl *visitable) {
    auto funcname = visitable->getName()->getIdent();
    auto parentScope = parentStack.top();
    auto scope = parentStack.top()->getNestedScope(funcname);
    parentStack.push(scope);
    std::vector<Type *> paramTyps;
    for (auto param: visitable->getParams()) {
        param->accept(this);
        paramTyps.push_back(param->getTyp());
    }
    Type *retTyp = typeParser.parseTypeExpr(visitable->getRetTyp(), parentScope);
    if (!retTyp) throw SemanticError{"Unable to parse function " + funcname + " return type"};
    auto fnTyp = typeContext.getFunctionTy(retTyp, paramTyps);
    visitable->setTyp(fnTyp);
    scope->addScopeMember(funcname, fnTyp);
    parentScope->addScopeMember(funcname, fnTyp);

    parentStack.pop();
}

void AstDeclTypeAnnotator::visit(ClassDecl *visitable) {
    auto classname = visitable->getClassname();
    auto parentScope = parentStack.top();
    auto classTy = dynamic_cast<ClassType *>(parentScope->getCurrentScopeReference(classname));
    auto scope = parentStack.top()->getNestedScope(classname);
    parentStack.push(scope);

    for (auto member: visitable->getMembers()) {
        member->accept(this);
    }

    if (visitable->getBaseclass()) {
        auto baseclassQualifier = visitable->getBaseclass()->getIdent();
        auto baseclassQualifierLst = ScopeSymbolTypeTable::getQuantifierList(baseclassQualifier);
        auto baseclassTy = dynamic_cast<ClassType *>(scope->findReferencedType(baseclassQualifierLst));
        if (!baseclassTy)
            throw SemanticError{
                classname + " must derive from class type not " + baseclassQualifier
            };
        classTy->setBaseclass(baseclassTy);
    }

    std::vector<InterfaceType *> interfaces;
    for (auto interfaceExpr: visitable->getInterfaces()) {
        auto interfaceQualifier = interfaceExpr->getIdent();
        auto interfaceQualifierLst = ScopeSymbolTypeTable::getQuantifierList(interfaceQualifier);
        auto interfaceTy = dynamic_cast<InterfaceType *>(scope->findReferencedType(interfaceQualifierLst));
        if (!interfaceTy)
            throw SemanticError{classname + " must implement from interface type not " + interfaceQualifier};
        interfaces.push_back(interfaceTy);
    }
    classTy->setInterfaces(interfaces);

    for (auto member: visitable->getMembers()) {
        if (!isInterfaceOrClassDecl(member->getDeclaration())) {
            auto membername = member->getName()->getIdent();
            auto type = scope->getCurrentScopeReference(membername);
            auto modifier = getVisibilityFromString(member->getModifier()->getIdent());
            classTy->addMember(membername, typeContext.createMemberTy(modifier, type));
        }
    }

    parentStack.pop();
}

void AstDeclTypeAnnotator::visit(InterfaceDecl *visitable) {
    auto interfacename = visitable->getInterfaceName();
    auto parentScope = parentStack.top();
    auto interfaceTy = dynamic_cast<InterfaceType *>(parentScope->getCurrentScopeReference(interfacename));
    auto scope = parentStack.top()->getNestedScope(interfacename);
    parentStack.push(scope);

    for (auto member: visitable->getInterfaceMembers()) {
        member->accept(this);
    }

    std::vector<InterfaceType *> interfaces;
    for (auto interfaceExpr: visitable->getInterfaces()) {
        auto interfaceQualifier = interfaceExpr->getIdent();
        auto interfaceQualifierLst = ScopeSymbolTypeTable::getQuantifierList(interfaceQualifier);
        auto derivedTy = dynamic_cast<InterfaceType *>(scope->findReferencedType(interfaceQualifierLst));
        if (!derivedTy)
            throw SemanticError{interfacename + " must extend from interface type not " + interfaceQualifier};
        interfaces.push_back(interfaceTy);
    }
    interfaceTy->setInterfaces(interfaces);

    for (auto member: visitable->getInterfaceMembers()) {
        if (!isInterfaceOrClassDecl(member->getDeclaration())) {
            auto membername = member->getName()->getIdent();
            auto type = scope->getCurrentScopeReference(membername);
            auto modifier = getVisibilityFromString(member->getModifier()->getIdent());
            interfaceTy->addInterfaceMethod(membername, typeContext.createMemberTy(modifier, type));
        }
    }

    parentStack.pop();
}

void AstDeclTypeAnnotator::visit(CompilationUnit *visitable) {
    auto &scope = root;
    parentStack.push(scope.get());
    for (auto decl: visitable->getDecls()) {
        decl->accept(this);
    }
    parentStack.pop();
}

void AstDeclTypeAnnotator::annotate(CompilationUnit *unit) {
    visit(unit);
}

}
