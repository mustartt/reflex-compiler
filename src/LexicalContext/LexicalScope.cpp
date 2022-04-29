//
// Created by henry on 2022-04-27.
//

#include "LexicalScope.h"

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
    ScopeMember member(std::move(name), memberType, this, child);
    auto res = std::find_if(members.begin(), members.end(),
                            [member](const auto &scope) {
                              return *scope.get() == member;
                            });
    if (res != members.end()) {
        auto &existing = *res;
        return *existing;
    }
    members.push_back(std::make_unique<ScopeMember>(std::move(member)));
    return *members.back().get();
}

ScopeMember *LexicalScope::bind(const std::string &name) const {
    for (const auto &member: members) {
        if (member->getMembername() == name) {
            return member.get();
        }
    }
    if (parentScope) {
        return parentScope->bind(name);
    }
    throw LexicalError{"Cannot bind " + name + " in any of its parent lexical scopes"};
}

ScopeMember *ScopeMember::follow(const std::string &qualifiedName) {
    if (qualifiedName.empty()) return this;
    auto pos = qualifiedName.find("::");
    auto prefix = qualifiedName.substr(0, pos);
    auto rest = qualifiedName.substr(pos + 2);
    if (!hasScope()) throw LexicalError{"Cannot resolve " + rest + " in current scope " + getStringQualifier()};
    for (auto &member: child->getMembers()) {
        if (member->getMembername() == prefix) {
            return member->follow(rest);
        }
    }
    throw LexicalError{"Cannot resolve " + prefix + " in current scope " + getStringQualifier()};
}

void LexicalScope::getScopeQualifierPrefix(QuantifierList &prefix) const {
    if (!parentScope) return;
    prefix.push_front(scopename);
    parentScope->getScopeQualifierPrefix(prefix);
}

}
