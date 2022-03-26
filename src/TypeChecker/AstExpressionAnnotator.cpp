//
// Created by henry on 2022-03-26.
//

#include <unordered_set>

#include "AstExpressionAnnotator.h"
#include "AstNodes.h"
#include "Utils/OperatorSemantics.h"

namespace reflex {

AstExpressionAnnotator::AstExpressionAnnotator(TypeContextManager *typeContext,
                                               AstContextManager *astContext,
                                               CompilationUnit *root)
    : astContext(astContext), typeContext(typeContext), root(root) {}

void AstExpressionAnnotator::annotate() {
    root->accept(this);
}

// sets up the global symbol table
void AstExpressionAnnotator::visit(CompilationUnit *visitable) {
    symbolTable.push(std::make_unique<ScopeSymbolTable>());
    auto &table = symbolTable.top();
    for (auto decl: visitable->getDecls()) {
        table->addScopeMember(decl->getName()->getIdent(), decl->getTyp());
    }
    for (auto decl: visitable->getDecls()) {
        decl->accept(this);
    }
    symbolTable.pop();
}

void AstExpressionAnnotator::visit(VariableDecl *visitable) {
    if (visitable->getInitializer()) {
        visitable->getInitializer()->accept(this);
        auto initTyp = typeStack.top();
        typeStack.pop();
        if (initTyp != visitable->getTyp()
            && !OperatorSemantics::primitiveTypeImplicitConvertible(initTyp, visitable->getTyp()))
            throw SemanticError{"VarDecl initializer type mismatch"};
    }
    auto &table = symbolTable.top();
    table->addScopeMember(visitable->getName()->getIdent(), visitable->getTyp());
}

void AstExpressionAnnotator::visit(BinaryExpr *visitable) {
    visitable->getLhs()->accept(this);
    auto lhsTyp = typeStack.top();
    typeStack.pop();

    visitable->getRhs()->accept(this);
    auto rhsTyp = typeStack.top();
    typeStack.pop();

    auto binExprTy = OperatorSemantics::getBinaryOperatorResultType(typeContext, visitable->getOp(), lhsTyp, rhsTyp);
    if (!binExprTy) throw SemanticError{"BinaryExpr operand type mismatch"};
    visitable->setTyp(binExprTy);
    typeStack.push(binExprTy);
}

void AstExpressionAnnotator::visit(Identifier *visitable) {
    auto &table = symbolTable.top();
    auto identifier = visitable->getIdent();
    auto referencedType = table->getReferencedType(identifier);
    if (!referencedType) throw SemanticError{"Undefined reference to " + identifier};
    visitable->setTyp(referencedType);
    typeStack.push(referencedType);
}

void AstExpressionAnnotator::visit(NumberLit *visitable) {
    typeStack.push(visitable->getTyp());
}

void AstExpressionAnnotator::visit(BoolLit *visitable) {
    typeStack.push(visitable->getTyp());
}

void AstExpressionAnnotator::visit(NullLit *visitable) {
    typeStack.push(visitable->getTyp());
}

void AstExpressionAnnotator::visit(UnaryExpr *visitable) {
    visitable->getExpr()->accept(this);
    auto exprTyp = typeStack.top();
    auto resTy = OperatorSemantics::getUnaryOperatorResultType(typeContext, visitable->getOp(), exprTyp);
    if (!resTy) throw SemanticError{"UnaryExpr invalid operand type"};
    visitable->setTyp(exprTyp);
}

void AstExpressionAnnotator::visit(FunctionDecl *visitable) {
    symbolTable.push(std::make_unique<ScopeSymbolTable>(symbolTable.top().get()));
    auto &table = symbolTable.top();
    for (auto paramDecl: visitable->getParams()) {
        table->addScopeMember(paramDecl->getName()->getIdent(), paramDecl->getTyp());
    }
    visitable->getBody()->accept(this);
    auto blockRetTyp = typeStack.top();
    typeStack.pop();
    auto funcType = dynamic_cast<FunctionType *>(table->getReferencedType(visitable->getName()->getIdent()));
    if (blockRetTyp != visitable->getTyp() &&
        !OperatorSemantics::primitiveTypeImplicitConvertible(blockRetTyp, funcType->getReturnTyp())) {
//        throw SemanticError{"Mismatched function return type"};
    }
    symbolTable.pop();
}

void AstExpressionAnnotator::visit(Block *visitable) {
    symbolTable.push(std::make_unique<ScopeSymbolTable>(symbolTable.top().get()));
    auto &table = symbolTable.top();

    std::unordered_set<Type *> retTypes;
    for (auto stmt: visitable->getStmts()) {
        stmt->accept(this);
        if (typeid(*stmt) == typeid(ReturnStmt) ||
            typeid(*stmt) == typeid(Block)) {
            auto typ = typeStack.top();
            typeStack.pop();
            retTypes.insert(typ);
        }
    }

//    retTypes.erase(typeContext->getPrimitiveTy(PrimType::Null));
    if (retTypes.empty()) {
        visitable->setTyp(typeContext->getVoidTy());
        typeStack.push(typeContext->getVoidTy());
        return;
    }
//    if (retTypes.size() > 1) throw SemanticError{"Too many return types with in a block"};
    auto ty = *retTypes.begin();
    visitable->setTyp(ty);
    typeStack.push(ty);
    symbolTable.pop();
}

void AstExpressionAnnotator::visit(ReturnStmt *visitable) {
    if (visitable->getReturnValue()) {
        visitable->getReturnValue()->accept(this);
        auto typ = typeStack.top();
        visitable->setTyp(typ);
    } else {
        visitable->setTyp(typeContext->getVoidTy());
        typeStack.push(typeContext->getVoidTy());
    }
}

void AstExpressionAnnotator::visit(BreakStmt *visitable) {
    visitable->setTyp(typeContext->getVoidTy());
}

void AstExpressionAnnotator::visit(ContinueStmt *visitable) {
    visitable->setTyp(typeContext->getVoidTy());
}

void AstExpressionAnnotator::visit(EmptyStmt *visitable) {
    visitable->setTyp(typeContext->getVoidTy());
}

void AstExpressionAnnotator::visit(SelectorExpr *visitable) {
    visitable->getBaseExpr()->accept(this);
    auto baseType = typeStack.top();
    typeStack.pop();
    if (baseType->isBasicType()) throw SemanticError{"PrimType does not have attributes"};
    auto selectorname = visitable->getSelector()->getIdent();
    auto selectorTy = dynamic_cast<AggregateType *>(baseType)->findMemberTyp(selectorname);
    if (!selectorTy) throw SemanticError{"Aggregate type does not have attributes ." + selectorname};
    visitable->setTyp(selectorTy);
    typeStack.push(selectorTy);
}

void AstExpressionAnnotator::visit(ExpressionStmt *visitable) {
    visitable->getExpr()->accept(this);
    auto typ = typeStack.top();
    typeStack.pop();
    visitable->setTyp(typ);
}

}
