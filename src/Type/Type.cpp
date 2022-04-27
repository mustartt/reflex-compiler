//
// Created by henry on 2022-04-26.
//

#include "Type.h"

#include <numeric>

namespace reflex {

std::string BuiltinType::getTypeString() const {
    switch (baseType) {
        case Integer: return "int";
        case Number: return "num";
        case Character: return "char";
        case Boolean: return "bool";
    }
}

std::string ArrayType::getTypeString() const {
    auto base = elementType->getTypeString() + "[";
    if (hasDefinedSize()) {
        base += std::to_string(size.value()) + "]";
    } else {
        base += "]";
    }
    return base;
}

std::string reflex::FunctionType::getTypeString() const {
    std::string base = "func(";
    if (!paramTypes.empty()) {
        base += paramTypes[0]->getTypeString();
        for (size_t i = 1; i < paramTypes.size(); ++i) {
            base += "," + paramTypes[i]->getTypeString();
        }
    }
    base += ")->" + returnType->getTypeString();
    return base;
}

std::string MemberAttrType::getTypeString() const {
    return getVisibilityString(visibility) + " " + type->getTypeString();
}

std::string ReferenceType::getTypeString() const {
    return "&" + refType->getTypeString() + (nullable ? "?" : "");
}

TypeError::TypeError(const std::string &arg)
    : runtime_error("CompilerTypeError: " + arg) {}
}

