//
// Created by henry on 2022-03-24.
//

#ifndef REFLEX_SRC_TYPES_TYPE_H_
#define REFLEX_SRC_TYPES_TYPE_H_

#include <string>
#include <optional>
#include <utility>
#include <vector>
#include <map>
#include <ostream>
#include <unordered_set>

namespace reflex {

class Type {
  public:
    virtual ~Type() = default;
    virtual std::string getTypeString() = 0;
    [[nodiscard]] virtual bool isBasicType() const { return false; }
};

class VoidType : public Type {
  public:
    std::string getTypeString() override;
    [[nodiscard]] bool isBasicType() const override;
};

class PrimType : public Type {
  public:
    enum BaseType {
      Integer = 0,
      Number = 1,
      Character = 2,
      Boolean = 3,
      Null = 4
    };
  public:
    explicit PrimType(BaseType baseTyp) : baseTyp(baseTyp) {}
    [[nodiscard]] BaseType getBaseTyp() const { return baseTyp; }
    std::string getTypeString() override;
    [[nodiscard]] bool isBasicType() const override;
  private:
    BaseType baseTyp;
};

class ArrayType : public Type {
    Type *elementTyp;
  public:
    explicit ArrayType(Type *elementTyp) : elementTyp(elementTyp) {}
    [[nodiscard]] Type *getElementTyp() const { return elementTyp; }
    std::string getTypeString() override;
    bool operator==(const ArrayType &rhs) const;
};

class FunctionType : public Type {
    Type *returnTyp;
    std::vector<Type *> paramTyp;
  public:
    FunctionType(Type *returnTyp, std::vector<Type *> paramTyp)
        : returnTyp(returnTyp), paramTyp(std::move(paramTyp)) {}
    [[nodiscard]] virtual bool isMemberFuncTyp() const { return false; }
    [[nodiscard]] Type *getReturnTyp() const { return returnTyp; }
    [[nodiscard]] const std::vector<Type *> &getParamTyp() const { return paramTyp; }
    std::string getTypeString() override;
    bool operator==(const FunctionType &rhs) const;
};

enum class Visibility {
  Public, Private, Protected
};
std::string getVisibilityString(Visibility visibility);
Visibility getVisibilityFromString(const std::string &ident);

class TypeError {};

class ClassType;

class MemberType : public Type {
    Visibility visibility;
    Type *memberTyp;
  public:
    MemberType(Visibility visibility, Type *memberTyp)
        : visibility(visibility), memberTyp(memberTyp) {}
    [[nodiscard]] Visibility getVisibility() const { return visibility; }

    [[nodiscard]] Type *getMemberTyp() const { return memberTyp; }

    std::string getTypeString() override;
    [[nodiscard]] bool isFunctionTyp() const { return dynamic_cast<FunctionType *>(memberTyp); }
    bool operator==(const MemberType &rhs) const;
};

class AggregateType : public Type {
    std::string name;
  public:
    explicit AggregateType(std::string name) : name(std::move(name)) {}

    virtual MemberType *findMemberTyp(const std::string &) = 0;
    [[nodiscard]] virtual std::vector<std::pair<std::string, MemberType *>> getAllInheritedMember() const = 0;

    [[nodiscard]] virtual bool isInterfaceTyp() const = 0;
    [[nodiscard]] virtual bool isClassTyp() const = 0;
    [[nodiscard]] virtual bool validate(std::vector<TypeError> &) const = 0;

    [[nodiscard]] const std::string &getName() const { return name; }
};

class InterfaceType : public AggregateType {
    std::vector<InterfaceType *> interfaces;
    std::map<std::string, MemberType *> members{};
  public:
    explicit InterfaceType(std::string interfacename, std::vector<InterfaceType *> interfaces)
        : AggregateType(std::move(interfacename)), interfaces(std::move(interfaces)) {}
    void addInterfaceMethod(const std::string &name, MemberType *typ) {
        members[name] = typ;
    }
    MemberType *findMemberTyp(const std::string &name) override;
    [[nodiscard]] std::vector<std::pair<std::string, MemberType *>> getAllInheritedMember() const override;

    void setInterfaces(const std::vector<InterfaceType *> &interfaces);

    [[nodiscard]] bool isInterfaceTyp() const override { return true; }
    [[nodiscard]] bool isClassTyp() const override { return false; }
    [[nodiscard]] bool isDerivedFrom(InterfaceType *base) const;
    [[nodiscard]] const std::vector<InterfaceType *> &getInterfaces() const;

    [[nodiscard]] const std::map<std::string, MemberType *> &getMembers() const;

    std::string getTypeString() override;
    bool operator==(const InterfaceType &rhs) const;
    bool validate(std::vector<TypeError> &vector) const override { return true; }
};

class ClassType : public AggregateType {
    ClassType *baseclass;
    std::vector<InterfaceType *> interfaces;
    std::map<std::string, MemberType *> members{};
  public:
    ClassType(std::string classname, ClassType *baseclass, std::vector<InterfaceType *> interfaces)
        : AggregateType(std::move(classname)), baseclass(baseclass), interfaces(std::move(interfaces)) {}
    void addMember(const std::string &name, MemberType *typ) {
        members[name] = typ;
    }
    MemberType *findMemberTyp(const std::string &name) override;
    [[nodiscard]] std::vector<std::pair<std::string, MemberType *>> getAllInheritedMember() const override;

    void setBaseclass(ClassType *baseclass);
    void setInterfaces(const std::vector<InterfaceType *> &interfaces);

    [[nodiscard]] bool isDerivedFrom(ClassType *base) const;
    [[nodiscard]] bool implements(InterfaceType *base) const;
    [[nodiscard]] bool isInterfaceTyp() const override { return false; }
    [[nodiscard]] bool isClassTyp() const override { return true; }
    [[nodiscard]] ClassType *getBaseclass() const;
    [[nodiscard]] const std::vector<InterfaceType *> &getInterfaces() const;
    [[nodiscard]] const std::map<std::string, MemberType *> &getMembers() const;
    std::string getTypeString() override;
    bool operator==(const ClassType &rhs) const;
    bool validate(std::vector<TypeError> &vector) const override { return true; }
};

}

#endif //REFLEX_SRC_TYPES_TYPE_H_
