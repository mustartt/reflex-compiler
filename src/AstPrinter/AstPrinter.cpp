//
// Created by henry on 2022-03-21.
//

#include "AstPrinter.h"
#include "../AST/AstNodes.h"
#include "../AST/Operator.h"

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
    ScopeIndent paramScope(indent);
    for (auto param: visitable->getParamList()) {
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

void AstPrinter::visit(ParamDecl *visitable) {
    generateIndent();
    output << "ParamDecl: " << std::endl;
    ScopeIndent param(indent);
    visitable->getName()->accept(this);
    visitable->getParamType()->accept(this);
}

void AstPrinter::visit(FunctionLit *visitable) {
    generateIndent();
    output << "FunctionLit: " << std::endl;
    ScopeIndent param(indent);
    for (auto decl: visitable->getParameters()) {
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
    if (visitable->getVariableType()) visitable->getVariableType()->accept(this);
    if (visitable->getInitializer()) visitable->getInitializer()->accept(this);
}

void AstPrinter::visit(ReturnStmt *visitable) {
    generateIndent();
    output << "ReturnStmt: " << std::endl;
    ScopeIndent expr(indent);
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
    generateIndent();
    output << "IfStmt: " << std::endl;
    ScopeIndent scope(indent);
    visitable->getCond()->accept(this);
    visitable->getPrimaryBlock()->accept(this);
    if (visitable->getElseBlock()) visitable->getElseBlock()->accept(this);
}

void AstPrinter::visit(ForRangeClause *visitable) {
    generateIndent();
    output << "ForRangeClause: " << std::endl;
    ScopeIndent scope(indent);
    visitable->getVariable()->accept(this);
    visitable->getIterExpr()->accept(this);
}

void AstPrinter::visit(ForNormalClause *visitable) {
    generateIndent();
    output << "ForNormalClause: " << std::endl;
    ScopeIndent scope(indent);
    visitable->getInit()->accept(this);
    visitable->getCond()->accept(this);
    visitable->getPost()->accept(this);
}

void AstPrinter::visit(ForStmt *visitable) {
    generateIndent();
    output << "ForStmt: " << std::endl;
    ScopeIndent scope(indent);
    visitable->getClause()->accept(this);
    visitable->getBody()->accept(this);
}

void AstPrinter::visit(WhileStmt *visitable) {
    generateIndent();
    output << "WhileStmt: " << std::endl;
    ScopeIndent scope(indent);
    visitable->getCond()->accept(this);
    visitable->getBody()->accept(this);
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

void AstPrinter::visit(FunctionDecl *visitable) {
    generateIndent();
    output << "FunctionDecl: " << std::endl;
    ScopeIndent scope(indent);
    visitable->getName()->accept(this);
    for (auto param: visitable->getParams()) {
        param->accept(this);
    }
    visitable->getRetTyp()->accept(this);
    if (visitable->getBody()) visitable->getBody()->accept(this);
}

void AstPrinter::visit(ClassDecl *visitable) {
    generateIndent();
    output << "ClassDecl: " << std::endl;
    ScopeIndent scope(indent);
    visitable->getName()->accept(this);
    if (visitable->getBaseclass()) visitable->getBaseclass()->accept(this);
    for (auto interface: visitable->getInterfaces()) {
        interface->accept(this);
    }
    for (auto member: visitable->getMembers()) {
        member->accept(this);
    }
}

void AstPrinter::visit(MemberDecl *visitable) {
    generateIndent();
    output << "MemberDecl: " << visitable->getModifier()->getIdent() << std::endl;
    ScopeIndent scope(indent);
    visitable->getDeclaration()->accept(this);
}

void AstPrinter::visit(InterfaceDecl *visitable) {
    generateIndent();
    output << "InterfaceDecl: " << std::endl;
    ScopeIndent scope(indent);
    visitable->getName()->accept(this);
    for (auto interface: visitable->getInterfaces()) {
        interface->accept(this);
    }
    for (auto signature: visitable->getSignatures()) {
        signature->accept(this);
    }
}

void AstPrinter::visit(CompilationUnit *visitable) {
    generateIndent();
    output << "CompilationUnit: " << std::endl;
    ScopeIndent scope(indent);
    for (auto decls: visitable->getDecls()) {
        decls->accept(this);
    }
}

}
