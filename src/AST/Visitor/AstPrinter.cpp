//
// Created by henry on 2022-03-21.
//

#include "AstPrinter.h"
#include "../AstNodes.h"

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
    ++indent;
    visitable->getElementTyp()->accept(this);
    output << std::endl;
    if (visitable->getLengthExpr())
        visitable->getLengthExpr()->accept(this);
    --indent;
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

}
void AstPrinter::visit(Parameter *visitable) {

}
void AstPrinter::visit(FunctionLit *visitable) {

}
void AstPrinter::visit(UnaryExpr *visitable) {

}
void AstPrinter::visit(BinaryExpr *visitable) {

}
void AstPrinter::visit(NewExpr *visitable) {

}
void AstPrinter::visit(CastExpr *visitable) {

}
void AstPrinter::visit(SelectorExpr *visitable) {

}
void AstPrinter::visit(ArgumentExpr *visitable) {

}
void AstPrinter::visit(VariableDecl *visitable) {

}
void AstPrinter::visit(ReturnStmt *visitable) {

}
void AstPrinter::visit(BreakStmt *visitable) {

}
void AstPrinter::visit(ContinueStmt *visitable) {

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

}
void AstPrinter::visit(AssignmentStmt *visitable) {

}
void AstPrinter::visit(IncDecStmt *visitable) {

}
void AstPrinter::visit(ExpressionStmt *visitable) {

}
void AstPrinter::visit(Block *visitable) {

}

}
