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
    std::vector<std::unique_ptr<ArrayType>> arrayTyp;
    std::vector<std::unique_ptr<FunctionType>> funcTyp;
    std::vector<std::unique_ptr<MemberType>> memberTyp;
    std::unordered_map<std::string, std::unique_ptr<ClassType>> classTyp;
    std::unordered_map<std::string, std::unique_ptr<InterfaceType>> interfaceTyp;
  public:
    TypeContextManager();;

    [[nodiscard]] VoidType *getVoidTy() const;
    [[nodiscard]] PrimType *getPrimitiveTy(PrimType::BaseType typ) const;
    [[nodiscard]] ArrayType *getArrayTy(Type *elementTyp);
    [[nodiscard]] FunctionType *getFunctionTy(Type *returnType, const std::vector<Type *> &paramTyp);

    [[nodiscard]] MemberType *createOrGetMemberTy(Visibility visibility, Type *type);

    [[nodiscard]] std::optional<ClassType *> getClassTyp(const std::string &name);
    [[nodiscard]] std::optional<InterfaceType *> getInterfaceTyp(const std::string &name);

    [[nodiscard]] ClassType *createOrGetClassTy(const std::string &name,
                                                ClassType *baseclass,
                                                const std::vector<InterfaceType *> &interfaces);
    [[nodiscard]] InterfaceType *createOrGetInterfaceTy(const std::string &name,
                                                        const std::vector<InterfaceType *> &interfaces);

    void dump(std::ostream &os);
};

}

#endif //REFLEX_SRC_TYPES_TYPECONTEXTMANAGER_H_
