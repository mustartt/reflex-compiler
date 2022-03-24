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

namespace reflex {

class Type {
  public:
    virtual ~Type() = default;
    virtual void printType(std::ostream &os) = 0;
};

class VoidType : public Type {
  public:
    void printType(std::ostream &os) override { os << "void"; };
};

class PrimType : public Type {
  public:
    enum BaseType {
      Integer = 0,
      Number = 1,
      Character = 2,
      Boolean = 3
    };
  public:
    explicit PrimType(BaseType baseTyp) : baseTyp(baseTyp) {}
    [[nodiscard]]
    BaseType getBaseTyp() const { return baseTyp; }
    void printType(std::ostream &os) override {
        switch (baseTyp) {
            case Integer: os << "int";
                break;
            case Number:os << "num";
                break;
            case Character:os << "char";
                break;
            case Boolean:os << "bool";
                break;
        }
    }
  private:
    BaseType baseTyp;
};

class ArrType : public Type {
    Type *elementTyp;
  public:
    explicit ArrType(Type *elementTyp) : elementTyp(elementTyp) {}
    [[nodiscard]] Type *getElementTyp() const { return elementTyp; }
    void printType(std::ostream &os) override {
        os << "ArrayType: ";
        elementTyp->printType(os);
        os << "[]";
    }
    bool operator==(const ArrType &rhs) const {
        return elementTyp == rhs.elementTyp;
    }
};

class FuncType : public Type {
    Type *returnTyp;
    std::vector<Type *> paramTyp;
  public:
    FuncType(Type *returnTyp, std::vector<Type *> paramTyp)
        : returnTyp(returnTyp), paramTyp(std::move(paramTyp)) {}
    [[nodiscard]] virtual bool isMemberFuncTyp() const { return false; }
    [[nodiscard]] Type *getReturnTyp() const { return returnTyp; }
    [[nodiscard]] const std::vector<Type *> &getParamTyp() const { return paramTyp; }
    void printType(std::ostream &os) override {
        os << "FunctionType: ";
        returnTyp->printType(os);
        os << "(";
        if (!paramTyp.empty()) {
            paramTyp[0]->printType(os);
            for (size_t i = 1; i < paramTyp.size(); ++i) {
                os << ",";
                paramTyp[i]->printType(os);
            }
        }
        os << ")";
    }
    bool operator==(const FuncType &rhs) const {
        return returnTyp == rhs.returnTyp
            && paramTyp == rhs.paramTyp;
    }
};

enum class Visibility {
  Public, Private, Protected
};

std::string getVisibilityString(Visibility visibility) {
    switch (visibility) {
        case Visibility::Public: return "public";
        case Visibility::Private: return "private";
        case Visibility::Protected: return "protected";
    }
}

class MemberType;
class AggregateType : public Type {
  public:
    virtual std::vector<MemberType *> findMemberTyp(const std::string &) = 0;
    [[nodiscard]] virtual bool isInterfaceTyp() const = 0;
    [[nodiscard]] virtual bool isClassTyp() const = 0;
};

class ClassType;
class MemberType : public Type {
    ClassType *instanceTyp;
    Visibility visibility;
    Type *memberTyp;
  public:
    MemberType(ClassType *instanceTyp, Visibility visibility, Type *memberTyp)
        : instanceTyp(instanceTyp), visibility(visibility), memberTyp(memberTyp) {}
    void printType(std::ostream &os) override {
        os << "    MemberType: " << getVisibilityString(visibility) << " ";
        memberTyp->printType(os);
    }
    [[nodiscard]] ClassType *getInstanceTyp() const { return instanceTyp; }
    [[nodiscard]] Visibility getVisibility() const { return visibility; }
    [[nodiscard]] Type *getMemberTyp() const { return memberTyp; }
    bool operator==(const MemberType &rhs) const {
        return instanceTyp == rhs.instanceTyp
            && visibility == rhs.visibility
            && memberTyp == rhs.memberTyp;
    }
};

class InterfaceType : public AggregateType {
    std::vector<InterfaceType *> interfaces;
    std::map<std::string, std::vector<MemberType *>> members{};
  public:
    explicit InterfaceType(std::vector<InterfaceType *> interfaces)
        : interfaces(std::move(interfaces)) {}
    void addInterfaceMethod(const std::string &name, MemberType *typ) {
        members[name].push_back(typ);
    }
    std::vector<MemberType *> findMemberTyp(const std::string &name) override {
        return {};
    }
    [[nodiscard]] bool isInterfaceTyp() const override { return true; }
    [[nodiscard]] bool isClassTyp() const override { return false; }
    void printType(std::ostream &os) override {
        os << "InterfaceType: " << "(" << interfaces.size() << ")" << std::endl;
        for (auto &[name, overloads]: members) {
            os << "  " << name << ": " << std::endl;
            for (auto overload: overloads) {
                overload->printType(os);
            }
        }
    }
    bool operator==(const InterfaceType &rhs) const {
        return interfaces == rhs.interfaces
            && members == rhs.members;
    }
};

class ClassType : public AggregateType {
  private:
    ClassType *baseclass;
    std::vector<InterfaceType *> interfaces;
    std::map<std::string, std::vector<MemberType *>> members{};
  public:
    ClassType(ClassType *baseclass, std::vector<InterfaceType *> interfaces)
        : baseclass(baseclass), interfaces(std::move(interfaces)) {}
    void addMember(const std::string &name, MemberType *typ) {
        members[name].push_back(typ);
    }
    std::vector<MemberType *> findMemberTyp(const std::string &name) override {
        return {};
    }
    [[nodiscard]] bool isInterfaceTyp() const override { return false; }
    [[nodiscard]] bool isClassTyp() const override { return true; }
    void printType(std::ostream &os) override {
        os << "ClassType: " << "(" << std::internal << std::hex << baseclass << ") -> "
           << interfaces.size() << std::endl;
        for (auto &[name, overloads]: members) {
            os << "  " << name << ": " << std::endl;
            for (auto overload: overloads) {
                overload->printType(os);
            }
        }
    }
    bool operator==(const ClassType &rhs) const {
        return baseclass == rhs.baseclass
            && interfaces == rhs.interfaces
            && members == rhs.members;
    }
};

}

#endif //REFLEX_SRC_TYPES_TYPE_H_
