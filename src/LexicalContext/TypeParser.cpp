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
    auto firstPos = qualifier.find("::");
    auto prefix = qualifier.substr(0, firstPos);
    auto rest = qualifier.substr(firstPos + 2);
    try {
        auto member = starting->bind(prefix);
        auto found = member->follow(rest);
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

ArrayType *TypeParser::parseArrayType(ArrayTypeExpr *expr, LexicalScope *starting) {
    auto elementType = parseTypeExpr(expr->getElementType(), starting);
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
        params.push_back(parseTypeExpr(param, starting));
    }
    auto returnType = parseTypeExpr(expr->getReturnType(), starting);
    return context.getFunctionType(returnType, params);
}

Type *TypeParser::parseTypeExpr(ASTTypeExpr *expr, LexicalScope *starting) {
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
        return context.getReferenceType(parseArrayType(array, starting));
    }
    if (auto func = dynamic_cast<FunctionTypeExpr *>(expr)) {
        return context.getReferenceType(parseFunctionType(func, starting));
    }
    throw TypeError{"Cannot parse type"};
}

ClassType *TypeParser::parseReferencedClassType(ReferenceTypenameExpr *expr, LexicalScope *starting) {
    auto compositeType = parseCompositeType(expr, starting);
    auto classType = dynamic_cast<ClassType *>(compositeType);
    if (!classType)
        throw TypeError{
            compositeType->getTypeString() + "is not a ClassType"
        };
    return classType;
}

InterfaceType *TypeParser::parseReferencedInterfaceType(ReferenceTypenameExpr *expr, LexicalScope *starting) {
    auto compositeType = parseCompositeType(expr, starting);
    auto interfaceType = dynamic_cast<InterfaceType *>(compositeType);
    if (!interfaceType)
        throw TypeError{
            compositeType->getTypeString() + "is not an InterfaceType"
        };
    return interfaceType;
}

}
