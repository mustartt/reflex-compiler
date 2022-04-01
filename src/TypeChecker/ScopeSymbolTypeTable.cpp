//
// Created by henry on 2022-04-01.
//

#include "ScopeSymbolTypeTable.h"

#include "SemanticError.h"
#include "Type.h"

namespace reflex {

ScopeSymbolTypeTable::ScopeSymbolTypeTable(ScopeSymbolTypeTable *parentScope, std::string name)
    : parentScope(parentScope), name(std::move(name)) {}

QuantifierList ScopeSymbolTypeTable::getQuantifierList(const std::string &identifier) {
    QuantifierList qident;
    size_t pos;
    std::string token;
    std::string delimiter = "::";
    std::string s = identifier;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        qident.push_back(std::move(token));
        s.erase(0, pos + delimiter.length());
    }
    qident.push_back(std::move(s));
    return qident;
}

bool ScopeSymbolTypeTable::isGlobalScope() const { return !parentScope; }

const std::string &ScopeSymbolTypeTable::getScopeName() const {
    return name;
}

void ScopeSymbolTypeTable::setParentScope(ScopeSymbolTypeTable *parent) {
    parentScope = parent;
}

void ScopeSymbolTypeTable::addScopeMember(const std::string &identifier, Type *type) {
    if (symbolTable.count(identifier))
        throw SemanticError{"Duplicate identifier in current scope " + identifier};
    symbolTable[identifier] = type;
}

void ScopeSymbolTypeTable::addNamedScope(const std::string &identifier, std::unique_ptr<ScopeSymbolTypeTable> scope) {
    if (namedScope.count(identifier))
        throw SemanticError{"Duplicate identifier in current named scope " + identifier};
    namedScope[identifier] = std::move(scope);
}

Type *ScopeSymbolTypeTable::findReferencedType(const QuantifierList &list) {
    if (list.empty()) throw SemanticError{"Invalid quantified identifier"};

    auto rest = list;
    rest.pop_front();

    if (rest.empty() && symbolTable.count(list.front()))
        return symbolTable[list.front()];
    if (!rest.empty() && namedScope.count(list.front()))
        return namedScope[list.front()]->findReferencedType(rest);

    if (isGlobalScope())
        throw SemanticError{"Unable to find quantified identifier"};
    return parentScope->findReferencedType(list);
}

std::string ScopeSymbolTypeTable::getScopePrefix() const {
    if (isGlobalScope()) return "";
    if (name.empty()) return parentScope->getScopePrefix();
    if (parentScope->isGlobalScope()) return name + "::";
    return parentScope->getScopePrefix() + name + "::";
}

bool ScopeSymbolTypeTable::isInCurrentScope(const std::string &identifier) const {
    return symbolTable.count(identifier);
}

void ScopeSymbolTypeTable::printScope(std::ostream &os, size_t indent) {
    printIndent(os, indent);
    os << "Scope: (" << name << ") {" << std::endl;
    for (auto &[symbol, type]: symbolTable) {
        printIndent(os, indent + 1);
        os << symbol << ": " << type->getTypeString() << std::endl;
    }
    for (auto &[scopename, scope]: namedScope) {
        scope->printScope(os, indent + 1);
    }
    printIndent(os, indent);
    os << "}" << std::endl;
}

void ScopeSymbolTypeTable::printIndent(std::ostream &os, size_t indent) {
    for (size_t i = 0; i < indent; ++i) os << "  ";
}

}
