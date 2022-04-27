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

#include "ASTUtils.h"

namespace reflex {

class Type {
  public:
    virtual ~Type() = default;

    virtual std::string getTypeString() const = 0;
    virtual bool isReferenceType() const = 0;
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
        : elementType(elemType), size(size) {}

    bool hasDefinedSize() const { return size.has_value(); }
    std::string getTypeString() const override;
    bool isReferenceType() const override { return false; }

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

  private:
    std::vector<Type *> paramTypes;
    Type *returnType;
};

class AggregateDecl;
class MemberAttrType;
class CompositeType : public ReferenceableType {
  public:
    CompositeType(std::string name, AggregateDecl *decl)
        : name(std::move(name)), decl(decl) {
        assert(decl && "CompileError: Cannot have null parent decl");
    };

    virtual bool isClassType() const = 0;
    const std::string &getDeclName() const { return name; }
    std::string getTypeString() const override { return name; }
    bool isReferenceType() const override { return false; }

  private:
    virtual std::vector<MemberAttrType *> getStaticAttribute(const std::string &name) const = 0;
    virtual std::vector<MemberAttrType *> getInstanceAttribute(const std::string &name) const = 0;

    AggregateDecl *decl;
    std::string name;
};

class MemberAttrType : public Type {
  public:
    MemberAttrType(Visibility visibility, CompositeType *parent, Type *type)
        : visibility(visibility), parent(parent), type(type) {
        assert(!dynamic_cast<MemberAttrType *>(type) && "CompilerError: Cannot have nested MemberAttrType");
    }

    std::string getTypeString() const override;
    bool isReferenceType() const override { return type->isReferenceType(); }

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
    const std::map<std::string, std::vector<MemberAttrType *>> &getMembers() const { return members; }

  private:
    std::vector<InterfaceType *> interfaces;
    std::map<std::string, std::vector<MemberAttrType *>> members;
};

class ClassType : public CompositeType {
  public:
    ClassType(const std::string &name, AggregateDecl *decl) : CompositeType(name, decl) {}

    ClassType *getBaseclass() const { return baseclass; }
    const std::vector<InterfaceType *> &getInterfaces() const { return interfaces; }
    const std::map<std::string, std::vector<MemberAttrType *>> &getMembers() const { return members; }

  private:
    ClassType *baseclass = nullptr;
    std::vector<InterfaceType *> interfaces;
    std::map<std::string, std::vector<MemberAttrType *>> members;
};

class ReferenceType : public Type {
  public:
    explicit ReferenceType(ReferenceableType *refType, bool nullable = false)
        : refType(refType), nullable(nullable) {
        assert(refType && "CompilerError: Cannot reference nullptr");
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
