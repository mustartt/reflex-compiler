//
// Created by henry on 2022-04-27.
//

#ifndef REFLEX_SRC_TYPE_TYPECONTEXT_H_
#define REFLEX_SRC_TYPE_TYPECONTEXT_H_

#include "Type.h"

#include <string>
#include <unordered_map>
#include <memory>

namespace reflex {

class TypeContext {
  public:
    TypeContext();

    VoidType *getVoidType() const;
    BuiltinType *getBuiltinType(BuiltinType::BaseType typ) const;
    ArrayType *getArrayType(Type *elementTyp, std::optional<size_t> size = std::nullopt);
    FunctionType *getFunctionType(Type *returnType, const std::vector<Type *> &paramTyp);
    MemberAttrType *getMemberType(Visibility visibility, CompositeType *parent, Type *type);
    ClassType *getClassType(const std::string &name, AggregateDecl *decl = nullptr);
    InterfaceType *getInterfaceType(const std::string &name, AggregateDecl *decl = nullptr);
    ReferenceType *getReferenceType(ReferenceableType *type, bool nullable = true);

    void dump(std::ostream &os);

  private:
    std::unique_ptr<VoidType> voidType;
    std::vector<std::unique_ptr<BuiltinType>> builtinType;
    std::vector<std::unique_ptr<ArrayType>> arrayType;
    std::vector<std::unique_ptr<FunctionType>> funcType;
    std::vector<std::unique_ptr<MemberAttrType>> memberType;
    std::unordered_map<std::string, std::unique_ptr<ClassType>> classType;
    std::unordered_map<std::string, std::unique_ptr<InterfaceType>> interfaceType;
    std::vector<std::unique_ptr<ReferenceType>> referenceType;
};

}

#endif //REFLEX_SRC_TYPE_TYPECONTEXT_H_
