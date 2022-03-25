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
}

void TypeContextManager::dump(std::ostream &os) {
    voidType->printType(os);
    os << std::endl;
    for (auto &p: primitiveTyp) {
        p->printType(os);
        os << std::endl;
    }
    for (auto &arr: arrayTyp) {
        arr->printType(os);
        os << std::endl;
    }
    for (auto &func: funcTyp) {
        func->printType(os);
        os << std::endl;
    }
    for (auto &[name, interface]: interfaceTyp) {
        os << name << ": ";
        interface->printType(os);
    }
    for (auto &[name, classTy]: classTyp) {
        os << name << ": ";
        classTy->printType(os);
    }
}

VoidType *TypeContextManager::getVoidTy() const { return voidType.get(); }

PrimType *TypeContextManager::getPrimitiveTy(PrimType::BaseType typ) const {
    return primitiveTyp[typ].get();
}

ArrType *TypeContextManager::getArrayTy(Type *elementTyp) {
    auto res = std::find_if(arrayTyp.begin(), arrayTyp.end(),
                            [elementTyp](const auto &existing) {
                              return existing->getElementTyp() == elementTyp;
                            });
    if (res == arrayTyp.end()) {
        arrayTyp.push_back(std::make_unique<ArrType>(elementTyp));
        return arrayTyp.back().get();
    }
    return res->get();
}

FuncType *TypeContextManager::getFunctionTy(Type *returnType, const std::vector<Type *> &paramTyp) {
    FuncType typ(returnType, paramTyp);
    auto res = std::find_if(funcTyp.begin(), funcTyp.end(),
                            [=](const auto &existing) {
                              return *existing == typ;
                            });
    if (res != funcTyp.end()) {
        return res->get();
    }
    funcTyp.push_back(std::make_unique<FuncType>(returnType, paramTyp));
    return funcTyp.back().get();
}

MemberType *TypeContextManager::createMemberTy(ClassType *instance, Visibility visibility, Type *type) {
    MemberType typ(instance, visibility, type);
    auto res = std::find_if(memberTyp.begin(), memberTyp.end(),
                            [=](const auto &existing) {
                              return *existing == typ;
                            });
    if (res != memberTyp.end()) {
        return res->get();
    }
    memberTyp.push_back(std::make_unique<MemberType>(instance, visibility, type));
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
    classTyp.emplace(name, std::make_unique<ClassType>(baseclass, interfaces));
    return classTyp[name].get();
}

InterfaceType *TypeContextManager::createOrGetInterfaceTy(const std::string &name,
                                                          const std::vector<InterfaceType *> &interfaces) {
    interfaceTyp.emplace(name, std::make_unique<InterfaceType>(interfaces));
    return interfaceTyp[name].get();
}

}
