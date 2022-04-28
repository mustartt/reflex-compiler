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

namespace reflex {

class Type;
class ASTNode;
class LexicalScope;
class LexicalContext;

class ScopeMember {
  public:
    ScopeMember(std::string membername, Type *memberType,
                LexicalScope *parent, LexicalScope *child)
        : membername(std::move(membername)), memberType(memberType),
          parent(parent), child(child) {}

    const std::string &getMembername() const { return membername; }
    Type *getMemberType() const { return memberType; }
    bool hasScope() const { return child; }
    LexicalScope *getChild() const { return child; }
    LexicalScope *getParent() const { return parent; }

    void setChild(LexicalScope *childscope) { child = childscope; }

    bool operator==(const ScopeMember &rhs) const;
    bool operator<(const ScopeMember &rhs) const;
    static constexpr auto ScopeCmp =
        [](const ScopeMember &lhs, const ScopeMember &rhs) {
          return lhs < rhs;
        };
  private:
    std::string membername;
    Type *memberType;
    LexicalScope *parent;
    LexicalScope *child;
};

class LexicalScope {
  public:
    LexicalScope(LexicalContext &context, LexicalScope *parentScope,
                 std::string scopename, ASTNode *decl)
        : context(context), parentScope(parentScope),
          decl(decl), scopename(std::move(scopename)) {}

    bool isGlobalScope() const { return !parentScope; }
    ScopeMember &addScopeMember(std::string name, Type *memberType, LexicalScope *child = nullptr);

    LexicalContext &getContext() const { return context; }
    LexicalScope *getParentScope() const { return parentScope; }
    ASTNode *getScopeDecl() const { return decl; }
    const std::string &getScopename() const { return scopename; }
    ASTNode *getNodeDecl() const { return decl; }
    const std::vector<std::unique_ptr<ScopeMember>> &getMembers() const { return members; }

  private:
    LexicalContext &context;
    LexicalScope *parentScope;
    ASTNode *decl;
    std::string scopename;
    std::vector<std::unique_ptr<ScopeMember>> members;
};

}

#endif //REFLEX_SRC_TRANSFORMPASS_LEXICALSCOPE_H_
