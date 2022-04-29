//
// Created by henry on 2022-04-26.
//

#ifndef REFLEX_SRC_TYPE_TYPE_H_
#define REFLEX_SRC_TYPE_TYPE_H_

#include <string>
#include <vector>
#include <optional>
#include <cassert>
#include <map>
#include <stdexcept>

#include "ASTUtils.h"

namespace reflex {

class Type {
  public:
    virtual ~Type() = default;

    virtual std::string getTypeString() const = 0;
    virtual bool isReferenceType() const = 0;
};

class TypeError : public std::runtime_error {
  public:
    explicit TypeError(const std::string &arg);
};

class VoidType : public Type {
  public:
    VoidType() = default;

    std::string getTypeString() const override { return "void"; }
    bool isReferenceType() const override { return false; }
};

class BuiltinType : public Type {
  public:
    enum BaseType {
      Integer = 0,
      Number = 1,
      Character = 2,
      Boolean = 3,
    };
  public:
    explicit BuiltinType(BaseType baseType) : baseType(baseType) {}

    std::string getTypeString() const override;
    bool isReferenceType() const override { return false; }

  private:
    BaseType baseType;
};

class ReferenceableType : public Type {};

class ArrayType : public ReferenceableType {
  public:
    explicit ArrayType(Type *elemType, std::optional<size_t> size = std::nullopt)
        : elementType(elemType), size(size) {
        if (dynamic_cast<VoidType *>(elemType))
            throw TypeError{"ArrayType cannot have void type"};
    }

    bool hasDefinedSize() const { return size.has_value(); }
    std::string getTypeString() const override;
    bool isReferenceType() const override { return false; }

    const std::optional<size_t> &getSize() const { return size; }
    Type *getElementType() const { return elementType; }

  private:
    std::optional<size_t> size;
    Type *elementType;
};

class FunctionType : public ReferenceableType {
  public:
    FunctionType(std::vector<Type *> paramTypes, Type *returnType)
        : paramTypes(std::move(paramTypes)), returnType(returnType) {}

    std::string getTypeString() const override;
    bool isReferenceType() const override { return false; }
    bool isReturnVoid() const { return dynamic_cast<VoidType *>(returnType); }

    const std::vector<Type *> &getParamTypes() const { return paramTypes; }
    Type *getReturnType() const { return returnType; }
    bool canApplyArguments(const std::vector<Type *> &args) const { return paramTypes == args; }

  private:
    std::vector<Type *> paramTypes;
    Type *returnType;
};

class AggregateDecl;
class MemberAttrType;
class CompositeType : public ReferenceableType {
  public:
    CompositeType(std::string name, AggregateDecl *decl)
        : name(std::move(name)), decl(decl) {};

    virtual bool isClassType() const = 0;
    const std::string &getDeclName() const { return name; }
    std::string getTypeString() const override { return name; }
    bool isReferenceType() const override { return false; }

    AggregateDecl *getDecl() const { return decl; }
    void setDecl(AggregateDecl *aggregateDecl) { CompositeType::decl = aggregateDecl; }

  private:
//    virtual std::vector<MemberAttrType *> getStaticAttribute(const std::string &name) const = 0;
//    virtual std::vector<MemberAttrType *> getInstanceAttribute(const std::string &name) const = 0;

    AggregateDecl *decl;
    std::string name;
};

class MemberAttrType : public Type {
  public:
    MemberAttrType(Visibility visibility, CompositeType *parent, Type *type)
        : visibility(visibility), parent(parent), type(type) {
        if (dynamic_cast<MemberAttrType *>(type))
            throw TypeError{"MemberAttrType cannot have nested MemberAttrType"};
        if (!parent) throw TypeError{"MemberAttrType must have parent CompositeType"};
        if (dynamic_cast<VoidType *>(type)) throw TypeError{"MemberAttrType cannot have void type"};
    }

    std::string getTypeString() const override;
    bool isReferenceType() const override { return type->isReferenceType(); }
    Type *getType() const { return type; }

    Visibility getVisibility() const { return visibility; }
    CompositeType *getParent() const { return parent; }
    Type *getMemberAttrType() const { return type; }

  private:
    Visibility visibility;
    CompositeType *parent;
    Type *type;
};

class InterfaceType : public CompositeType {
  public:
    InterfaceType(const std::string &name, AggregateDecl *decl) : CompositeType(name, decl) {}

    const std::vector<InterfaceType *> &getInterfaces() const { return interfaces; }
    bool isClassType() const override { return false; }

    void addInterface(InterfaceType *interface) { interfaces.push_back(interface); }

  private:
    std::vector<InterfaceType *> interfaces;
    std::map<std::string, std::vector<MemberAttrType *>> methods;
};

class ClassType : public CompositeType {
  public:
    ClassType(const std::string &name, AggregateDecl *decl) : CompositeType(name, decl) {}

    ClassType *getBaseclass() const { return baseclass; }
    const std::vector<InterfaceType *> &getInterfaces() const { return interfaces; }
    bool isClassType() const override { return true; }

    void setBaseclass(ClassType *klass) { ClassType::baseclass = klass; }
    void addInterface(InterfaceType *interface) { interfaces.push_back(interface); }

  private:
    ClassType *baseclass = nullptr;
    std::vector<InterfaceType *> interfaces;

    std::map<std::string, MemberAttrType *> members;
    std::map<std::string, std::vector<MemberAttrType *>> methods;
};

class ReferenceType : public Type {
  public:
    explicit ReferenceType(ReferenceableType *refType, bool nullable = true)
        : refType(refType), nullable(nullable) {
        if (!refType) throw TypeError{"ReferenceType cannot reference nullptr"};

    }

    bool isNullable() const { return nullable; }
    ReferenceableType *getRefType() const { return refType; }

    std::string getTypeString() const override;
    bool isReferenceType() const override { return true; }

  private:
    bool nullable;
    ReferenceableType *refType;
};

}

#endif //REFLEX_SRC_TYPE_TYPE_H_
