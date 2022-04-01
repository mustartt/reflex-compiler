//
// Created by henry on 2022-03-31.
//

#ifndef REFLEX_SRC_TYPECHECKER_ASTRECORDTYPEANNOTATOR_H_
#define REFLEX_SRC_TYPECHECKER_ASTRECORDTYPEANNOTATOR_H_

#include <memory>
#include <stack>

#include <AstVisitor.h>

namespace reflex {

class TypeContextManager;
class ScopeSymbolTypeTable;
class AstRecordTypeAnnotator : public AstVisitor {
    TypeContextManager &typeContext;

    std::unique_ptr<ScopeSymbolTypeTable> root{};
    std::stack<ScopeSymbolTypeTable *> parentStack{};
    size_t prefixCounter = 0;

  public:
    explicit AstRecordTypeAnnotator(TypeContextManager &tyCtx) : typeContext(tyCtx) {};

    std::unique_ptr<ScopeSymbolTypeTable> annotate(CompilationUnit *unit);

    void visit(ArrayLit *visitable) override;
    void visit(FunctionLit *visitable) override;
    void visit(UnaryExpr *visitable) override;
    void visit(BinaryExpr *visitable) override;
    void visit(CastExpr *visitable) override;
    void visit(SelectorExpr *visitable) override;
    void visit(IndexExpr *visitable) override;
    void visit(ArgumentExpr *visitable) override;
    void visit(VariableDecl *visitable) override;
    void visit(ReturnStmt *visitable) override;
    void visit(IfStmt *visitable) override;
    void visit(ForRangeClause *visitable) override;
    void visit(ForNormalClause *visitable) override;
    void visit(ForStmt *visitable) override;
    void visit(WhileStmt *visitable) override;
    void visit(AssignmentStmt *visitable) override;
    void visit(IncDecStmt *visitable) override;
    void visit(ExpressionStmt *visitable) override;
    void visit(Block *visitable) override;
    void visit(FunctionDecl *visitable) override;
    void visit(ClassDecl *visitable) override;
    void visit(MemberDecl *visitable) override;
    void visit(InterfaceDecl *visitable) override;
    void visit(CompilationUnit *visitable) override;
};

}

#endif //REFLEX_SRC_TYPECHECKER_ASTRECORDTYPEANNOTATOR_H_
