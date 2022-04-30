//
// Created by henry on 2022-04-26.
//

#include "Type.h"

#include <unordered_map>

namespace reflex {

TypeError::TypeError(const std::string &arg)
    : runtime_error("TypeError: " + arg) {}

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

bool hasCyclicReference(std::unordered_map<InterfaceType *, bool> &visited,
                        InterfaceType *interface) {
    if (visited[interface]) return true;
    visited[interface] = true;
    for (auto derived: interface->getInterfaces()) {
        if (hasCyclicReference(visited, derived)) return true;
    }
    visited[interface] = false;
    return false;
}

/// @returns if a cycle is detected or not
bool detectCycle(InterfaceType *interface, InterfaceType *toBeDerived) {
    std::unordered_map<InterfaceType *, bool> visited;
    visited[interface] = true;
    return hasCyclicReference(visited, toBeDerived);
}

void InterfaceType::addInterface(InterfaceType *interface) {
    if (detectCycle(this, interface)) {
        throw TypeError{interface->getTypeString() + " is part of cyclic inheritance with " + getTypeString()};
    }
    interfaces.push_back(interface);
}

void InterfaceType::addMethod(const std::string &name, MemberAttrType *method) {
    auto funcType = dynamic_cast<FunctionType *>(method->getMemberAttrType());
    if (!funcType) throw TypeError{name + " must be a function"};
    if (methods.contains(name)) throw TypeError{"Cannot overload " + name + ", it already exists"};
    methods[name] = method;
}

void addInheritedTrait(std::vector<Method> &methods, const std::string &name, MemberAttrType *type) {
    auto res = std::find_if(methods.begin(), methods.end(),
                            [&](const auto &method) -> bool {
                              return method.first == name;
                            });
    if (res == methods.end()) {
        methods.emplace_back(name, type);
    }
}

std::vector<Method> InterfaceType::getInterfaceTraits() const {
    std::vector<Method> members;
    for (auto interface: interfaces) {
        const auto inherited = interface->getInterfaceTraits();
        for (const auto &[name, type]: inherited) {
            addInheritedTrait(members, name, type);
        }
    }
    for (const auto &[name, type]: methods) {
        addInheritedTrait(members, name, type);
    }
    return members;
}

std::vector<Method> ClassType::getClassImplTraits() const {
    std::vector<Method> traits;
    if (baseclass) {
        const auto inherited = baseclass->getClassImplTraits();
        for (const auto &[name, type]: inherited) {
            addInheritedTrait(traits, name, type);
        }
    }
    for (const auto &[name, type]: methods) {
        addInheritedTrait(traits, name, type);
    }
    return traits;
}

void ClassType::addMethod(const std::string &name, MemberAttrType *method) {
    if (!dynamic_cast<FunctionType *>(method->getType()))
        throw TypeError{name + " must be of FunctionType"};
    if (methods.contains(name))
        throw TypeError{"Method already exists in interface " + name};
    methods[name] = method;
}

void ClassType::addField(const std::string &name, MemberAttrType *method) {
    if (!(dynamic_cast<ReferenceType *>(method->getType()) ||
        dynamic_cast<BuiltinType *>(method->getType())))
        throw TypeError{name + " must be of ReferenceType or BuiltinType"};
    if (members.contains(name))
        throw TypeError{"Method already exists in class " + name};
    members[name] = method;
}

}

