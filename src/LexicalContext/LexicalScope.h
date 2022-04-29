//
// Created by henry on 2022-04-27.
//

#ifndef REFLEX_SRC_TRANSFORMPASS_LEXICALSCOPE_H_
#define REFLEX_SRC_TRANSFORMPASS_LEXICALSCOPE_H_

#include <string>
#include <set>
#include <utility>
#include <vector>
#include <memory>
#include <list>

namespace reflex {

class Type;
class ASTNode;
class LexicalScope;
class LexicalContext;
class AggregateDecl;
class FunctionDecl;
class MethodDecl;
class BlockStmt;
class FunctionLiteral;

using QuantifierList = std::list<std::string>;

class LexicalError : public std::runtime_error {
  public:
    explicit LexicalError(const std::string &arg);
};

class ScopeMember {
  public:
    ScopeMember(std::string membername, Type *memberType,
                LexicalScope *parent, LexicalScope *child)
        : membername(std::move(membername)), memberType(memberType),
          parent(parent), child(child) {
        if (!parent) throw LexicalError{"ScopeMember must be part of a parent scope"};
    }

    const std::string &getMembername() const { return membername; }
    Type *getMemberType() const { return memberType; }
    bool hasScope() const { return child; }
    LexicalScope *getChild() const { return child; }
    LexicalScope *getParent() const { return parent; }
    void setMemberType(Type *type) { ScopeMember::memberType = type; }

    void setChild(LexicalScope *childscope) { child = childscope; }
    QuantifierList getQualifier() const;
    std::string getStringQualifier() const;

    /// Uses membername and memberType for equality
    bool operator==(const ScopeMember &rhs) const;
    bool operator<(const ScopeMember &rhs) const;
    static constexpr auto ScopeCmp =
        [](const ScopeMember &lhs, const ScopeMember &rhs) {
          return lhs < rhs;
        };

    /// Attempt to follow the qualifier name to find the referenced member in its children scope
    /// @return the scope member if the qualifier is resolved
    /// @throws LexicalError if qualifier cannot be resolved in its child scope
    ScopeMember *follow(const std::string &qualifiedName);
  private:
    std::string membername;
    Type *memberType;
    LexicalScope *parent;
    LexicalScope *child;
};

enum class LexicalScopeType {
  Global, Composite, Function, Method, Block, Lambda
};

class LexicalScope {
  public:
    LexicalScope(LexicalContext &context, LexicalScope *parentScope,
                 LexicalScopeType type, std::string scopename, ASTNode *decl)
        : context(context), parentScope(parentScope), scopeType(type),
          decl(decl), scopename(std::move(scopename)) {}

    bool isGlobalScope() const { return !parentScope; }

    /// Adds a scope member to the current LexicalScope with associated type and name
    /// by default it has no associated children scope.
    /// @note name is not unique in the lexical scope for the sake of implementing overloading
    ScopeMember &addScopeMember(std::string name, Type *memberType, LexicalScope *child = nullptr);

    LexicalContext &getContext() const { return context; }
    LexicalScope *getParentScope() const { return parentScope; }
    const std::string &getScopename() const { return scopename; }
    ASTNode *getNodeDecl() const { return decl; }
    const std::vector<std::unique_ptr<ScopeMember>> &getMembers() const { return members; }
    LexicalScopeType getScopeType() const { return scopeType; }

    void getScopeQualifierPrefix(QuantifierList &prefix) const;

    /// Creates a children lexical scope by forwarding arguments to the context
    /// @returns LexicalScope @{
    LexicalScope *createCompositeScope(AggregateDecl *decl);
    LexicalScope *createFunctionScope(FunctionDecl *decl);
    LexicalScope *createMethodScope(MethodDecl *decl);
    LexicalScope *createBlockScope(BlockStmt *decl);
    LexicalScope *createLambdaScope(FunctionLiteral *decl);
    /// @}

    /// Recursively binds to the nearest parent scope member starting at the current scope
    /// to find a lexical scope member that has the same name
    /// @return the bound scope with the same name
    /// @throws LexicalError if no such member exists
    ScopeMember *bind(const std::string &name) const;

  private:
    size_t incBlockCount() { return blockCount++; }
    size_t incLambdaCount() { return lambdaCount++; }

    LexicalContext &context;
    LexicalScope *parentScope;
    LexicalScopeType scopeType;
    ASTNode *decl;
    std::string scopename;
    std::vector<std::unique_ptr<ScopeMember>> members;
    size_t blockCount = 0;
    size_t lambdaCount = 0;
};

}

#endif //REFLEX_SRC_TRANSFORMPASS_LEXICALSCOPE_H_
