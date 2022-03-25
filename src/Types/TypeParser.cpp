//
// Created by henry on 2022-03-25.
//

#include "TypeParser.h"

namespace reflex {

VoidType *TypeParser::parseVoidTypeExpr(IdentifierType *decl) {
    if (decl->getTypename()->getIdent() == "void")
        return typeContext->getVoidTy();
    return nullptr;
}

PrimType *TypeParser::parsePrimTypeExpr(IdentifierType *decl) {
    auto keyword = decl->getTypename()->getIdent();
    if (keyword == "int") return typeContext->getPrimitiveTy(PrimType::Integer);
    if (keyword == "num") return typeContext->getPrimitiveTy(PrimType::Number);
    if (keyword == "char") return typeContext->getPrimitiveTy(PrimType::Character);
    if (keyword == "bool") return typeContext->getPrimitiveTy(PrimType::Boolean);
    return nullptr;
}

ArrType *TypeParser::parseArrTypeExpr(ArrayType *decl) {
    auto elementTyp = parseTypeExpr(decl->getElementTyp());
    if (!elementTyp) return nullptr;
    return typeContext->getArrayTy(elementTyp);
}

FuncType *TypeParser::parseFuncTypeExpr(FunctionType *decl) {
    std::vector<Type *> params;
    for (auto param: decl->getParamList()) {
        params.push_back(parseTypeExpr(param));
        if (!params.back()) return nullptr;
    }
    auto returnType = parseTypeExpr(decl->getReturnTyp());
    if (!returnType) return nullptr;
    return typeContext->getFunctionTy(returnType, params);
}

AggregateType *TypeParser::parseAggregateExpr(IdentifierType *decl) {
    auto name = decl->getTypename()->getIdent();
    auto classTyp = typeContext->getClassTyp(name);
    auto interfaceTyp = typeContext->getInterfaceTyp(name);
    if (!classTyp && !interfaceTyp) return nullptr;
    return classTyp ? static_cast<AggregateType *>(classTyp.value())
                    : static_cast<AggregateType *>(interfaceTyp.value());
}

Type *TypeParser::parseTypeExpr(TypeExpr *decl) {
    if (auto ptr = dynamic_cast<IdentifierType *>(decl)) {
        if (auto res = parseVoidTypeExpr(ptr)) return res;
        if (auto res = parsePrimTypeExpr(ptr)) return res;
        if (auto res = parseAggregateExpr(ptr)) return res;
        return nullptr;
    }
    if (auto ptr = dynamic_cast<ArrayType *>(decl)) {
        return parseArrTypeExpr(ptr);
    }
    if (auto ptr = dynamic_cast<FunctionType *>(decl)) {
        return parseFuncTypeExpr(ptr);
    }
    return nullptr;
}

}
