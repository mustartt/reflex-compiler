//
// Created by henry on 2022-03-24.
//

#include <iostream>
#include <sstream>
#include "Type.h"
#include "LLVMTypeGenerator.h"
#include "llvm/Support/Casting.h"

namespace reflex {

std::string getVisibilityString(Visibility visibility) {
    switch (visibility) {
        case Visibility::Public: return "public";
        case Visibility::Private: return "private";
        case Visibility::Protected: return "protected";
    }
}

Visibility getVisibilityFromString(const std::string &ident) {
    if (ident == "public") return Visibility::Public;
    if (ident == "private") return Visibility::Private;
    if (ident == "protected") return Visibility::Protected;
    throw std::runtime_error("Unknown access modifier " + ident);
}

std::string VoidType::getTypeString() {
    return "void";
}
bool VoidType::isBasicType() const {
    return true;
}

llvm::Type *VoidType::createLLVMType(LLVMTypeGenerator &TG) {
    return TG.getVoidType();
}

std::string PrimType::getTypeString() {
    switch (baseTyp) {
        case Integer: return "int";
        case Number: return "num";
        case Character: return "char";
        case Boolean: return "bool";
        case Null: return "null";
    }
}

bool PrimType::isBasicType() const {
    return true;
}

llvm::Type *PrimType::createLLVMType(LLVMTypeGenerator &TG) {
    return TG.getLLVMBasicType(this);
}

bool ArrayType::operator==(const ArrayType &rhs) const {
    return elementTyp == rhs.elementTyp;
}

std::string ArrayType::getTypeString() {
    return elementTyp->getTypeString() + " []";
}

llvm::Type *ArrayType::createLLVMType(LLVMTypeGenerator &TG) {
    return TG.getArrayType(elementTyp->createLLVMType(TG));
}

std::string FunctionType::getTypeString() {
    std::stringstream ss;
    ss << "(";
    if (!paramTyp.empty()) {
        ss << paramTyp[0]->getTypeString();
        for (size_t i = 1; i < paramTyp.size(); ++i) {
            ss << "," << paramTyp[i]->getTypeString();
        }
    }
    ss << ")->" << returnTyp->getTypeString();
    return ss.str();
}

bool FunctionType::operator==(const FunctionType &rhs) const {
    return returnTyp == rhs.returnTyp
        && paramTyp == rhs.paramTyp;
}

llvm::Type *FunctionType::createLLVMType(LLVMTypeGenerator &TG) {
    std::vector<llvm::Type *> paramTypes;
    for (auto param: paramTyp) {
        paramTypes.push_back(param->createLLVMType(TG));
    }
    return LLVMTypeGenerator::getFunctionType(paramTypes, returnTyp->createLLVMType(TG));
}

std::string MemberType::getTypeString() {
    return getVisibilityString(visibility) + " "
        + memberTyp->getTypeString();
}

bool MemberType::operator==(const MemberType &rhs) const {
    return visibility == rhs.visibility
        && memberTyp == rhs.memberTyp
        && parent == rhs.parent;
}

llvm::Type *MemberType::createLLVMType(LLVMTypeGenerator &TG) {
    auto method = dynamic_cast<FunctionType *>(memberTyp);
    if (!method) return memberTyp->createLLVMType(TG);
    std::vector<llvm::Type *> paramTypes{LLVMTypeGenerator::getPointerType(parent->createLLVMType(TG))};
    std::transform(method->getParamTyp().begin(), method->getParamTyp().end(),
                   paramTypes.end(), [&TG](Type *type) {
          return type->createLLVMType(TG);
        });
    return LLVMTypeGenerator::getFunctionType(
        paramTypes,
        method->getReturnTyp()->createLLVMType(TG)
    );
}

std::string InterfaceType::getTypeString() {
    return "interface " + getName();
}

bool InterfaceType::operator==(const InterfaceType &rhs) const {
    return getName() == rhs.getName()
        && interfaces == rhs.interfaces
        && members == rhs.members;
}

std::string ClassType::getTypeString() {
    return "class " + getName();
}

bool ClassType::operator==(const ClassType &rhs) const {
    return getName() == rhs.getName()
        && baseclass == rhs.baseclass
        && interfaces == rhs.interfaces
        && members == rhs.members;
}

bool InterfaceType::isDerivedFrom(InterfaceType *base) const {
    std::vector<const InterfaceType *> curr;
    curr.push_back(this);
    while (!curr.empty()) {
        std::vector<const InterfaceType *> next;
        if (std::find(curr.begin(), curr.end(), base) != curr.end())
            return true;
        for (const auto i: curr) {
            next.insert(next.end(), i->interfaces.begin(), i->interfaces.end());
        }
        curr = std::move(next);
    }
    return false;
}

const std::vector<InterfaceType *> &InterfaceType::getInterfaces() const { return interfaces; }
const std::map<std::string, MemberType *> &InterfaceType::getMembers() const { return members; }
void InterfaceType::setInterfaces(const std::vector<InterfaceType *> &interfaces) {
    InterfaceType::interfaces = interfaces;
}

std::vector<std::pair<std::string, MemberType *>> InterfaceType::getAllInheritedMember() const {
    std::vector<std::pair<std::string, MemberType *>> inheritedMembers;
    for (auto interface: interfaces) {
        auto inherited = interface->getAllInheritedMember();
        for (auto &member: inherited) {
            inheritedMembers.emplace_back(member);
        }
    }
    for (auto &member: members) {
        inheritedMembers.emplace_back(member);
    }
    return inheritedMembers;
}

MemberType *InterfaceType::findMemberTyp(const std::string &name) {
    if (members.count(name)) return members.at(name);
    for (auto interface: interfaces) {
        auto result = interface->findMemberTyp(name);
        if (result) return result;
    }
    return nullptr;
}

llvm::Type *InterfaceType::createLLVMType(LLVMTypeGenerator &TG) {
    return TG.getInterfaceType(this);
}

ClassType *ClassType::getBaseclass() const { return baseclass; }
const std::vector<InterfaceType *> &ClassType::getInterfaces() const { return interfaces; }
const std::map<std::string, MemberType *> &ClassType::getMembers() const { return members; }

bool ClassType::isDerivedFrom(ClassType *base) const {
    auto curr = this;
    while (curr) {
        if (curr == base) return true;
        curr = baseclass;
    }
    return false;
}

bool ClassType::implements(InterfaceType *base) const {
    auto curr = this;
    while (curr) {
        for (auto interface: interfaces) {
            if (interface == base) return true;
        }
        curr = baseclass;
    }
    return false;
}

void ClassType::setBaseclass(ClassType *baseclass) {
    ClassType::baseclass = baseclass;
}

void ClassType::setInterfaces(const std::vector<InterfaceType *> &interfaces) {
    ClassType::interfaces = interfaces;
}
MemberType *ClassType::findMemberTyp(const std::string &name) {
    if (members.count(name)) return members.at(name);
    if (baseclass) {
        auto result = baseclass->findMemberTyp(name);
        if (result) return result;
    }
    for (auto interface: interfaces) {
        auto result = interface->findMemberTyp(name);
        if (result) return result;
    }
    return nullptr;
}

std::vector<std::pair<std::string, MemberType *>> ClassType::getAllInheritedMember() const {
    std::vector<std::pair<std::string, MemberType *>> inheritedMembers;
    if (baseclass) {
        auto inherited = baseclass->getAllInheritedMember();
        for (auto &member: inherited) {
            inheritedMembers.emplace_back(member);
        }
    }
    for (auto &member: members) {
        inheritedMembers.emplace_back(member);
    }
    return inheritedMembers;
}

llvm::Type *ClassType::createLLVMType(LLVMTypeGenerator &TG) {
    auto type = TG.getClassType(this);
    if (type) return type.value();

    TG.predeclareClassType(this);
    std::vector<llvm::Type *> dataLayout;
    std::vector<llvm::FunctionType *> methodLayout;
    auto layout = getAllInheritedMember();
    for (auto &[name, member]: layout) {
        if (!dynamic_cast<FunctionType *>(member->getMemberTyp())) {
            dataLayout.push_back(member->createLLVMType(TG));
        } else {
            llvm::Type *memberMethodType = member->createLLVMType(TG);
            methodLayout.push_back(llvm::dyn_cast<llvm::FunctionType>(memberMethodType));
        }
    }
    auto vtabLayoutType = TG.getClassVtabLayoutType(this, methodLayout);
    return TG.getClassType(this, vtabLayoutType, dataLayout);
}

}
