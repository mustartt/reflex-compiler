//
// Created by henry on 2022-04-27.
//

#include "TypeContext.h"

#include <ostream>

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

void TypeContext::dump(std::ostream &os) {
    os << "VoidType:" << std::endl;
    os << "  " << voidType->getTypeString() << " " << std::hex << voidType.get() << std::endl;
    os << "BuiltinType:" << std::endl;
    for (const auto &builtin: builtinType) {
        os << "  " << builtin->getTypeString() << " " << std::hex << builtin.get() << std::endl;
    }
    os << "ArrayType:" << std::endl;
    for (const auto &arr: arrayType) {
        os << "  " << arr->getTypeString() << " " << std::hex << arr.get() << std::endl;
    }
    os << "FunctionType:" << std::endl;
    for (const auto &func: funcType) {
        os << "  " << func->getTypeString() << " " << std::hex << func.get() << std::endl;
    }
    os << "ReferenceType:" << std::endl;
    for (const auto &ref: referenceType) {
        os << "  " << ref->getTypeString() << " " << std::hex << ref.get() << std::endl;
    }
    os << "InterfaceType:" << std::endl;
    for (const auto &[name, interface]: interfaceType) {
        os << "  interface " << interface->getTypeString() << " ";
        if (!interface->getInterfaces().empty()) {
            os << ": " << interface->getInterfaces()[0]->getTypeString();
            for (size_t i = 1; i < interface->getInterfaces().size(); ++i) {
                os << ", " << interface->getInterfaces()[i]->getTypeString();
            }
            os << " ";
        }
        os << std::hex << interface.get() << ":" << std::endl;
        for (const auto &[method, type]: interface->getMethods()) {
            os << "    +" << method << ": " << type->getTypeString() << std::endl;
        }
    }
    os << "ClassType:" << std::endl;
    for (const auto &[name, klass]: classType) {
        os << "  class " << klass->getTypeString() << " ";
        if (klass->getBaseclass()) {
            os << "(" << klass->getBaseclass()->getTypeString() << ") ";
        }
        if (!klass->getInterfaces().empty()) {
            os << ": " << klass->getInterfaces()[0];
            for (size_t i = 1; i < klass->getInterfaces().size(); ++i) {
                os << ", " << klass->getInterfaces()[i];
            }
            os << " ";
        }
        os << std::hex << klass.get() << ":" << std::endl;
        for (const auto &[field, type]: klass->getMembers()) {
            os << "    -" << field << ": " << type->getTypeString() << std::endl;
        }
        for (const auto &[method, type]: klass->getMethods()) {
            os << "    +" << method << ": " << type->getTypeString() << std::endl;
        }
    }
}

}
