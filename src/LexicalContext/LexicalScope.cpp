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

LexicalScope *LexicalScope::bind(const std::string &name) const {
    return nullptr;
}

void LexicalScope::getScopeQualifierPrefix(QuantifierList &prefix) const {
    if (isGlobalScope()) return;
    prefix.push_front(scopename);
    parentScope->getScopeQualifierPrefix(prefix);
}

}
