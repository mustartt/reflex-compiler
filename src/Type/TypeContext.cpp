//
// Created by henry on 2022-04-27.
//

#include "TypeContext.h"

namespace reflex {

TypeContext::TypeContext() {
    voidType = std::make_unique<VoidType>();
    builtinType.push_back(std::make_unique<BuiltinType>(BuiltinType::Integer));
    builtinType.push_back(std::make_unique<BuiltinType>(BuiltinType::Number));
    builtinType.push_back(std::make_unique<BuiltinType>(BuiltinType::Character));
    builtinType.push_back(std::make_unique<BuiltinType>(BuiltinType::Boolean));
}

VoidType *TypeContext::getVoidType() const {
    return voidType.get();
}

BuiltinType *TypeContext::getBuiltinType(BuiltinType::BaseType typ) const {
    return builtinType[typ].get();
}

ArrayType *TypeContext::getArrayType(Type *elementTyp, std::optional<size_t> size) {
    auto res = std::find_if(arrayType.begin(), arrayType.end(),
                            [&size, elementTyp](const auto &existing) {
                              return existing->getElementType() == elementTyp
                                  && existing->getSize() == size;
                            });
    if (res == arrayType.end()) {
        arrayType.push_back(std::make_unique<ArrayType>(elementTyp, size));
        return arrayType.back().get();
    }
    return res->get();
}

FunctionType *TypeContext::getFunctionType(Type *returnType, const std::vector<Type *> &paramTyp) {
    auto res = std::find_if(funcType.begin(), funcType.end(),
                            [&paramTyp, returnType](const auto &existing) {
                              return existing->getParamTypes() == paramTyp
                                  && existing->getReturnType() == returnType;
                            });
    if (res != funcType.end()) {
        return res->get();
    }
    funcType.push_back(std::make_unique<FunctionType>(paramTyp, returnType));
    return funcType.back().get();
}

MemberAttrType *TypeContext::getMemberType(Visibility visibility, CompositeType *parent, Type *type) {
    auto res = std::find_if(memberType.begin(), memberType.end(),
                            [=](const auto &existing) {
                              return existing->getMemberAttrType() == type
                                  && existing->getParent() == parent
                                  && existing->getVisibility() == visibility;
                            });
    if (res != memberType.end()) {
        return res->get();
    }
    memberType.push_back(std::make_unique<MemberAttrType>(visibility, parent, type));
    return memberType.back().get();
}

ClassType *TypeContext::getClassType(const std::string &name, AggregateDecl *decl) {
    if (classType.contains(name)) {
        if (!classType[name]->getDecl())
            classType[name]->setDecl(decl);
        return classType[name].get();
    }
    classType[name] = std::make_unique<ClassType>(name, decl);
    return classType[name].get();
}

InterfaceType *TypeContext::getInterfaceType(const std::string &name, AggregateDecl *decl) {
    if (interfaceType.contains(name)) {
        if (!interfaceType[name]->getDecl())
            interfaceType[name]->setDecl(decl);
        return interfaceType[name].get();
    }
    interfaceType[name] = std::make_unique<InterfaceType>(name, decl);
    return interfaceType[name].get();
}

ReferenceType *TypeContext::getReferenceType(ReferenceableType *type, bool nullable) {
    auto res = std::find_if(referenceType.begin(), referenceType.end(),
                            [=](const auto &existing) {
                              return existing->getRefType() == type
                                  && existing->isNullable() == nullable;
                            });
    if (res == referenceType.end()) {
        referenceType.push_back(std::make_unique<ReferenceType>(type, nullable));
        return referenceType.back().get();
    }
    return res->get();
}

void TypeContext::dump(std::ostream &os) {}

}
