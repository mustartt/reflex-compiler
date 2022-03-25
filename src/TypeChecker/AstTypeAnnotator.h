//
// Created by henry on 2022-03-24.
//

#ifndef REFLEX_SRC_TYPECHECKER_ASTTYPEANNOTATOR_H_
#define REFLEX_SRC_TYPECHECKER_ASTTYPEANNOTATOR_H_

#include <AstVisitor.h>
#include <TypeContextManager.h>
#include <TypeParser.h>

namespace reflex {

class TypeParseError : public std::runtime_error {
  public:
    explicit TypeParseError(const std::string &arg);
};

class ClassTypeError : public std::runtime_error {
  public:
    explicit ClassTypeError(const std::string &arg);
};

class InterfaceTypeError : public std::runtime_error {
  public:
    explicit InterfaceTypeError(const std::string &arg);
};

class AstTypeAnnotator : public AstVisitor {
    AstContextManager *astContext;
    TypeContextManager *typeContext;
    TypeParser parser;
  public:
    AstTypeAnnotator(AstContextManager *astContext, TypeContextManager *typeContext);
    void annotate();
  public:
    void visit(NumberLit *visitable) override;
    void visit(BoolLit *visitable) override;
    void visit(NullLit *visitable) override;
    void visit(ParamDecl *visitable) override;
    void visit(FunctionLit *visitable) override;
    void visit(NewExpr *visitable) override;
    void visit(CastExpr *visitable) override;
    void visit(VariableDecl *visitable) override;
    void visit(FunctionDecl *visitable) override;
    void visit(ClassDecl *visitable) override;
    void visit(MemberDecl *visitable) override;
    void visit(InterfaceDecl *visitable) override;
  private:
    void annotateInterfaceDecls();
    void annotateClassDecls();

    void annotateInterfaceBody();
    void annotateClassBody();

    void annotateFunctionDecls();
    void annotateFunctionLits();

    void annotateVariableDecls();
    void annotateCastAndNewExpr();

    void annotateBasicLiterals();

    std::vector<ClassDecl *> sortClassInheritance();
    std::vector<InterfaceDecl *> sortInterfaceInheritance();
};

}

#endif //REFLEX_SRC_TYPECHECKER_ASTTYPEANNOTATOR_H_
