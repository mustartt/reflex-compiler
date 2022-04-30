//
// Created by henry on 2022-04-27.
//

#include "LexicalScope.h"

#include "Type.h"
#include "LexicalContext.h"

namespace reflex {

std::string ScopeMember::getStringQualifier() const {
    auto list = getQualifier();
    std::string base = *list.begin();
    auto i = ++list.begin();
    for (; i != list.end(); ++i) {
        base += "::" + *i;
    }
    return base;
}

bool ScopeMember::operator<(const ScopeMember &rhs) const {
    if (this->getMembername() < rhs.getMembername())
        return true;
    if (rhs.getMembername() < this->getMembername())
        return false;
    return this->getMemberType() < rhs.getMemberType();
}

bool ScopeMember::operator==(const ScopeMember &rhs) const {
    return membername == rhs.membername
        && memberType == rhs.memberType;
}

QuantifierList ScopeMember::getQualifier() const {
    QuantifierList list;
    list.push_front(membername);
    parent->getScopeQualifierPrefix(list);
    return list;
}

LexicalError::LexicalError(const std::string &arg)
    : runtime_error("LexicalError: " + arg) {}

ScopeMember &LexicalScope::addScopeMember(std::string name, Type *memberType,
                                          LexicalScope *child) {
    // find member of same name
    auto res = std::find_if(members.begin(), members.end(),
                            [&name](const auto &existing) {
                              return existing->getMembername() == name;
                            });
    if (res != members.end()) {
        // no impl for any types of overloading
        throw LexicalError{"Cannot overload " + name + " in " + getScopename()};
    }
    members.push_back(std::make_unique<ScopeMember>(std::move(name), memberType, this, child));
    return *members.back().get();
}

ScopeMember *LexicalScope::bind(const std::string &name) const {
    for (const auto &member: members) {
        if (member->getMembername() == name) {
            return member.get();
        }
    }
    if (parentMember) {
        return parentMember->getParent()->bind(name);
    }
    throw LexicalError{"Cannot bind " + name + " in any of its parent lexical scopes"};
}

ScopeMember *ScopeMember::follow(const std::string &qualifiedName) {
    if (qualifiedName.empty()) return this;
    auto pos = qualifiedName.find("::");

    std::string prefix;
    std::string rest;
    if (pos != std::string::npos) {
        prefix = qualifiedName.substr(0, pos);
        rest = qualifiedName.substr(pos + 2);
    } else {
        prefix = qualifiedName;
    }

    if (!hasScope()) throw LexicalError{"Cannot resolve " + rest + " in current scope " + getStringQualifier()};
    for (auto &member: child->getMembers()) {
        if (member->getMembername() == prefix) {
            return member->follow(rest);
        }
    }
    throw LexicalError{"Cannot resolve " + prefix + " in current scope " + getStringQualifier()};
}

ScopeMember *LexicalScope::resolve(const std::string &qualifier) const {
    if (qualifier.empty()) throw LexicalError{"Cannot resolve empty qualifier"};
    auto pos = qualifier.find("::");

    std::string prefix;
    std::string rest;
    if (pos != std::string::npos) {
        prefix = qualifier.substr(0, pos);
        rest = qualifier.substr(pos + 2);
    } else {
        prefix = qualifier;
    }

    return bind(prefix)->follow(rest);
}

void LexicalScope::getScopeQualifierPrefix(QuantifierList &prefix) const {
    if (!parentMember) return;
    prefix.push_front(scopename);
    parentMember->getParent()->getScopeQualifierPrefix(prefix);
}

std::pair<LexicalScope *, ScopeMember *> LexicalScope::createCompositeScope(AggregateDecl *declscope) {
    auto &newScopeMember = addScopeMember(declscope->getDeclname(), nullptr);
    auto scope = newScopeMember.setChild(
        context.createCompositeScope(declscope, &newScopeMember)
    );
    return {scope, &newScopeMember};
}

std::pair<LexicalScope *, ScopeMember *> LexicalScope::createFunctionScope(FunctionDecl *declscope) {
    auto &newScopeMember = addScopeMember(declscope->getDeclname(), nullptr);
    auto scope = newScopeMember.setChild(
        context.createFunctionScope(declscope, &newScopeMember)
    );
    return {scope, &newScopeMember};
}

std::pair<LexicalScope *, ScopeMember *> LexicalScope::createMethodScope(MethodDecl *declscope) {
    auto &newScopeMember = addScopeMember(declscope->getDeclname(), nullptr);
    auto scope = newScopeMember.setChild(
        context.createMethodScope(declscope, &newScopeMember)
    );
    return {scope, &newScopeMember};
}
std::pair<LexicalScope *, ScopeMember *> LexicalScope::createBlockScope(BlockStmt *declscope) {
    auto &newScopeMember = addScopeMember("temporary", nullptr);
    auto scope = newScopeMember.setChild(
        context.createBlockScope(declscope, incBlockCount(), &newScopeMember)
    );
    newScopeMember.setMembername(scope->getScopename());
    return {scope, &newScopeMember};
}
std::pair<LexicalScope *, ScopeMember *> LexicalScope::createLambdaScope(FunctionLiteral *declscope) {
    auto &newScopeMember = addScopeMember("temporary", nullptr);
    auto scope = newScopeMember.setChild(
        context.createLambdaScope(declscope, incLambdaCount(), &newScopeMember)
    );
    newScopeMember.setMembername(scope->getScopename());
    return {scope, &newScopeMember};
}

}
