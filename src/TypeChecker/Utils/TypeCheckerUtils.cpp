//
// Created by henry on 2022-03-26.
//

#include "TypeCheckerUtils.h"

#include "AstNodes.h"

namespace reflex {

bool isClassDecl(Declaration *decl) {
    return typeid(*decl) == typeid(ClassDecl);
}

bool isInterfaceOrClassDecl(Declaration *decl) {
    return typeid(*decl) == typeid(ClassDecl)
        || typeid(*decl) == typeid(InterfaceDecl);
}

void printIndent(std::ostream &os, size_t indent) {
    for (size_t i = 0; i < indent; ++i) os << "  ";
}

}
