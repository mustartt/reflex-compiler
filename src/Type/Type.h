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
    bool hasSameBaseAttr(const MemberAttrType *other) const;

  private:
    Visibility visibility;
    CompositeType *parent;
    Type *type;
};

using Method = std::pair<std::string, MemberAttrType *>;

/// Represent an Interface
/// Responsible for maintaining the interface invariants:
/// - no cyclic inheritance
/// - no method overloads
class InterfaceType : public CompositeType {
  public:
    InterfaceType(const std::string &name, AggregateDecl *decl)
        : CompositeType(name, decl) {}

    const std::vector<InterfaceType *> &getInterfaces() const { return interfaces; }
    bool isClassType() const override { return false; }

    const std::map<std::string, MemberAttrType *> &getMethods() const { return methods; }

    /// derives from @p interface and maintains invariant of no cyclic inheritance, and offers strong exception guarantee
    /// @param interface the base interface to derive from
    /// @throws TypeError if cyclic inheritance is detected
    void addInterface(InterfaceType *interface);

    /// add @p method to the interface trait and maintains invariant of no overload
    /// @param name the methodname
    /// @param method the method attribute
    /// @throws TypeError if overload present and method is a function type
    /// @note only maintains invariant of current and base interfaces
    void addMethod(const std::string &name, MemberAttrType *method);

    /// Get all traits "the methods that the interface supports" a class can implement
    /// @returns the trait of the interface
    std::vector<Method> getInterfaceTraits() const;

  private:
    /// Search parents for override attribute error
    /// @returns MemberAttrType which has the defined method, nullptr otherwise;
    MemberAttrType *hasOverrideAttrError(const std::string &name, MemberAttrType *method);

    std::vector<InterfaceType *> interfaces;
    std::map<std::string, MemberAttrType *> methods;
};

/// Represent a Class
/// Responsible for maintaining the class invariants:
/// - no cyclic inheritance
/// - no method overloads
/// - no member name conflicts
class ClassType : public CompositeType {
  public:
    ClassType(const std::string &name, AggregateDecl *decl) : CompositeType(name, decl) {}

    ClassType *getBaseclass() const { return baseclass; }
    const std::vector<InterfaceType *> &getInterfaces() const { return interfaces; }
    bool isClassType() const override { return true; }

    /// derives from @p klass and maintains invariant of no cyclic inheritance, and offers strong exception guarantee
    /// @param klass the base class to derive from
    /// @throws TypeError if cyclic inheritance is detected
    void setBaseclass(ClassType *klass);

    void addInterface(InterfaceType *interface) { interfaces.push_back(interface); }

    /// add @p field to the interface trait and maintains invariant of no overload
    /// @param name the field name
    /// @param method the field attribute
    /// @throws TypeError if name conflict or shadows baseclass fields
    /// @note only maintains invariant of current and base classes
    void addField(const std::string &name, MemberAttrType *field);

    /// add @p method to the class and maintains invariant of no overload
    /// @param name the methodname
    /// @param method the method attribute
    /// @throws TypeError if overload present and method is not a function type or name conflict
    /// @note only maintains invariant of current and base classes
    void addMethod(const std::string &name, MemberAttrType *method);

    /// check if class implements all interface traits
    bool isAbstract() const;

    const std::map<std::string, MemberAttrType *> &getMembers() const { return members; }
    const std::map<std::string, MemberAttrType *> &getMethods() const { return methods; }

    /// @note must not have cyclic inheritance
    std::vector<Method> getClassImplTraits() const;

  private:
    /// Search parents for override method attribute error
    /// @returns MemberAttrType which has the defined method, nullptr otherwise
    MemberAttrType *hasOverrideAttrError(const std::string &name, MemberAttrType *method);

    /// Search parents for shadowed fields attribute error
    /// @returns MemberAttrType which has the defined field, nullptr otherwise
    MemberAttrType *shadowsFieldAttrError(const std::string &name);

    ClassType *baseclass = nullptr;
    std::vector<InterfaceType *> interfaces;

    std::map<std::string, MemberAttrType *> members;
    std::map<std::string, MemberAttrType *> methods;
};

/// Represents a reference
/// @note If a @p refType is null, it represents a null reference
class ReferenceType : public Type {
  public:
    explicit ReferenceType(ReferenceableType *refType, bool nullable = true)
        : refType(refType), nullable(nullable) {}

    bool isNullable() const { return nullable; }
    ReferenceableType *getRefType() const { return refType; }

    std::string getTypeString() const override;
    bool isReferenceType() const override { return true; }
    bool isNullReference() const { return refType == nullptr; }

  private:
    bool nullable;
    ReferenceableType *refType;
};

}

#endif //REFLEX_SRC_TYPE_TYPE_H_
