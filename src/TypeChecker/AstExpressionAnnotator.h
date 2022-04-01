//
// Created by henry on 2022-04-01.
//

#ifndef REFLEX_SRC_TYPECHECKER_ASTEXPRESSIONANNOTATOR_H_
#define REFLEX_SRC_TYPECHECKER_ASTEXPRESSIONANNOTATOR_H_

#include <memory>
#include <stack>

#include <AstVisitor.h>

namespace reflex {

class TypeContextManager;
class ScopeSymbolTypeTable;
class AstExpressionAnnotator : public AstVisitor {
    TypeContextManager &typeContext;
    std::unique_ptr<ScopeSymbolTypeTable> &root;
    std::stack<ScopeSymbolTypeTable *> parentStack{};
    size_t prefixCounter = 0;

    std::stack<Type *> typeStack{};
    Type *typeHint = nullptr;
  public:
    AstExpressionAnnotator(TypeContextManager &typeContext, std::unique_ptr<ScopeSymbolTypeTable> &root);

    void annotate(CompilationUnit *unit);

    void visit(Identifier *visitable) override;
    void visit(NumberLit *visitable) override;
    void visit(StringLit *visitable) override;
    void visit(BoolLit *visitable) override;
    void visit(NullLit *visitable) override;
    void visit(ArrayLit *visitable) override;
    void visit(FunctionLit *visitable) override;
    void visit(UnaryExpr *visitable) override;
    void visit(BinaryExpr *visitable) override;
    void visit(NewExpr *visitable) override;
    void visit(CastExpr *visitable) override;
    void visit(SelectorExpr *visitable) override;
    void visit(IndexExpr *visitable) override;
    void visit(ArgumentExpr *visitable) override;
    void visit(VariableDecl *visitable) override;
    void visit(ReturnStmt *visitable) override;
    void visit(BreakStmt *visitable) override;
    void visit(ContinueStmt *visitable) override;
    void visit(IfStmt *visitable) override;
    void visit(ForRangeClause *visitable) override;
    void visit(ForNormalClause *visitable) override;
    void visit(ForStmt *visitable) override;
    void visit(WhileStmt *visitable) override;
    void visit(EmptyStmt *visitable) override;
    void visit(AssignmentStmt *visitable) override;
    void visit(IncDecStmt *visitable) override;
    void visit(ExpressionStmt *visitable) override;
    void visit(Block *visitable) override;
    void visit(FunctionDecl *visitable) override;
    void visit(ClassDecl *visitable) override;
    void visit(MemberDecl *visitable) override;
    void visit(InterfaceDecl *visitable) override;
    void visit(CompilationUnit *visitable) override;
  private:
    Type *popTypeStack();
};

}

#endif //REFLEX_SRC_TYPECHECKER_ASTEXPRESSIONANNOTATOR_H_
