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
        // only function and method overloads are allowed otherwise result in an LexicalError
        auto &existing = *res;
        auto existingFunc = dynamic_cast<FunctionType *>(existing->getMemberType());
        auto newFunc = dynamic_cast<FunctionType *>(memberType);

        if (existingFunc && newFunc &&
            existingFunc->getParamTypes() != newFunc->getParamTypes()) {
            return *existing;
        }
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

ScopeMember *LexicalScope::resolve(const std::string &qualifier) const {
    if (qualifier.empty()) throw LexicalError{"Cannot resolve empty qualifier"};
    auto pos = qualifier.find("::");
    auto prefix = qualifier.substr(0, pos);
    auto rest = qualifier.substr(pos + 2);

    return bind(prefix)->follow(rest);
}

void LexicalScope::getScopeQualifierPrefix(QuantifierList &prefix) const {
    if (!parentScope) return;
    prefix.push_front(scopename);
    parentScope->getScopeQualifierPrefix(prefix);
}

LexicalScope *LexicalScope::createCompositeScope(AggregateDecl *declscope) {
    return context.createCompositeScope(declscope, this);
}

LexicalScope *LexicalScope::createFunctionScope(FunctionDecl *declscope) {
    return context.createFunctionScope(declscope, this);
}

LexicalScope *LexicalScope::createMethodScope(MethodDecl *declscope) {
    return context.createMethodScope(declscope, this);
}

LexicalScope *LexicalScope::createBlockScope(BlockStmt *declscope) {
    return context.createBlockScope(declscope, incBlockCount(), this);
}

LexicalScope *LexicalScope::createLambdaScope(FunctionLiteral *declscope) {
    return context.createLambdaScope(declscope, incLambdaCount(), this);

}

}
