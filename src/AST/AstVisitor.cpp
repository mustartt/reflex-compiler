//
// Created by henry on 2022-03-24.
//

#include "AstVisitor.h"
#include "AstNodes.h"

namespace reflex {

void AstVisitor::visit(TypeExpr *visitable) {
    if (auto ptr = dynamic_cast<IdentifierType *>(visitable); ptr) {
        ptr->accept(this);
        return;
    }
    if (auto ptr = dynamic_cast<ArrayType *>(visitable); ptr) {
        ptr->accept(this);
        return;
    }
    if (auto ptr = dynamic_cast<FunctionType *>(visitable); ptr) {
        ptr->accept(this);
        return;
    }
    throw std::runtime_error("Unknown AST TypeExpr.");
}

}
