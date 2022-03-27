//
// Created by henry on 2022-03-26.
//

#ifndef REFLEX_SRC_TYPECHECKER_ASTEXPRESSIONANNOTATOR_H_
#define REFLEX_SRC_TYPECHECKER_ASTEXPRESSIONANNOTATOR_H_

#include <string>
#include <unordered_map>
#include <AstVisitor.h>
#include <AstContextManager.h>
#include <TypeContextManager.h>
#include <stack>
#include "Type.h"
#include "SemanticError.h"

namespace reflex {

class ScopeSymbolTable {
    ScopeSymbolTable *parentScope = nullptr;
    std::unordered_map<std::string, Type *> symbolTable{};
  public:
    ScopeSymbolTable() = default;
    explicit ScopeSymbolTable(ScopeSymbolTable *parentScope) : parentScope(parentScope) {}

    [[nodiscard]] bool isGlobalScope() const { return !parentScope; }
    Type *getReferencedType(const std::string &name) {
        if (symbolTable.count(name)) return symbolTable[name];
        if (isGlobalScope()) return nullptr;
        return parentScope->getReferencedType(name);
    }
    void addScopeMember(const std::string &name, Type *type) {
        if (symbolTable.count(name))
            throw SemanticError{"Reference already exists in current scope " + name};
        symbolTable[name] = type;
    }
    [[nodiscard]] bool isInCurrentScope(const std::string &name) const {
        return symbolTable.count(name);
    }
};

class AstExpressionAnnotator : public AstVisitor {
    AstContextManager *astContext;
    TypeContextManager *typeContext;
    std::stack<std::unique_ptr<ScopeSymbolTable>> symbolTable;
    std::stack<Type *> typeStack;
    CompilationUnit *root;
  public:
    AstExpressionAnnotator(TypeContextManager *typeContext,
                           AstContextManager *astContext,
                           CompilationUnit *root);

    void annotate();

    void visit(ClassDecl *visitable) override;
    void visit(MemberDecl *visitable) override;
    void visit(NewExpr *visitable) override;
    void visit(ArgumentExpr *visitable) override;
    void visit(CastExpr *visitable) override;
    void visit(SelectorExpr *visitable) override;
    void visit(ExpressionStmt *visitable) override;
    void visit(BreakStmt *visitable) override;
    void visit(ContinueStmt *visitable) override;
    void visit(EmptyStmt *visitable) override;
    void visit(ReturnStmt *visitable) override;
    void visit(Block *visitable) override;
    void visit(FunctionDecl *visitable) override;
    void visit(NumberLit *visitable) override;
    void visit(BoolLit *visitable) override;
    void visit(NullLit *visitable) override;
    void visit(UnaryExpr *visitable) override;
    void visit(Identifier *visitable) override;
    void visit(BinaryExpr *visitable) override;
    void visit(VariableDecl *visitable) override;
    void visit(CompilationUnit *visitable) override;
};

}

#endif //REFLEX_SRC_TYPECHECKER_ASTEXPRESSIONANNOTATOR_H_
