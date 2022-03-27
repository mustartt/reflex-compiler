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
    auto varname = visitable->getName()->getIdent();
    if (!table->isInCurrentScope(varname)) {
        table->addScopeMember(varname, visitable->getTyp());
    }
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
    auto memberType = table->getReferencedType(visitable->getName()->getIdent());
    auto funcType = dynamic_cast<FunctionType *>(memberType);
    if (blockRetTyp != visitable->getTyp() &&
        !OperatorSemantics::primitiveTypeImplicitConvertible(blockRetTyp, funcType->getReturnTyp())) {
//        throw SemanticError{"Mismatched function return type"};
    }
    symbolTable.pop();
}

void AstExpressionAnnotator::visit(Block *visitable) {
    symbolTable.push(std::make_unique<ScopeSymbolTable>(symbolTable.top().get()));
    auto &table = symbolTable.top();

    if (visitable->getStmts().empty()) {
        typeStack.push(typeContext->getVoidTy());
        visitable->setTyp(typeContext->getVoidTy());
        return;
    }

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

void AstExpressionAnnotator::visit(ClassDecl *visitable) {
    symbolTable.push(std::make_unique<ScopeSymbolTable>(symbolTable.top().get()));
    auto &table = symbolTable.top();
    for (auto decl: visitable->getMembers()) {
        auto member = decl->getDeclaration();
        table->addScopeMember(member->getName()->getIdent(), member->getTyp());
    }
    for (auto decl: visitable->getMembers()) {
        decl->accept(this);
    }
    symbolTable.pop();
}

void AstExpressionAnnotator::visit(MemberDecl *visitable) {
    visitable->getDeclaration()->accept(this);
}

void AstExpressionAnnotator::visit(NewExpr *visitable) {
    typeStack.push(visitable->getTyp());
}

void AstExpressionAnnotator::visit(CastExpr *visitable) {
    visitable->getFrom()->accept(this);
    auto fromTyp = typeStack.top();
    typeStack.pop();
    auto toTyp = visitable->getTyp();
    // todo: check type cast compatibility
    typeStack.push(toTyp);
}

void AstExpressionAnnotator::visit(ArgumentExpr *visitable) {
    visitable->getBaseExpr()->accept(this);
    auto baseTy = visitable->getBaseExpr()->getTyp();
    if (baseTy->isBasicType())
        throw SemanticError{"BasicType " + baseTy->getTypeString() + " cannot accept argument"};
    if (auto func = dynamic_cast<FunctionType *>(baseTy)) {
        visitable->setTyp(func->getReturnTyp());
        typeStack.push(func->getReturnTyp());
        if (visitable->getArguments().size() != func->getParamTyp().size()) {
            throw SemanticError{
                "Expected " + std::to_string(func->getParamTyp().size()) + " arguments " +
                    "but received " + std::to_string(visitable->getArguments().size()) + " arguments"
            };
        }
        for (auto arg: visitable->getArguments()) {
            arg->accept(this);
        }
        // todo: check type compatibility
        return;
    }
    if (auto cls = dynamic_cast<ClassType *>(baseTy)) {
        // todo: class instantiation, check for constructors
        typeStack.push(cls);
        visitable->setTyp(cls);
        return;
    }
    if (auto interface = dynamic_cast<InterfaceType *>(baseTy)) {
        throw SemanticError{"InterfaceType " + baseTy->getTypeString() + " cannot be instantiated"};
    }
    throw SemanticError{"Cannot apply argument to type " + baseTy->getTypeString()};
}

void AstExpressionAnnotator::visit(IndexExpr *visitable) {
    visitable->getBaseExpr()->accept(this);
    auto baseTy = typeStack.top();
    auto baseArrTy = dynamic_cast<ArrayType *>(baseTy);
    typeStack.pop();
    if (!baseArrTy) throw SemanticError{"Cannot index into type " + baseTy->getTypeString()};
    visitable->getIndex()->accept(this);
    auto indexTy = dynamic_cast<PrimType *>(typeStack.top());
    typeStack.pop();
    // todo: check for type compatibility
    if (!indexTy || indexTy->getBaseTyp() != PrimType::Integer)
        throw SemanticError{"Array index must be convertible to an integer"};
    typeStack.push(baseArrTy->getElementTyp());
    visitable->setTyp(baseArrTy->getElementTyp());
}

void AstExpressionAnnotator::visit(AssignmentStmt *visitable) {
    visitable->getLhs()->accept(this);
    auto lhsTy = typeStack.top();
    typeStack.pop();
    visitable->getRhs()->accept(this);
    auto rhsTy = typeStack.top();
    typeStack.pop();
    if (lhsTy != rhsTy) throw SemanticError{"Assignment type mismatch"};
    visitable->setTyp(lhsTy);
}

void AstExpressionAnnotator::visit(IfStmt *visitable) {
    visitable->setTyp(typeContext->getVoidTy());
    visitable->getCond()->accept(this);
    visitable->getPrimaryBlock()->accept(this);
    if (visitable->getElseBlock()) visitable->getElseBlock()->accept(this);
}

void AstExpressionAnnotator::visit(ForNormalClause *visitable) {
    visitable->setTyp(typeContext->getVoidTy());
    visitable->getInit()->accept(this);
    if (visitable->getCond())
        visitable->getCond()->accept(this);
    visitable->getPost()->accept(this);
}

void AstExpressionAnnotator::visit(ForStmt *visitable) {
    visitable->setTyp(typeContext->getVoidTy());
    visitable->getClause()->accept(this);
    visitable->getBody()->accept(this);
}

void AstExpressionAnnotator::visit(WhileStmt *visitable) {
    visitable->setTyp(typeContext->getVoidTy());
    visitable->getCond()->accept(this);
    visitable->getBody()->accept(this);
}

void AstExpressionAnnotator::visit(ArrayLit *visitable) {
    std::unordered_set<Type *> lstType;
    for (auto elem: visitable->getInitializerList()) {
        elem->accept(this);
        lstType.insert(typeStack.top());
        typeStack.pop();
    }

    ArrayType *arrTy;
    if (lstType.empty()) {
        // todo: provides type hints but not implemented
        auto typeHint = typeStack.top();
        arrTy = typeContext->getArrayTy(typeHint);
    } else {
        if (lstType.size() > 1) throw SemanticError{"ArrayLit contains too many different types"};
        auto type = *lstType.begin();
        arrTy = typeContext->getArrayTy(type);
    }
    visitable->setTyp(arrTy);
    typeStack.push(arrTy);
}

void AstExpressionAnnotator::visit(FunctionLit *visitable) {
    symbolTable.push(std::make_unique<ScopeSymbolTable>(symbolTable.top().get()));
    auto &table = symbolTable.top();
    std::vector<Type *> paramTypes;
    for (auto paramDecl: visitable->getParameters()) {
        table->addScopeMember(paramDecl->getName()->getIdent(), paramDecl->getTyp());
        paramTypes.push_back(paramDecl->getTyp());
    }
    visitable->getBody()->accept(this);
    auto blockRetTyp = typeStack.top();
    typeStack.pop();
    auto funcType = typeContext->getFunctionTy(blockRetTyp, paramTypes);
    visitable->setTyp(funcType);
    typeStack.push(funcType);

    symbolTable.pop();
}

}
