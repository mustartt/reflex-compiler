//
// Created by henry on 2022-03-21.
//

#ifndef REFLEX_SRC_AST_ASTVISITOR_H_
#define REFLEX_SRC_AST_ASTVISITOR_H_

namespace reflex {

class Identifier;
class ModuleSelector;
class NumberLit;
class StringLit;
class BoolLit;
class NullLit;
class IdentifierTypeExpr;
class ArrayTypeExpr;
class FunctionTypeExpr;
class ArrayLit;
class ParamDecl;
class FunctionLit;
class UnaryExpr;
class BinaryExpr;
class NewExpr;
class CastExpr;
class SelectorExpr;
class IndexExpr;
class ArgumentExpr;
class Statement;
class SimpleStmt;
class VariableDecl;
class ReturnStmt;
class BreakStmt;
class ContinueStmt;
class IfStmt;
class ForClause;
class ForRangeClause;
class ForNormalClause;
class ForStmt;
class WhileStmt;
class EmptyStmt;
class AssignmentStmt;
class IncDecStmt;
class ExpressionStmt;
class Block;
class FunctionDecl;
class ClassDecl;
class MemberDecl;
class InterfaceDecl;
class CompilationUnit;
class TypeExpr;

class AstVisitor {
  public:
    virtual ~AstVisitor() = default;

    virtual void visit(TypeExpr *visitable);
    virtual void visit(Identifier *visitable) {}
    virtual void visit(ModuleSelector *visitable) {}
    virtual void visit(NumberLit *visitable) {}
    virtual void visit(StringLit *visitable) {}
    virtual void visit(BoolLit *visitable) {}
    virtual void visit(NullLit *visitable) {}
    virtual void visit(IdentifierTypeExpr *visitable) {}
    virtual void visit(ArrayTypeExpr *visitable) {}
    virtual void visit(FunctionTypeExpr *visitable) {}
    virtual void visit(ArrayLit *visitable) {}
    virtual void visit(ParamDecl *visitable) {}
    virtual void visit(FunctionLit *visitable) {}
    virtual void visit(UnaryExpr *visitable) {}
    virtual void visit(BinaryExpr *visitable) {}
    virtual void visit(NewExpr *visitable) {}
    virtual void visit(CastExpr *visitable) {}
    virtual void visit(SelectorExpr *visitable) {}
    virtual void visit(IndexExpr *visitable) {}
    virtual void visit(ArgumentExpr *visitable) {}
    virtual void visit(VariableDecl *visitable) {}
    virtual void visit(ReturnStmt *visitable) {}
    virtual void visit(BreakStmt *visitable) {}
    virtual void visit(ContinueStmt *visitable) {}
    virtual void visit(IfStmt *visitable) {}
    virtual void visit(ForRangeClause *visitable) {}
    virtual void visit(ForNormalClause *visitable) {}
    virtual void visit(ForStmt *visitable) {}
    virtual void visit(WhileStmt *visitable) {}
    virtual void visit(EmptyStmt *visitable) {}
    virtual void visit(AssignmentStmt *visitable) {}
    virtual void visit(IncDecStmt *visitable) {}
    virtual void visit(ExpressionStmt *visitable) {}
    virtual void visit(Block *visitable) {}
    virtual void visit(FunctionDecl *visitable) {}
    virtual void visit(ClassDecl *visitable) {}
    virtual void visit(MemberDecl *visitable) {}
    virtual void visit(InterfaceDecl *visitable) {}
    virtual void visit(CompilationUnit *visitable) {}
};

}

#endif //REFLEX_SRC_AST_ASTVISITOR_H_
