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

bool MemberAttrType::hasSameBaseAttr(const MemberAttrType *other) const {
    return other->visibility == visibility
        && other->type == type;
}

std::string ReferenceType::getTypeString() const {
    return "&" + refType->getTypeString() + (nullable ? "?" : "");
}

bool hasCyclicInterfaceReference(std::unordered_map<InterfaceType *, bool> &visited,
                                 InterfaceType *interface) {
    if (visited[interface]) return true;
    visited[interface] = true;
    for (auto derived: interface->getInterfaces()) {
        if (hasCyclicInterfaceReference(visited, derived)) return true;
    }
    visited[interface] = false;
    return false;
}

/// @returns if a cycle is detected or not
bool detectDerivedInterfaceCycle(InterfaceType *interface, InterfaceType *toBeDerived) {
    std::unordered_map<InterfaceType *, bool> visited;
    visited[interface] = true;
    return hasCyclicInterfaceReference(visited, toBeDerived);
}

MemberAttrType *InterfaceType::hasOverrideAttrError(const std::string &name, MemberAttrType *method) {
    if (methods.contains(name) &&
        !methods.at(name)->hasSameBaseAttr(method)) {
        return methods.at(name);
    }
    for (auto interface: interfaces) {
        if (auto parent = interface->hasOverrideAttrError(name, method)) {
            return parent;
        }
    }
    return nullptr;
}

void InterfaceType::addInterface(InterfaceType *interface) {
    if (detectDerivedInterfaceCycle(this, interface)) {
        throw TypeError{interface->getTypeString() + " is part of cyclic inheritance with " + getTypeString()};
    }
    interfaces.push_back(interface);
}

void InterfaceType::addMethod(const std::string &name, MemberAttrType *method) {
    auto funcType = dynamic_cast<FunctionType *>(method->getMemberAttrType());
    if (!funcType) throw TypeError{name + " must be a function"};
    if (methods.contains(name)) throw TypeError{"Cannot overload " + name + ", it already exists"};
    if (auto parent = hasOverrideAttrError(name, method)) {
        throw TypeError{
            name + " MemberAttrType mismatch " +
                method->getTypeString() + " expected " + parent->getTypeString()};
    }
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

bool hasCyclicClassReference(std::unordered_map<ClassType *, bool> &visited,
                             ClassType *klass) {
    if (visited[klass]) return true;
    visited[klass] = true;
    if (klass->getBaseclass()) {
        if (hasCyclicClassReference(visited, klass->getBaseclass())) return true;
    }
    visited[klass] = false;
    return false;
}

/// @returns if a cycle is detected or not
bool detectDerivedClassCycle(ClassType *klass, ClassType *toBeDerived) {
    std::unordered_map<ClassType *, bool> visited;
    visited[klass] = true;
    return hasCyclicClassReference(visited, toBeDerived);
}

void ClassType::setBaseclass(ClassType *klass) {
    if (detectDerivedClassCycle(this, klass)) {
        throw TypeError{klass->getTypeString() + " is part of cyclic inheritance with " + getTypeString()};
    }
    baseclass = klass;
}

MemberAttrType *ClassType::hasOverrideAttrError(const std::string &name, MemberAttrType *method) {
    if (methods.contains(name) &&
        !methods.at(name)->hasSameBaseAttr(method)) {
        return methods.at(name);
    }
    if (baseclass) {
        if (auto parent = baseclass->hasOverrideAttrError(name, method)) {
            return parent;
        }
    }
    return nullptr;
}

MemberAttrType *ClassType::shadowsFieldAttrError(const std::string &name) {
    if (members.contains(name)) return members[name];
    if (baseclass) return baseclass->shadowsFieldAttrError(name);
    return nullptr;
}

void ClassType::addField(const std::string &name, MemberAttrType *field) {
    auto refType = dynamic_cast<ReferenceType *>(field->getMemberAttrType());
    auto builtinType = dynamic_cast<BuiltinType *>(field->getMemberAttrType());
    if (!refType && !builtinType) {
        throw TypeError{name + " must be either ReferenceType or BuiltinType"};
    }
    if (auto parent = shadowsFieldAttrError(name)) {
        throw TypeError{
            "field " + name + " cannot be shadowed in baseclass " +
                parent->getParent()->getTypeString()
        };
    }
    if (auto parent = hasOverrideAttrError(name, field)) {
        throw TypeError{
            "Name conflict with " + name + " in " +
                parent->getParent()->getTypeString()
        };
    }
    members[name] = field;
}

void ClassType::addMethod(const std::string &name, MemberAttrType *method) {
    auto funcType = dynamic_cast<FunctionType *>(method->getMemberAttrType());
    if (!funcType) throw TypeError{name + " must be a function"};
    if (methods.contains(name)) throw TypeError{"Cannot overload " + name + ", it already exists"};
    if (auto field = shadowsFieldAttrError(name)) {
        throw TypeError{
            "Shadows member field " + name + " in base class " +
                field->getParent()->getTypeString()
        };
    }
    if (auto parent = hasOverrideAttrError(name, method)) {
        throw TypeError{
            name + " MemberAttrType mismatch " +
                method->getTypeString() + " expected " + parent->getTypeString()};
    }
    methods[name] = method;
}

bool ClassType::isAbstract() const {
    std::vector<Method> mustImpl;
    for (auto interface: interfaces) {
        auto inherited = interface->getInterfaceTraits();
        for (auto &[name, type]: inherited) {
            addInheritedTrait(mustImpl, name, type);
        }
    }
    auto implemented = getClassImplTraits();
    for (auto &trait: mustImpl) {
        auto iter = std::find_if(implemented.begin(), implemented.end(),
                                 [&trait](const Method &item) -> bool {
                                   return item.first == trait.first
                                       && item.second->hasSameBaseAttr(trait.second);
                                 });
        if (iter == implemented.end()) return true;
    }
    return false;
}

}

