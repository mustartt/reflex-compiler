//
// Created by henry on 2022-03-21.
//

#include "AstPrinter.h"
#include "../AstNodes.h"
#include "../Operator.h"

namespace reflex {

AstPrinter::AstPrinter(std::ostream &output) : output(output), indent(0) {}

void AstPrinter::generateIndent() {
    for (size_t i = 0; i < 2 * indent; ++i) {
        output << ' ';
    }
}

class ScopeIndent {
    size_t &indent;
  public:
    explicit ScopeIndent(size_t &ident) : indent(ident) { ++indent; }
    ~ScopeIndent() { --indent; }
};

void AstPrinter::visit(Identifier *visitable) {
    generateIndent();
    output << "Identifier: " << visitable->getIdent() << std::endl;
}
void AstPrinter::visit(ModuleSelector *visitable) {
    generateIndent();
    output << "ModuleSelector: " << visitable->getIdent() << std::endl;
}
void AstPrinter::visit(NumberLit *visitable) {
    generateIndent();
    output << "NumberLit: " << visitable->getValue() << std::endl;
}
void AstPrinter::visit(StringLit *visitable) {
    generateIndent();
    output << "StringLit: " << visitable->getValue() << std::endl;
}
void AstPrinter::visit(BoolLit *visitable) {
    generateIndent();
    output << "BoolLit: " << visitable->getValue() << std::endl;
}
void AstPrinter::visit(NullLit *visitable) {
    generateIndent();
    output << "NullLit: " << visitable->getValue() << std::endl;
}
void AstPrinter::visit(IdentifierType *visitable) {
    generateIndent();
    output << "IdentifierType: " << std::endl;
    ScopeIndent type(indent);
    visitable->getTypename()->accept(this);
}
void AstPrinter::visit(ArrayType *visitable) {
    generateIndent();
    output << "ArrayType: " << std::endl;
    ScopeIndent elem(indent);
    visitable->getElementTyp()->accept(this);
    output << std::endl;
    if (visitable->getLengthExpr())
        visitable->getLengthExpr()->accept(this);
}

void AstPrinter::visit(FunctionType *visitable) {
    generateIndent();
    output << "FunctionType: " << std::endl;
    ScopeIndent scope(indent);
    generateIndent();
    output << "Parameters: " << std::endl;
    for (auto param: visitable->getParamList()) {
        ScopeIndent paramScope(indent);
        param->accept(this);
    }
    visitable->getReturnTyp()->accept(this);
}

void AstPrinter::visit(ArrayLit *visitable) {
    generateIndent();
    output << "ArrayLit: " << std::endl;
    ScopeIndent arr(indent);
    for (auto element: visitable->getInitializerList()) {
        element->accept(this);
    }
}

void AstPrinter::visit(Parameter *visitable) {
    generateIndent();
    output << "ParamDecl: " << std::endl;
    ScopeIndent param(indent);
    visitable->getName()->accept(this);
    visitable->getTyp()->accept(this);
}

void AstPrinter::visit(FunctionLit *visitable) {
    generateIndent();
    output << "FunctionLit: " << std::endl;

    ScopeIndent param(indent);
    generateIndent();
    output << "Parameters: " << std::endl;
    for (auto decl: visitable->getParameters()) {
        ScopeIndent paramDecl(indent);
        decl->accept(this);
    }

    visitable->getReturnTyp()->accept(this);
    ScopeIndent body(indent);
    visitable->getBody()->accept(this);
}

void AstPrinter::visit(UnaryExpr *visitable) {
    generateIndent();
    output << "UnaryExpr: " << getUnaryOperator(visitable->getOp()) << std::endl;
    ScopeIndent expr(indent);
    visitable->getExpr()->accept(this);
}

void AstPrinter::visit(BinaryExpr *visitable) {
    generateIndent();
    output << "BinaryExpr: " << getBinaryOperator(visitable->getOp()) << std::endl;
    ScopeIndent expr(indent);
    visitable->getLhs()->accept(this);
    visitable->getRhs()->accept(this);
}

void AstPrinter::visit(IndexExpr *visitable) {
    generateIndent();
    output << "IndexExpr: " << std::endl;
    ScopeIndent expr(indent);
    visitable->getBaseExpr()->accept(this);
    visitable->getIndex()->accept(this);
}

void AstPrinter::visit(NewExpr *visitable) {
    generateIndent();
    output << "NewExpr: " << std::endl;
    ScopeIndent expr(indent);
    visitable->getInstanceTyp()->accept(this);
}

void AstPrinter::visit(CastExpr *visitable) {
    generateIndent();
    output << "CastExpr: " << std::endl;
    ScopeIndent expr(indent);
    visitable->getTargetTyp()->accept(this);
    visitable->getFrom()->accept(this);
}

void AstPrinter::visit(SelectorExpr *visitable) {
    generateIndent();
    output << "SelectorExpr: " << std::endl;
    ScopeIndent expr(indent);
    visitable->getBaseExpr()->accept(this);
    visitable->getSelector()->accept(this);
}
void AstPrinter::visit(ArgumentExpr *visitable) {
    generateIndent();
    output << "ArgumentExpr: " << std::endl;
    ScopeIndent expr(indent);
    visitable->getBaseExpr()->accept(this);
    ScopeIndent params(indent);
    generateIndent();
    output << "Arguments: " << std::endl;
    for (auto arg: visitable->getArguments()) {
        ScopeIndent argScope(indent);
        arg->accept(this);
    }
}

void AstPrinter::visit(VariableDecl *visitable) {
    generateIndent();
    output << "VariableDecl: " << std::endl;
    ScopeIndent expr(indent);
    visitable->getName()->accept(this);
    if (visitable->getTyp()) visitable->getTyp()->accept(this);
    if (visitable->getInitializer()) visitable->getInitializer()->accept(this);
}

void AstPrinter::visit(ReturnStmt *visitable) {
    generateIndent();
    output << "ReturnStmt: " << std::endl;
    visitable->getReturnValue()->accept(this);
}

void AstPrinter::visit(BreakStmt *visitable) {
    generateIndent();
    output << "BreakStmt" << std::endl;
}

void AstPrinter::visit(ContinueStmt *visitable) {
    generateIndent();
    output << "ContinueStmt" << std::endl;
}
void AstPrinter::visit(IfStmt *visitable) {

}
void AstPrinter::visit(ForRangeClause *visitable) {

}
void AstPrinter::visit(ForNormalClause *visitable) {

}
void AstPrinter::visit(ForStmt *visitable) {

}
void AstPrinter::visit(WhileStmt *visitable) {

}
void AstPrinter::visit(EmptyStmt *visitable) {
    generateIndent();
    output << "EmptyStmt" << std::endl;
}

void AstPrinter::visit(AssignmentStmt *visitable) {
    generateIndent();
    output << "AssignmentStmt: "
           << getAssignOperator(visitable->getAssignOp())
           << std::endl;
    ScopeIndent scope(indent);
    visitable->getLhs()->accept(this);
    visitable->getRhs()->accept(this);
}

void AstPrinter::visit(IncDecStmt *visitable) {
    generateIndent();
    output << "IncDecStmt: "
           << getPostfixOperator(visitable->getPostfixOp())
           << std::endl;
    ScopeIndent scope(indent);
    visitable->getExpr()->accept(this);
}

void AstPrinter::visit(ExpressionStmt *visitable) {
    generateIndent();
    output << "ExpressionStmt: " << std::endl;
    ScopeIndent scope(indent);
    visitable->getExpr()->accept(this);
}

void AstPrinter::visit(Block *visitable) {
    generateIndent();
    output << "Block: " << std::endl;
    ScopeIndent scope(indent);
    for (auto stmt: visitable->getStmts()) {
        stmt->accept(this);
    }
}

}
