//
// Created by henry on 2022-03-21.
//

#ifndef REFLEX_SRC_AST_VISITOR_ASTPRINTER_H_
#define REFLEX_SRC_AST_VISITOR_ASTPRINTER_H_

#include "../AstVisitor.h"
#include <ostream>

namespace reflex {

class AstPrinter : public AstVisitor {
    std::ostream &output;
    size_t indent;
  public:
    explicit AstPrinter(std::ostream &output);
    void visit(IndexExpr *visitable) override;
    void visit(Identifier *visitable) override;
    void visit(ModuleSelector *visitable) override;
    void visit(NumberLit *visitable) override;
    void visit(StringLit *visitable) override;
    void visit(BoolLit *visitable) override;
    void visit(NullLit *visitable) override;
    void visit(IdentifierType *visitable) override;
    void visit(ArrayType *visitable) override;
    void visit(FunctionType *visitable) override;
    void visit(ArrayLit *visitable) override;
    void visit(Parameter *visitable) override;
    void visit(FunctionLit *visitable) override;
    void visit(UnaryExpr *visitable) override;
    void visit(BinaryExpr *visitable) override;
    void visit(NewExpr *visitable) override;
    void visit(CastExpr *visitable) override;
    void visit(SelectorExpr *visitable) override;
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
  private:
    void generateIndent();
};

}

#endif //REFLEX_SRC_AST_VISITOR_ASTPRINTER_H_
