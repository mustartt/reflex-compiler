//
// Created by henry on 2022-03-24.
//

#include "TypeContextManager.h"

namespace reflex {

TypeContextManager::TypeContextManager() {
    primitiveTyp.push_back(std::make_unique<PrimType>(PrimType::Integer));
    primitiveTyp.push_back(std::make_unique<PrimType>(PrimType::Number));
    primitiveTyp.push_back(std::make_unique<PrimType>(PrimType::Character));
    primitiveTyp.push_back(std::make_unique<PrimType>(PrimType::Boolean));
    primitiveTyp.push_back(std::make_unique<PrimType>(PrimType::Null));
}

VoidType *TypeContextManager::getVoidTy() const { return voidType.get(); }

PrimType *TypeContextManager::getPrimitiveTy(PrimType::BaseType typ) const {
    return primitiveTyp[typ].get();
}

ArrayType *TypeContextManager::getArrayTy(Type *elementTyp) {
    auto res = std::find_if(arrayTyp.begin(), arrayTyp.end(),
                            [elementTyp](const auto &existing) {
                              return existing->getElementTyp() == elementTyp;
                            });
    if (res == arrayTyp.end()) {
        arrayTyp.push_back(std::make_unique<ArrayType>(elementTyp));
        return arrayTyp.back().get();
    }
    return res->get();
}

FunctionType *TypeContextManager::getFunctionTy(Type *returnType, const std::vector<Type *> &paramTyp) {
    FunctionType typ(returnType, paramTyp);
    auto res = std::find_if(funcTyp.begin(), funcTyp.end(),
                            [=](const auto &existing) {
                              return *existing == typ;
                            });
    if (res != funcTyp.end()) {
        return res->get();
    }
    funcTyp.push_back(std::make_unique<FunctionType>(returnType, paramTyp));
    return funcTyp.back().get();
}

MemberType *TypeContextManager::createMemberTy(Visibility visibility, Type *type) {
    MemberType typ(visibility, type);
    auto res = std::find_if(memberTyp.begin(), memberTyp.end(),
                            [=](const auto &existing) {
                              return *existing == typ;
                            });
    if (res != memberTyp.end()) {
        return res->get();
    }
    memberTyp.push_back(std::make_unique<MemberType>(visibility, type));
    return memberTyp.back().get();
}

std::optional<ClassType *> TypeContextManager::getClassTyp(const std::string &name) {
    if (classTyp.count(name)) return {classTyp[name].get()};
    return std::nullopt;
}

std::optional<InterfaceType *> TypeContextManager::getInterfaceTyp(const std::string &name) {
    if (interfaceTyp.count(name)) return {interfaceTyp[name].get()};
    return std::nullopt;
}

ClassType *TypeContextManager::createOrGetClassTy(const std::string &name,
                                                  ClassType *baseclass,
                                                  const std::vector<InterfaceType *> &interfaces) {
    classTyp.emplace(name, std::make_unique<ClassType>(name, baseclass, interfaces));
    return classTyp[name].get();
}

InterfaceType *TypeContextManager::createOrGetInterfaceTy(const std::string &name,
                                                          const std::vector<InterfaceType *> &interfaces) {
    interfaceTyp.emplace(name, std::make_unique<InterfaceType>(name, interfaces));
    return interfaceTyp[name].get();
}

void TypeContextManager::dump(std::ostream &os) {
    for (auto &func: funcTyp) {
        os << func->getTypeString() << std::endl;
    }
    for (auto &[name, interface]: interfaceTyp) {
        os << interface->getTypeString();
        if (!interface->getInterfaces().empty()) {
            os << " :";
            for (auto derived: interface->getInterfaces()) {
                os << " " << derived->getName();
            }
        }
        os << " {";
        if (!interface->getMembers().empty()) os << std::endl;
        for (auto &[member, type]: interface->getMembers()) {
            os << "  " << member << ": " << type->getTypeString() << std::endl;
        }
        os << "}" << std::endl;
    }
    for (auto &[name, classTy]: classTyp) {
        os << classTy->getTypeString();
        if (classTy->getBaseclass()) {
            os << " (" << classTy->getBaseclass()->getTypeString() << ")";
        }
        if (!classTy->getInterfaces().empty()) {
            os << " :";
            for (auto derived: classTy->getInterfaces()) {
                os << " " << derived->getName();
            }
        }
        os << " {";
        if (!classTy->getMembers().empty()) os << std::endl;
        for (auto &[member, type]: classTy->getMembers()) {
            os << "  " << member << ": " << type->getTypeString() << std::endl;
        }
        os << "}" << std::endl;
    }
}

}
