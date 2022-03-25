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
};

class VoidType : public Type {
  public:
    std::string getTypeString() override;
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

class MemberType;
class AggregateType : public Type {
  public:
    virtual std::vector<MemberType *> findMemberTyp(const std::string &) = 0;
    [[nodiscard]] virtual bool isInterfaceTyp() const = 0;
    [[nodiscard]] virtual bool isClassTyp() const = 0;
    [[nodiscard]] virtual bool validate(std::vector<TypeError> &) const = 0;
};

class ClassType;
class MemberType : public Type {
    ClassType *instanceTyp;
    Visibility visibility;
    Type *memberTyp;
  public:
    MemberType(ClassType *instanceTyp, Visibility visibility, Type *memberTyp)
        : instanceTyp(instanceTyp), visibility(visibility), memberTyp(memberTyp) {}
    [[nodiscard]] ClassType *getInstanceTyp() const { return instanceTyp; }
    [[nodiscard]] Visibility getVisibility() const { return visibility; }
    [[nodiscard]] Type *getMemberTyp() const { return memberTyp; }
    std::string getTypeString() override;
    [[nodiscard]] bool isFunctionTyp() const { return dynamic_cast<FunctionType *>(memberTyp); }
    bool operator==(const MemberType &rhs) const;
};

class InterfaceType : public AggregateType {
    std::string interfacename;
    std::vector<InterfaceType *> interfaces;
    std::map<std::string, std::vector<MemberType *>> members{};
  public:
    explicit InterfaceType(std::string interfacename, std::vector<InterfaceType *> interfaces)
        : interfacename(std::move(interfacename)), interfaces(std::move(interfaces)) {}
    void addInterfaceMethod(const std::string &name, MemberType *typ) {
        members[name].push_back(typ);
    }
    std::vector<MemberType *> findMemberTyp(const std::string &name) override {
        return {};
    }
    [[nodiscard]] bool isInterfaceTyp() const override { return true; }
    [[nodiscard]] bool isClassTyp() const override { return false; }
    std::string getTypeString() override;
    bool operator==(const InterfaceType &rhs) const;
    bool validate(std::vector<TypeError> &vector) const override {
        return false;
    }
};

class ClassType : public AggregateType {
  private:
    std::string classname;
    ClassType *baseclass;
    std::vector<InterfaceType *> interfaces;
    std::map<std::string, std::vector<MemberType *>> members{};
  public:
    ClassType(std::string classname, ClassType *baseclass, std::vector<InterfaceType *> interfaces)
        : classname(std::move(classname)), baseclass(baseclass), interfaces(std::move(interfaces)) {}
    void addMember(const std::string &name, MemberType *typ) {
        members[name].push_back(typ);
    }
    std::vector<MemberType *> findMemberTyp(const std::string &name) override {
        return {};
    }
    [[nodiscard]] bool isInterfaceTyp() const override { return false; }
    [[nodiscard]] bool isClassTyp() const override { return true; }
    std::string getTypeString() override;
    bool operator==(const ClassType &rhs) const;
    bool validate(std::vector<TypeError> &vector) const override {
        for (auto&[name, overloads]: members) {
            // no duplicate types for the same name except for functions
            size_t funcCount = std::count_if(overloads.begin(), overloads.end(),
                                             [](const auto memberTyp) -> bool {
                                               return memberTyp->isFunctionTyp();
                                             });
            if (overloads.size() - funcCount > 1) {
                return false;
            }
            // overloads must have different parameter
            std::vector<std::vector<Type *>> seenParams;
            for (auto overload: overloads) {
                if (overload->isFunctionTyp()) {
                    auto func = dynamic_cast<FunctionType *>(overload->getMemberTyp());
                    auto &params = func->getParamTyp();
                    if (std::find(seenParams.begin(), seenParams.end(), params) != seenParams.end()) {
                        seenParams.push_back(params);
                    } else {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    [[nodiscard]] bool isAbstract() {
        return false;
    }
};

}

#endif //REFLEX_SRC_TYPES_TYPE_H_
