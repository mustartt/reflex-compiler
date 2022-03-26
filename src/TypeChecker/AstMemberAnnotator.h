//
// Created by henry on 2022-03-26.
//

#ifndef REFLEX_SRC_TYPECHECKER_ASTMEMBERANNOTATOR_H_
#define REFLEX_SRC_TYPECHECKER_ASTMEMBERANNOTATOR_H_

#include <AstVisitor.h>
#include <TypeContextManager.h>
#include <AstContextManager.h>
#include <TypeParser.h>

namespace reflex {

class AstMemberAnnotator : public AstVisitor {
    TypeContextManager *typeContext;
    AstContextManager *astContext;
    TypeParser parser;
  public:
    AstMemberAnnotator(TypeContextManager *typeContext, AstContextManager *astContext);

    void annotate();

    void visit(VariableDecl *visitable) override;
    void visit(FunctionDecl *visitable) override;
    void visit(ClassDecl *visitable) override;
    void visit(MemberDecl *visitable) override;
    void visit(InterfaceDecl *visitable) override;
    void visit(FunctionLit *visitable) override;
    void visit(ParamDecl *visitable) override;
    void visit(NewExpr *visitable) override;
    void visit(CastExpr *visitable) override;
    void visit(NumberLit *visitable) override;
    void visit(BoolLit *visitable) override;
    void visit(NullLit *visitable) override;
  private:
    void annotateClassAndInterfaceDecls();
    void annotateFunctionDecls();
    void annotateFunctionLits();
    void annotateVariableDecls();
    void annotateBasicLiterals();
    void annotateCastAndNewExpr();
};

}

#endif //REFLEX_SRC_TYPECHECKER_ASTMEMBERANNOTATOR_H_
