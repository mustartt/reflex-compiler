//
// Created by henry on 2022-04-28.
//

#include "TypeParser.h"

#include "ASTLiteral.h"
#include "TypeContext.h"
#include "LexicalScope.h"

namespace reflex {

VoidType *TypeParser::parseVoidType(ReferenceTypenameExpr *expr, LexicalScope *) {
    auto qualifier = expr->getQualifiedString();
    if (qualifier == "void") {
        return context.getVoidType();
    }
    throw TypeError{"Cannot parse " + qualifier + " as void type"};
}

BuiltinType *TypeParser::parseBuiltinType(ReferenceTypenameExpr *expr, LexicalScope *) {
    const auto keyword = expr->getQualifiedString();
    if (keyword == "int") return context.getBuiltinType(BuiltinType::Integer);
    if (keyword == "num") return context.getBuiltinType(BuiltinType::Number);
    if (keyword == "char") return context.getBuiltinType(BuiltinType::Character);
    if (keyword == "bool") return context.getBuiltinType(BuiltinType::Boolean);

    throw TypeError{"Cannot parse " + keyword + " as builtin type"};
}

CompositeType *TypeParser::parseCompositeType(ReferenceTypenameExpr *expr, LexicalScope *starting) {
    auto qualifier = expr->getQualifiedString();
    try {
        auto found = starting->resolve(qualifier);
        auto compositeType = dynamic_cast<CompositeType *>(found->getMemberType());

        if (compositeType) return compositeType;
        throw TypeError{
            "Cannot reference a non class or interface type " +
                found->getMemberType()->getTypeString()
        };
    } catch (LexicalError &err) {
        throw TypeError{
            "Cannot find class or interface type " + qualifier +
                " due to " + err.what()
        };
    }
}

ArrayType *TypeParser::parseRefArrayType(ArrayTypeExpr *expr, LexicalScope *starting) {
    auto elementType = parseReferenceTypeExpr(expr->getElementType(), starting);
    auto length = expr->getSize();
    if (length) {
        if (length->getValue() < 0) throw TypeError{"Array length must be a non-negative integer"};
        auto len = (size_t)length->getValue();
        return context.getArrayType(elementType, {len});
    } else {
        return context.getArrayType(elementType);
    }
}

FunctionType *TypeParser::parseRefFunctionType(FunctionTypeExpr *expr, LexicalScope *starting) {
    std::vector<Type *> params;
    for (auto param: expr->getParamTypes()) {
        params.push_back(parseReferenceTypeExpr(param, starting));
    }
    auto returnType = parseReferenceTypeExpr(expr->getReturnType(), starting);
    return context.getFunctionType(returnType, params);
}

Type *TypeParser::parseReferenceTypeExpr(ASTTypeExpr *expr, LexicalScope *starting) {
    if (auto typname = dynamic_cast<ReferenceTypenameExpr *>(expr)) {
        try {
            return parseVoidType(typname, starting);
        } catch (TypeError &err) {}
        try {
            return parseBuiltinType(typname, starting);
        } catch (TypeError &err) {}
        return context.getReferenceType(parseCompositeType(typname, starting));
    }
    if (auto array = dynamic_cast<ArrayTypeExpr *>(expr)) {
        return context.getReferenceType(parseRefArrayType(array, starting));
    }
    if (auto func = dynamic_cast<FunctionTypeExpr *>(expr)) {
        return context.getReferenceType(parseRefFunctionType(func, starting));
    }
    throw TypeError{"Cannot parse type"};
}

ClassType *TypeParser::parseClassType(ReferenceTypenameExpr *expr, LexicalScope *starting) {
    auto compositeType = parseCompositeType(expr, starting);
    auto classType = dynamic_cast<ClassType *>(compositeType);
    if (!classType)
        throw TypeError{
            compositeType->getTypeString() + "is not a ClassType"
        };
    return classType;
}

InterfaceType *TypeParser::parseInterfaceType(ReferenceTypenameExpr *expr, LexicalScope *starting) {
    auto compositeType = parseCompositeType(expr, starting);
    auto interfaceType = dynamic_cast<InterfaceType *>(compositeType);
    if (!interfaceType)
        throw TypeError{
            compositeType->getTypeString() + "is not an InterfaceType"
        };
    return interfaceType;
}

ArrayType *TypeParser::parseArrayType(ArrayTypeExpr *expr, LexicalScope *starting) {
    auto elementType = parseBaseType(expr->getElementType(), starting);
    auto length = expr->getSize();
    if (length) {
        if (length->getValue() < 0) throw TypeError{"Array length must be a non-negative integer"};
        auto len = (size_t)length->getValue();
        return context.getArrayType(elementType, {len});
    } else {
        return context.getArrayType(elementType);
    }
}

FunctionType *TypeParser::parseFunctionType(FunctionTypeExpr *expr, LexicalScope *starting) {
    std::vector<Type *> params;
    for (auto param: expr->getParamTypes()) {
        params.push_back(parseBaseType(param, starting));
    }
    auto returnType = parseBaseType(expr->getReturnType(), starting);
    return context.getFunctionType(returnType, params);
}

Type *TypeParser::parseBaseType(ASTTypeExpr *expr, LexicalScope *starting) {
    if (auto typname = dynamic_cast<ReferenceTypenameExpr *>(expr)) {
        try {
            return parseVoidType(typname, starting);
        } catch (TypeError &err) {}
        try {
            return parseBuiltinType(typname, starting);
        } catch (TypeError &err) {}
        return parseCompositeType(typname, starting);
    }
    if (auto array = dynamic_cast<ArrayTypeExpr *>(expr)) {
        return parseArrayType(array, starting);
    }
    if (auto func = dynamic_cast<FunctionTypeExpr *>(expr)) {
        return parseFunctionType(func, starting);
    }
    throw TypeError{"Cannot parse type"};
}

}
