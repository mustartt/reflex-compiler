//
// Created by henry on 2022-04-01.
//

#ifndef REFLEX_SRC_TYPECHECKER_SCOPESYMBOLTYPETABLE_H_
#define REFLEX_SRC_TYPECHECKER_SCOPESYMBOLTYPETABLE_H_

#include <deque>
#include <unordered_map>
#include <string>
#include <memory>
#include <ostream>

namespace reflex {

using QuantifierList = std::deque<std::string>;

class Type;
class ScopeSymbolTypeTable {
    ScopeSymbolTypeTable *parentScope = nullptr;
    std::string name{};
    std::unordered_map<std::string, Type *> symbolTable{};
    std::unordered_map<std::string, std::unique_ptr<ScopeSymbolTypeTable>> namedScope{};
  public:
    ScopeSymbolTypeTable() = default;
    ScopeSymbolTypeTable(ScopeSymbolTypeTable *parentScope, std::string name);

    static QuantifierList getQuantifierList(const std::string &identifier);

    [[nodiscard]] bool isGlobalScope() const;
    [[nodiscard]] const std::string &getScopeName() const;
    void setParentScope(ScopeSymbolTypeTable *parent);

    void addScopeMember(const std::string &identifier, Type *type);
    void addNamedScope(const std::string &identifier, std::unique_ptr<ScopeSymbolTypeTable> scope);

    Type *findReferencedType(const QuantifierList &list);

    [[nodiscard]] std::string getScopePrefix() const;
    [[nodiscard]] bool isInCurrentScope(const std::string &identifier) const;
    [[nodiscard]] ScopeSymbolTypeTable *getNestedScope(const std::string &identifier) const;
    [[nodiscard]] Type *getCurrentScopeReference(const std::string &identifier) const;

    void printScope(std::ostream &os, size_t indent);
  private:
    static void printIndent(std::ostream &os, size_t indent);
};

}

#endif //REFLEX_SRC_TYPECHECKER_SCOPESYMBOLTYPETABLE_H_
