//
// Created by henry on 2022-04-27.
//

#include "LexicalScope.h"

namespace reflex {

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

}
