//
// Created by henry on 2022-03-24.
//

#include "AstVisitor.h"
#include "AstNodes.h"

namespace reflex {

void AstVisitor::visit(TypeExpr *visitable) {
    if (auto ptr = dynamic_cast<IdentifierTypeExpr *>(visitable); ptr) {
        ptr->accept(this);
        return;
    }
    if (auto ptr = dynamic_cast<ArrayTypeExpr *>(visitable); ptr) {
        ptr->accept(this);
        return;
    }
    if (auto ptr = dynamic_cast<FunctionTypeExpr *>(visitable); ptr) {
        ptr->accept(this);
        return;
    }
    throw std::runtime_error("Unknown AST TypeExpr.");
}

}
