//
// Created by henry on 2022-03-25.
//

#include "AstSemanticChecker.h"
#include "AstNodes.h"

namespace reflex {

AstSemanticChecker::AstSemanticChecker(AstContextManager *astContext, TypeContextManager *typeContext) : astContext(
    astContext), typeContext(typeContext) {}

// sets up the global symbol table
void AstSemanticChecker::visit(CompilationUnit *visitable) {
    symbolTable.emplace();
    auto &table = symbolTable.top();
    for (auto decl: visitable->getDecls()) {
        if (auto var = dynamic_cast<VariableDecl *>(decl)) {
            table.addScopeMember(var->getName()->getIdent(), var->getTyp());
        } else if (auto func = dynamic_cast<FunctionDecl *>(decl)) {
            table.addScopeMember(func->getName()->getIdent(), func->getTyp());
        } else if (auto cls = dynamic_cast<ClassDecl *>(decl)) {
            table.addScopeMember(cls->getClassname(), cls->getTyp());
        } else if (auto inf = dynamic_cast<InterfaceDecl *>(decl)) {
            table.addScopeMember(inf->getInterfaceName(), inf->getTyp());
        }
    }

    for (auto decl: visitable->getDecls()) {
        decl->accept(this);
    }
}

void AstSemanticChecker::visit(VariableDecl *visitable) {
    if (visitable->getInitializer()) {
        visitable->getInitializer()->accept(this);
        auto initTyp = typeStack.top();
        typeStack.pop();
        if (initTyp != visitable->getTyp()) throw SemanticError{"VarDecl initializer type mismatch"};
    }
}

void AstSemanticChecker::visit(BinaryExpr *visitable) {
    visitable->getLhs()->accept(this);
    auto lhsTyp = typeStack.top();
    typeStack.pop();

    visitable->getRhs()->accept(this);
    auto rhsTyp = typeStack.top();
    typeStack.pop();

    // todo: implement implicit conversion
    if (rhsTyp != lhsTyp) throw SemanticError{"BinaryExpr operand type mismatch"};
    visitable->setTyp(lhsTyp);
    typeStack.push(lhsTyp);
}

void AstSemanticChecker::visit(Identifier *visitable) {
    auto &table = symbolTable.top();
    auto identifier = visitable->getIdent();
    auto admissibleTypes = table.getReferencedType(identifier);
    if (admissibleTypes.empty()) throw SemanticError{"Undefined reference to " + identifier};
    auto type = *admissibleTypes.begin();
    visitable->setTyp(type);
    typeStack.push(type);
}

}
