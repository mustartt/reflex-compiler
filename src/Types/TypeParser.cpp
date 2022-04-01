//
// Created by henry on 2022-03-25.
//

#include "AstNodes.h"
#include "TypeParser.h"
#include "TypeContextManager.h"
#include "ScopeSymbolTypeTable.h"

namespace reflex {

VoidType *TypeParser::parseVoidTypeExpr(IdentifierTypeExpr *decl, ScopeSymbolTypeTable *scope) {
    if (decl->getTypename()->getIdent() == "void")
        return typeContext.getVoidTy();
    return nullptr;
}

PrimType *TypeParser::parsePrimTypeExpr(IdentifierTypeExpr *decl, ScopeSymbolTypeTable *scope) {
    auto keyword = decl->getTypename()->getIdent();
    if (keyword == "int") return typeContext.getPrimitiveTy(PrimType::Integer);
    if (keyword == "num") return typeContext.getPrimitiveTy(PrimType::Number);
    if (keyword == "char") return typeContext.getPrimitiveTy(PrimType::Character);
    if (keyword == "bool") return typeContext.getPrimitiveTy(PrimType::Boolean);
    return nullptr;
}

ArrayType *TypeParser::parseArrTypeExpr(ArrayTypeExpr *decl, ScopeSymbolTypeTable *scope) {
    auto elementTyp = parseTypeExpr(decl->getElementTyp(), scope);
    if (!elementTyp) return nullptr;
    return typeContext.getArrayTy(elementTyp);
}

FunctionType *TypeParser::parseFuncTypeExpr(FunctionTypeExpr *decl, ScopeSymbolTypeTable *scope) {
    std::vector<Type *> params;
    for (auto param: decl->getParamList()) {
        params.push_back(parseTypeExpr(param, scope));
        if (!params.back()) return nullptr;
    }
    auto returnType = parseTypeExpr(decl->getReturnTyp(), scope);
    if (!returnType) return nullptr;
    return typeContext.getFunctionTy(returnType, params);
}

AggregateType *TypeParser::parseAggregateExpr(IdentifierTypeExpr *decl, ScopeSymbolTypeTable *scope) {
    auto name = decl->getTypename()->getIdent();
    auto qualifierLst = ScopeSymbolTypeTable::getQuantifierList(name);
    return dynamic_cast<AggregateType *>(scope->findReferencedType(qualifierLst));
}

Type *TypeParser::parseTypeExpr(TypeExpr *decl, ScopeSymbolTypeTable *scope) {
    if (auto ptr = dynamic_cast<IdentifierTypeExpr *>(decl)) {
        if (auto res = parseVoidTypeExpr(ptr, scope)) return res;
        if (auto res = parsePrimTypeExpr(ptr, scope)) return res;
        if (auto res = parseAggregateExpr(ptr, scope)) return res;
        return nullptr;
    }
    if (auto ptr = dynamic_cast<ArrayTypeExpr *>(decl)) {
        return parseArrTypeExpr(ptr, scope);
    }
    if (auto ptr = dynamic_cast<FunctionTypeExpr *>(decl)) {
        return parseFuncTypeExpr(ptr, scope);
    }
    return nullptr;
}

}
