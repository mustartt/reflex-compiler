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
class IdentifierType;
class ArrayType;
class FunctionType;
class ArrayLit;
class Parameter;
class FunctionLit;
class UnaryExpr;
class BinaryExpr;
class NewExpr;
class CastExpr;
class SelectorExpr;
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

class AstVisitor {
  public:
    virtual void visit(Identifier *visitable) = 0;
    virtual void visit(ModuleSelector *visitable) = 0;
    virtual void visit(NumberLit *visitable) = 0;
    virtual void visit(StringLit *visitable) = 0;
    virtual void visit(BoolLit *visitable) = 0;
    virtual void visit(NullLit *visitable) = 0;
    virtual void visit(IdentifierType *visitable) = 0;
    virtual void visit(ArrayType *visitable) = 0;
    virtual void visit(FunctionType *visitable) = 0;
    virtual void visit(ArrayLit *visitable) = 0;
    virtual void visit(Parameter *visitable) = 0;
    virtual void visit(FunctionLit *visitable) = 0;
    virtual void visit(UnaryExpr *visitable) = 0;
    virtual void visit(BinaryExpr *visitable) = 0;
    virtual void visit(NewExpr *visitable) = 0;
    virtual void visit(CastExpr *visitable) = 0;
    virtual void visit(SelectorExpr *visitable) = 0;
    virtual void visit(ArgumentExpr *visitable) = 0;
    virtual void visit(VariableDecl *visitable) = 0;
    virtual void visit(ReturnStmt *visitable) = 0;
    virtual void visit(BreakStmt *visitable) = 0;
    virtual void visit(ContinueStmt *visitable) = 0;
    virtual void visit(IfStmt *visitable) = 0;
    virtual void visit(ForRangeClause *visitable) = 0;
    virtual void visit(ForNormalClause *visitable) = 0;
    virtual void visit(ForStmt *visitable) = 0;
    virtual void visit(WhileStmt *visitable) = 0;
    virtual void visit(EmptyStmt *visitable) = 0;
    virtual void visit(AssignmentStmt *visitable) = 0;
    virtual void visit(IncDecStmt *visitable) = 0;
    virtual void visit(ExpressionStmt *visitable) = 0;
    virtual void visit(Block *visitable) = 0;

};

}

#endif //REFLEX_SRC_AST_ASTVISITOR_H_
