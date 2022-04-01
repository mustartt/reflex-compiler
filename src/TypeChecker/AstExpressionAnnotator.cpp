//
// Created by henry on 2022-04-01.
//

#include "AstNodes.h"
#include "AstExpressionAnnotator.h"
#include "TypeContextManager.h"
#include "ScopeSymbolTypeTable.h"
#include "Utils/TypeCheckerUtils.h"
#include "SemanticError.h"

namespace reflex {

AstExpressionAnnotator::AstExpressionAnnotator(TypeContextManager &typeContext,
                                               std::unique_ptr<ScopeSymbolTypeTable> &root)
    : typeContext(typeContext), root(root) {}

void AstExpressionAnnotator::visit(NumberLit *visitable) {
    typeStack.push(visitable->getTyp());
}

void AstExpressionAnnotator::visit(StringLit *visitable) {
    typeStack.push(visitable->getTyp());
}

void AstExpressionAnnotator::visit(BoolLit *visitable) {
    typeStack.push(visitable->getTyp());
}

void AstExpressionAnnotator::visit(NullLit *visitable) {
    typeStack.push(visitable->getTyp());
}

void AstExpressionAnnotator::visit(ArrayLit *visitable) {
    for (auto val: visitable->getInitializerList()) {
        val->accept(this);
        auto ty = popTypeStack();
    }
    visitable->setTyp(typeHint);
    typeStack.push(typeHint);
}

void AstExpressionAnnotator::visit(Identifier *visitable) {
    auto identifier = visitable->getIdent();
    auto ty = parentStack.top()->lookupIdentifierType(identifier);
    visitable->setTyp(ty);
    typeStack.push(ty);
}

void AstExpressionAnnotator::visit(UnaryExpr *visitable) {
    visitable->getExpr()->accept(this);
    auto ty = popTypeStack();
    visitable->setTyp(ty);
    typeStack.push(ty);
}

void AstExpressionAnnotator::visit(BinaryExpr *visitable) {
    visitable->getLhs()->accept(this);
    auto lhsTy = popTypeStack();
    visitable->getRhs()->accept(this);
    auto rhsTy = popTypeStack();

    visitable->setTyp(lhsTy);
    typeStack.push(lhsTy);
}

void AstExpressionAnnotator::visit(NewExpr *visitable) {
    typeStack.push(visitable->getTyp());
}

void AstExpressionAnnotator::visit(CastExpr *visitable) {
    visitable->getFrom()->accept(this);
    auto ty = popTypeStack();

    typeStack.push(visitable->getTyp());
}

void AstExpressionAnnotator::visit(SelectorExpr *visitable) {
    visitable->getBaseExpr()->accept(this);
    auto ty = popTypeStack();
    if (auto recordTy = dynamic_cast<AggregateType *>(ty)) {
        auto member = recordTy->findMemberTyp(visitable->getSelector()->getIdent());
        visitable->setTyp(member->getMemberTyp());
        typeStack.push(member->getMemberTyp());
    } else {
        throw SemanticError{"Cannot access property of non record type"};
    }
}

void AstExpressionAnnotator::visit(IndexExpr *visitable) {
    visitable->getBaseExpr()->accept(this);
    auto ty = popTypeStack();
    visitable->getIndex()->accept(this);
    popTypeStack();
    if (auto arrTy = dynamic_cast<ArrayType *>(ty)) {
        visitable->setTyp(arrTy->getElementTyp());
        typeStack.push(arrTy->getElementTyp());
    } else {
        throw SemanticError{"Can only index into array type"};
    }
}

void AstExpressionAnnotator::visit(ArgumentExpr *visitable) {
    visitable->getBaseExpr()->accept(this);
    auto ty = popTypeStack();

    std::vector<Type *> arguments;
    for (auto arg: visitable->getArguments()) {
        arg->accept(this);
        arguments.push_back(popTypeStack());
    }

    if (auto func = dynamic_cast<FunctionType *>(ty)) {
        visitable->setTyp(func->getReturnTyp());
        typeStack.push(func->getReturnTyp());
    } else if (auto instance = dynamic_cast<NewExpr *>(visitable->getBaseExpr())) {
        visitable->setTyp(instance->getTyp());
        typeStack.push(instance->getTyp());
    }
}

void AstExpressionAnnotator::visit(ReturnStmt *visitable) {
    if (visitable->getReturnValue()) {
        visitable->getReturnValue()->accept(this);
        auto retTy = popTypeStack();
        visitable->setTyp(retTy);
    } else {
        visitable->setTyp(typeContext.getVoidTy());
    }
}

void AstExpressionAnnotator::visit(BreakStmt *visitable) {
    visitable->setTyp(typeContext.getVoidTy());
}

void AstExpressionAnnotator::visit(ContinueStmt *visitable) {
    visitable->setTyp(typeContext.getVoidTy());
}

void AstExpressionAnnotator::visit(IfStmt *visitable) {
    visitable->getCond()->accept(this);
    visitable->getPrimaryBlock()->accept(this);
    if (visitable->getElseBlock()) visitable->getElseBlock()->accept(this);
    visitable->setTyp(typeContext.getVoidTy());
}

void AstExpressionAnnotator::visit(ForRangeClause *visitable) {
    visitable->getVariable()->accept(this);
    visitable->getIterExpr()->accept(this);
    visitable->setTyp(typeContext.getVoidTy());
}

void AstExpressionAnnotator::visit(ForNormalClause *visitable) {
    visitable->getInit()->accept(this);
    visitable->getCond()->accept(this);
    visitable->getPost()->accept(this);
    visitable->setTyp(typeContext.getVoidTy());
}

void AstExpressionAnnotator::visit(ForStmt *visitable) {
    visitable->getClause()->accept(this);
    visitable->getBody()->accept(this);
    visitable->setTyp(typeContext.getVoidTy());
}

void AstExpressionAnnotator::visit(WhileStmt *visitable) {
    visitable->getCond()->accept(this);
    visitable->getBody()->accept(this);
    visitable->setTyp(typeContext.getVoidTy());
}

void AstExpressionAnnotator::visit(EmptyStmt *visitable) {
    visitable->setTyp(typeContext.getVoidTy());
}

void AstExpressionAnnotator::visit(AssignmentStmt *visitable) {
    visitable->getLhs()->accept(this);
    auto lhsTy = popTypeStack();
    typeHint = lhsTy;
    visitable->getRhs()->accept(this);
    auto rhsTy = popTypeStack();
    visitable->setTyp(typeContext.getVoidTy());
}

void AstExpressionAnnotator::visit(IncDecStmt *visitable) {
    visitable->getExpr()->accept(this);
    auto ty = popTypeStack();
    visitable->setTyp(ty);
}

void AstExpressionAnnotator::visit(ExpressionStmt *visitable) {
    visitable->setTyp(typeContext.getVoidTy());
    visitable->getExpr()->accept(this);
    popTypeStack();
}

void AstExpressionAnnotator::visit(VariableDecl *visitable) {
    if (visitable->getInitializer()) {
        visitable->getInitializer()->accept(this);
        typeHint = visitable->getTyp();
        popTypeStack();
    }
}

void AstExpressionAnnotator::visit(Block *visitable) {
    auto parent = parentStack.top();
    auto scopename = "_b" + std::to_string(parent->getNextBlockCount());
    auto scope = parentStack.top()->getNestedScope(scopename);
    parentStack.push(scope);
    for (auto stmt: visitable->getStmts()) {
        stmt->accept(this);
    }
    parentStack.pop();
    visitable->setTyp(typeContext.getVoidTy());
}

void AstExpressionAnnotator::visit(FunctionLit *visitable) {
    auto parent = parentStack.top();
    auto scopename = "_l" + std::to_string(parent->getNextLambdaCount());
    auto scope = parentStack.top()->getNestedScope(scopename);
    parentStack.push(scope);
    visitable->getBody()->accept(this);
    parentStack.pop();
    typeStack.push(visitable->getTyp());
}

void AstExpressionAnnotator::visit(FunctionDecl *visitable) {
    auto funcname = visitable->getName()->getIdent();
    auto scope = parentStack.top()->getNestedScope(funcname);
    parentStack.push(scope);
    if (visitable->getBody()) visitable->getBody()->accept(this);
    parentStack.pop();
}

void AstExpressionAnnotator::visit(MemberDecl *visitable) {
    visitable->getDeclaration()->accept(this);
}

void AstExpressionAnnotator::visit(ClassDecl *visitable) {
    auto classname = visitable->getClassname();
    auto scope = parentStack.top()->getNestedScope(classname);
    parentStack.push(scope);
    for (auto member: visitable->getMembers()) {
        if (!isInterfaceOrClassDecl(member->getDeclaration())) {
            member->accept(this);
        }
    }
    parentStack.pop();
}

void AstExpressionAnnotator::visit(InterfaceDecl *visitable) {
    auto interfacename = visitable->getInterfaceName();
    auto scope = parentStack.top()->getNestedScope(interfacename);
    parentStack.push(scope);
    for (auto member: visitable->getInterfaceMembers()) {
        if (!isInterfaceOrClassDecl(member->getDeclaration())) {
            member->accept(this);
        }
    }
    parentStack.pop();
}

void AstExpressionAnnotator::visit(CompilationUnit *visitable) {
    auto &scope = root;
    parentStack.push(scope.get());
    for (auto decl: visitable->getDecls()) {
        decl->accept(this);
    }
    parentStack.pop();
    visitable->setTyp(typeContext.getVoidTy());
}

void AstExpressionAnnotator::annotate(CompilationUnit *unit) {
    visit(unit);
}

Type *AstExpressionAnnotator::popTypeStack() {
    auto ty = typeStack.top();
    typeStack.pop();
    return ty;
}

}
