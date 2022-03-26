//
// Created by henry on 2022-03-25.
//

#ifndef REFLEX_SRC_TYPECHECKER_ASTSEMANTICCHECKER_H_
#define REFLEX_SRC_TYPECHECKER_ASTSEMANTICCHECKER_H_

#include "AstVisitor.h"
#include "TypeContextManager.h"
#include "AstContextManager.h"

#include <unordered_map>
#include <vector>
#include <stack>

namespace reflex {

class SemanticError : public std::runtime_error {
  public:
    explicit SemanticError(const std::string &arg)
        : std::runtime_error("Semantic Error: " + arg) {}
};

class ScopeSymbolTable {
    ScopeSymbolTable *parentScope = nullptr;
    std::unordered_map<std::string, std::unordered_set<Type *>> symbolTable{};
  public:
    ScopeSymbolTable() = default;
    explicit ScopeSymbolTable(ScopeSymbolTable *parentScope) : parentScope(parentScope) {}

    [[nodiscard]] bool isGlobalScope() const { return !parentScope; }
    std::unordered_set<Type *> getReferencedType(const std::string &name) {
        if (isGlobalScope()) {
            if (symbolTable.count(name))
                return symbolTable.at(name);
            return {};
        }
        auto parentRefs = parentScope->getReferencedType(name);
        if (symbolTable.count(name)) {
            auto &currentScopeRefs = symbolTable.at(name);
            parentRefs.insert(currentScopeRefs.begin(), currentScopeRefs.end());
        }
        return parentRefs;
    }
    void addScopeMember(const std::string &name, Type *type) {
        symbolTable[name].insert(type);
    }
};

class AstSemanticChecker : public AstVisitor {
    AstContextManager *astContext;
    TypeContextManager *typeContext;
    std::stack<ScopeSymbolTable> symbolTable;
    std::stack<Type *> typeStack;
  public:
    AstSemanticChecker(AstContextManager *astContext, TypeContextManager *typeContext);

    void visit(Identifier *visitable) override;
    void visit(BinaryExpr *visitable) override;
    void visit(VariableDecl *visitable) override;
    void visit(CompilationUnit *visitable) override;
};

}

#endif //REFLEX_SRC_TYPECHECKER_ASTSEMANTICCHECKER_H_
