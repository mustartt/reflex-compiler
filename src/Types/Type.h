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
    virtual void printType(std::ostream &os) = 0;
};

class VoidType : public Type {
  public:
    void printType(std::ostream &os) override;;
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
    [[nodiscard]] BaseType getBaseTyp() const { return baseTyp; }
    void printType(std::ostream &os) override;
  private:
    BaseType baseTyp;
};

class ArrType : public Type {
    Type *elementTyp;
  public:
    explicit ArrType(Type *elementTyp) : elementTyp(elementTyp) {}
    [[nodiscard]] Type *getElementTyp() const { return elementTyp; }
    void printType(std::ostream &os) override;
    bool operator==(const ArrType &rhs) const;
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
    void printType(std::ostream &os) override;
    bool operator==(const FuncType &rhs) const;
};

enum class Visibility {
  Public, Private, Protected
};
std::string getVisibilityString(Visibility visibility);
Visibility getVisibilityFromString(const std::string& ident);

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
    void printType(std::ostream &os) override;
    [[nodiscard]] ClassType *getInstanceTyp() const { return instanceTyp; }
    [[nodiscard]] Visibility getVisibility() const { return visibility; }
    [[nodiscard]] Type *getMemberTyp() const { return memberTyp; }
    [[nodiscard]] bool isFunctionTyp() const { return dynamic_cast<FuncType *>(memberTyp); }
    bool operator==(const MemberType &rhs) const;
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
    void printType(std::ostream &os) override;
    bool operator==(const InterfaceType &rhs) const;
    bool validate(std::vector<TypeError> &vector) const override {
        return false;
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
    void printType(std::ostream &os) override;
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
                    auto func = dynamic_cast<FuncType *>(overload->getMemberTyp());
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
