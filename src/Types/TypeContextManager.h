//
// Created by henry on 2022-03-24.
//

#ifndef REFLEX_SRC_TYPES_TYPECONTEXTMANAGER_H_
#define REFLEX_SRC_TYPES_TYPECONTEXTMANAGER_H_

#include "Type.h"

#include <unordered_map>
#include <memory>
#include <vector>

namespace reflex {

class TypeContextManager {
    std::unique_ptr<VoidType> voidType = std::make_unique<VoidType>();
    std::vector<std::unique_ptr<PrimType>> primitiveTyp;
    std::vector<std::unique_ptr<ArrType>> arrayTyp;
    std::vector<std::unique_ptr<FuncType>> funcTyp;
    std::vector<std::unique_ptr<MemberType>> memberTyp;
    std::unordered_map<std::string, std::unique_ptr<ClassType>> classTyp;
    std::unordered_map<std::string, std::unique_ptr<InterfaceType>> interfaceTyp;
  public:
    TypeContextManager() {
        primitiveTyp.push_back(std::make_unique<PrimType>(PrimType::Integer));
        primitiveTyp.push_back(std::make_unique<PrimType>(PrimType::Number));
        primitiveTyp.push_back(std::make_unique<PrimType>(PrimType::Character));
        primitiveTyp.push_back(std::make_unique<PrimType>(PrimType::Boolean));
    };

    [[nodiscard]] VoidType *getVoidTy() const { return voidType.get(); }
    [[nodiscard]] PrimType *getPrimitiveTy(PrimType::BaseType typ) const {
        return primitiveTyp[typ].get();
    }
    [[nodiscard]] ArrType *getArrayTy(Type *elementTyp) {
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
    [[nodiscard]] FuncType *getFunctionTy(Type *returnType, const std::vector<Type *> &paramTyp) {
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

    [[nodiscard]] MemberType *createMemberTy(ClassType *instance, Visibility visibility, Type *type) {
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

    [[nodiscard]] std::optional<ClassType *> getClassTyp(const std::string &name) {
        if (classTyp.count(name)) return {classTyp[name].get()};
        return std::nullopt;
    }

    [[nodiscard]] std::optional<InterfaceType *> getInterfaceTyp(const std::string &name) {
        if (interfaceTyp.count(name)) return {interfaceTyp[name].get()};
        return std::nullopt;
    }

    [[nodiscard]] ClassType *createOrGetClassTy(const std::string &name,
                                                ClassType *baseclass,
                                                const std::vector<InterfaceType *> &interfaces) {
        classTyp.emplace(name, std::make_unique<ClassType>(baseclass, interfaces));
        return classTyp[name].get();
    }

    [[nodiscard]] InterfaceType *createOrGetInterfaceTy(const std::string &name,
                                                        const std::vector<InterfaceType *> &interfaces) {
        interfaceTyp.emplace(name, std::make_unique<InterfaceType>(interfaces));
        return interfaceTyp[name].get();
    }

    void dump(std::ostream &os) {
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
};

}

#endif //REFLEX_SRC_TYPES_TYPECONTEXTMANAGER_H_
