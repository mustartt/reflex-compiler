//
// Created by henry on 2022-04-30.
//

#ifndef REFLEX_SRC_LEXICALCONTEXT_SEMANTICANALYSISPASS_H_
#define REFLEX_SRC_LEXICALCONTEXT_SEMANTICANALYSISPASS_H_

#include <map>
#include <stack>
#include <memory>
#include <set>

#include "ASTVisitor.h"
#include "TypeParser.h"

namespace reflex {

class ReferenceError : public std::runtime_error {
  public:
    explicit ReferenceError(const std::string &arg);
};

class AnalysisError : public std::runtime_error {
  public:
    explicit AnalysisError(const std::string &arg);
};

class Declaration;

class SymbolTable final {
  public:
    explicit SymbolTable(SymbolTable *parent) : parent(parent) {}

    /// find variable decl in current table or parent scope
    /// @param name find the referenced decl
    /// @returns the VariableDecl in the nearest scope
    /// @throws ReferenceError if name can't be found
    Declaration *find(const std::string &name);

    /// add decl to the current scope
    /// @param decl decl to be added
    /// @param name name override, defaults to use @p decl declname
    /// @throws ReferenceError if name already exists in current scope
    void add(Declaration *decl, const std::string &name = "");

    bool isGlobalTable() const { return parent == nullptr; }

  private:
    SymbolTable *parent;
    std::map<std::string, Declaration *> symbolTable;
    std::set<Declaration *> captures;
};

class ASTContext;
class TypeContext;
class LexicalScope;
class Expression;

/// Class for preforming type analysis on Expression and inserts ImplicitCastExpr
class ExprAnalysisPass : public ASTExprVisitor {
  public:
    OpaqueType visit(DeclRefExpr &expr) override;
    OpaqueType visit(NewExpr &expr) override;
    OpaqueType visit(NumberLiteral &literal) override;
    OpaqueType visit(StringLiteral &literal) override;
    OpaqueType visit(BooleanLiteral &literal) override;
    OpaqueType visit(NullLiteral &literal) override;

    OpaqueType visit(UnaryExpr &expr) override;
    OpaqueType visit(BinaryExpr &expr) override;
    OpaqueType visit(CastExpr &expr) override;
    OpaqueType visit(IndexExpr &expr) override;
    OpaqueType visit(SelectorExpr &expr) override;
    OpaqueType visit(ArgumentExpr &expr) override;
    OpaqueType visit(ArrayLiteral &literal) override;

    OpaqueType visit(FunctionLiteral &literal) override;

  private:
    /// Attempts to create implicit cast from @p expr original type to @p targetType
    /// by inserting ImplicitCast Conversion to the AST
    /// @throws TypeError if no implicit conversion possible
    Expression *insertImplicitCast(Expression *expr, Type *targetType);

    LexicalScope *lexicalscope;
    SymbolTable *table;
    TypeContext &typeContext;
    ASTContext &astContext;
};

/// Class for performing semantic analysis on FunctionDecl, MethodDecl, FunctionLiteral
class SemanticAnalysisPass : public ASTStmtVisitor {
    friend class ExprAnalysisPass;
  public:
    SemanticAnalysisPass(TypeContext &typeContext, LexicalScope *lexicalScope)
        : typeContext(typeContext), typeParser(typeContext) {
        lexicalScopes.push(lexicalScope);
    }
    ~SemanticAnalysisPass() override {
        lexicalScopes.pop();
    }

    void analyzeFunction(FunctionDecl *decl);
    void analyzeMethod(MethodDecl *decl);
    void analyzeLambda(FunctionLiteral *literal);

    OpaqueType visit(BlockStmt &stmt) override;
    OpaqueType visit(DeclStmt &stmt) override;

    OpaqueType visit(ReturnStmt &stmt) override ASTVisitorDefImpl
    OpaqueType visit(BreakStmt &stmt) override;
    OpaqueType visit(ContinueStmt &stmt) override;
    OpaqueType visit(IfStmt &stmt) override ASTVisitorDefImpl
    OpaqueType visit(ForStmt &stmt) override ASTVisitorDefImpl
    OpaqueType visit(WhileStmt &stmt) override ASTVisitorDefImpl
    OpaqueType visit(EmptyStmt &stmt) override ASTVisitorDefImpl
    OpaqueType visit(AssignmentStmt &stmt) override ASTVisitorDefImpl
    OpaqueType visit(IncDecStmt &stmt) override ASTVisitorDefImpl
    OpaqueType visit(ExpressionStmt &stmt) override ASTVisitorDefImpl

    TypeContext &getTypeContext() { return typeContext; }
    TypeParser &getTypeParser() { return typeParser; }

  private:
    TypeContext &typeContext;
    TypeParser typeParser;

    std::stack<LexicalScope *> lexicalScopes;
    std::stack<std::unique_ptr<SymbolTable>> symbolTables;
    bool inloop = false;
};

} // reflex

#endif //REFLEX_SRC_LEXICALCONTEXT_SEMANTICANALYSISPASS_H_