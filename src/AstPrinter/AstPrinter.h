//
// Created by henry on 2022-03-21.
//

#ifndef REFLEX_SRC_AST_VISITOR_ASTPRINTER_H_
#define REFLEX_SRC_AST_VISITOR_ASTPRINTER_H_

#include "../Ast/AstVisitor.h"
#include <ostream>
#include <vector>
#include <stack>

namespace reflex {

class AstPrinter;
class Scope {
    AstPrinter &printer;
  public:
    explicit Scope(AstPrinter &printer, bool isLast = false);
    ~Scope();
    friend class AstPrinter;
};

class Type;
class AstPrinter : public AstVisitor {
    std::ostream &output;
    size_t depth;
    std::vector<bool> depthFlag;
    std::stack<bool> isLast;
  public:
    explicit AstPrinter(std::ostream &output);

    void visit(IndexExpr *visitable) override;
    void visit(Identifier *visitable) override;
    void visit(ModuleSelector *visitable) override;
    void visit(NumberLit *visitable) override;
    void visit(StringLit *visitable) override;
    void visit(BoolLit *visitable) override;
    void visit(NullLit *visitable) override;
    void visit(IdentifierTypeExpr *visitable) override;
    void visit(ArrayTypeExpr *visitable) override;
    void visit(FunctionTypeExpr *visitable) override;
    void visit(ArrayLit *visitable) override;
    void visit(ParamDecl *visitable) override;
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
    void visit(InterfaceDecl *visitable) override;
    void visit(CompilationUnit *visitable) override;

    friend class Scope;
  private:
    void printTreePrefix();
    void printNodePrefix(const std::string &message, bool end = true);
    std::string printAstType(Type *type);
};

}

#endif //REFLEX_SRC_AST_VISITOR_ASTPRINTER_H_
