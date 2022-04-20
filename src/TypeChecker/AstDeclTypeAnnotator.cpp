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

void AstDeclTypeAnnotator::visit(ArrayLit *visitable) {
    for (auto lit: visitable->getInitializerList()) {
        lit->accept(this);
    }
}

void AstDeclTypeAnnotator::visit(UnaryExpr *visitable) {
    visitable->getExpr()->accept(this);
}

void AstDeclTypeAnnotator::visit(BinaryExpr *visitable) {
    visitable->getLhs()->accept(this);
    visitable->getRhs()->accept(this);
}

void AstDeclTypeAnnotator::visit(NewExpr *visitable) {
    auto instanceTy = typeParser.parseTypeExpr(visitable->getInstanceTyp(), parentStack.top());
    if (!instanceTy)
        throw SemanticError{"Unable to parse instance type to be instantiated"};
    visitable->setTyp(instanceTy);
}

void AstDeclTypeAnnotator::visit(CastExpr *visitable) {
    auto ty = typeParser.parseTypeExpr(visitable->getTargetTyp(), parentStack.top());
    if (!ty) throw SemanticError{"Unable to parse cast target type"};
    visitable->setTyp(ty);
}

void AstDeclTypeAnnotator::visit(SelectorExpr *visitable) {
    visitable->getBaseExpr()->accept(this);
}

void AstDeclTypeAnnotator::visit(IndexExpr *visitable) {
    visitable->getBaseExpr()->accept(this);
    visitable->getIndex()->accept(this);
}

void AstDeclTypeAnnotator::visit(ArgumentExpr *visitable) {
    visitable->getBaseExpr()->accept(this);
    for (auto arg: visitable->getArguments()) {
        arg->accept(this);
    }
}

void AstDeclTypeAnnotator::visit(VariableDecl *visitable) {
    auto scope = parentStack.top();
    if (visitable->getInitializer()) visitable->getInitializer()->accept(this);
    auto name = visitable->getName()->getIdent();
    auto type = typeParser.parseTypeExpr(visitable->getVariableType(), parentStack.top());
    visitable->setTyp(type);
    visitable->getName()->setTyp(type);
    scope->addScopeMember(name, type);
}

void AstDeclTypeAnnotator::visit(ReturnStmt *visitable) {
    if (visitable->getReturnValue())
        visitable->getReturnValue()->accept(this);
}

void AstDeclTypeAnnotator::visit(BreakStmt *visitable) {
}

void AstDeclTypeAnnotator::visit(ContinueStmt *visitable) {
}
void AstDeclTypeAnnotator::visit(EmptyStmt *visitable) {
}

void AstDeclTypeAnnotator::visit(IfStmt *visitable) {
    visitable->getCond()->accept(this);
    visitable->getPrimaryBlock()->accept(this);
    if (visitable->getElseBlock()) visitable->getElseBlock()->accept(this);
}

void AstDeclTypeAnnotator::visit(ForRangeClause *visitable) {
    visitable->getVariable()->accept(this);
    visitable->getIterExpr()->accept(this);
}

void AstDeclTypeAnnotator::visit(ForNormalClause *visitable) {
    visitable->getInit()->accept(this);
    visitable->getCond()->accept(this);
    visitable->getPost()->accept(this);
}

void AstDeclTypeAnnotator::visit(ForStmt *visitable) {
    visitable->getClause()->accept(this);
    visitable->getBody()->accept(this);
}

void AstDeclTypeAnnotator::visit(WhileStmt *visitable) {
    visitable->getCond()->accept(this);
    visitable->getBody()->accept(this);
}

void AstDeclTypeAnnotator::visit(AssignmentStmt *visitable) {
    visitable->getLhs()->accept(this);
    visitable->getRhs()->accept(this);
}

void AstDeclTypeAnnotator::visit(IncDecStmt *visitable) {
    visitable->getExpr()->accept(this);
}

void AstDeclTypeAnnotator::visit(ExpressionStmt *visitable) {
    visitable->getExpr()->accept(this);
}

void AstDeclTypeAnnotator::visit(Block *visitable) {
    auto parent = parentStack.top();
    auto scopename = "_b" + std::to_string(parent->getNextBlockCount());
    auto scope = parentStack.top()->getNestedScope(scopename);

    parentStack.push(scope);
    for (auto stmt: visitable->getStmts()) {
        stmt->accept(this);
    }
    parentStack.pop();
}

void AstDeclTypeAnnotator::visit(FunctionLit *visitable) {
    auto parent = parentStack.top();
    auto scopename = "_l" + std::to_string(parent->getNextLambdaCount());
    auto scope = parentStack.top()->getNestedScope(scopename);
    parentStack.push(scope);
    std::vector<Type *> paramTyps;
    for (auto param: visitable->getParameters()) {
        param->accept(this);
        paramTyps.push_back(param->getTyp());
    }
    Type *retTyp = typeParser.parseTypeExpr(visitable->getReturnTyp(), parent);
    if (!retTyp) throw SemanticError{"Unable to parse function lambda " + scope->getScopePrefix() + " return type"};
    auto fnTyp = typeContext.getFunctionTy(retTyp, paramTyps);
    visitable->setTyp(fnTyp);

    visitable->getBody()->accept(this);
    parentStack.pop();
}

void AstDeclTypeAnnotator::visit(MemberDecl *visitable) {
    visitable->getDeclaration()->accept(this);
    auto modifier = getVisibilityFromString(visitable->getModifier()->getIdent());
    auto type = visitable->getDeclaration()->getTyp();
    visitable->setTyp(
        typeContext.createOrGetMemberTy(
            modifier, type,
            dynamic_cast<AggregateType *>(
                parentStack.top()->getCurrentScopeReference("this")
            )
        )
    );
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

    if (visitable->getBody()) visitable->getBody()->accept(this);

    parentStack.pop();
}

void AstDeclTypeAnnotator::visit(ClassDecl *visitable) {
    auto classname = visitable->getClassname();
    auto parentScope = parentStack.top();
    auto classTy = dynamic_cast<ClassType *>(parentScope->getCurrentScopeReference(classname));
    auto scope = parentStack.top()->getNestedScope(classname);
    parentStack.push(scope);

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

    scope->addScopeMember("this", classTy);
    if (classTy->getBaseclass()) scope->addScopeMember("super", classTy->getBaseclass());

    for (auto member: visitable->getMembers()) {
        member->accept(this);
        if (!isInterfaceOrClassDecl(member->getDeclaration())) {
            auto membername = member->getName()->getIdent();
            auto type = scope->getCurrentScopeReference(membername);
            auto modifier = getVisibilityFromString(member->getModifier()->getIdent());
            classTy->addMember(membername, typeContext.createOrGetMemberTy(modifier, type, classTy));
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

    std::vector<InterfaceType *> interfaces;
    for (auto interfaceExpr: visitable->getInterfaces()) {
        auto interfaceQualifier = interfaceExpr->getIdent();
        auto interfaceQualifierLst = ScopeSymbolTypeTable::getQuantifierList(interfaceQualifier);
        auto derivedTy = dynamic_cast<InterfaceType *>(scope->findReferencedType(interfaceQualifierLst));
        if (!derivedTy)
            throw SemanticError{interfacename + " must extend from interface type not " + interfaceQualifier};
        interfaces.push_back(derivedTy);
    }
    interfaceTy->setInterfaces(interfaces);
    scope->addScopeMember("this", interfaceTy);

    for (auto member: visitable->getInterfaceMembers()) {
        member->accept(this);
        if (!isInterfaceOrClassDecl(member->getDeclaration())) {
            auto membername = member->getName()->getIdent();
            auto type = scope->getCurrentScopeReference(membername);
            auto modifier = getVisibilityFromString(member->getModifier()->getIdent());
            interfaceTy->addInterfaceMethod(membername, typeContext.createOrGetMemberTy(modifier, type, interfaceTy));
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

void AstDeclTypeAnnotator::visit(NullLit *visitable) {
    visitable->setTyp(typeContext.getPrimitiveTy(PrimType::Null));
}

void AstDeclTypeAnnotator::visit(BoolLit *visitable) {
    visitable->setTyp(typeContext.getPrimitiveTy(PrimType::Boolean));
}

bool isFloatNumber(const std::string &str) {
    return std::find(str.begin(), str.end(), '.') != str.end();
}

void AstDeclTypeAnnotator::visit(NumberLit *visitable) {
    auto value = visitable->getValue();
    if (isFloatNumber(value)) {
        visitable->setTyp(typeContext.getPrimitiveTy(PrimType::Number));
    } else {
        visitable->setTyp(typeContext.getPrimitiveTy(PrimType::Integer));
    }
}

void AstDeclTypeAnnotator::visit(StringLit *visitable) {
    // todo: string type
}

void AstDeclTypeAnnotator::annotate(CompilationUnit *unit) {
    visit(unit);
}

}
