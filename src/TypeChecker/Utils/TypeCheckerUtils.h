//
// Created by henry on 2022-03-26.
//

#ifndef REFLEX_SRC_TYPECHECKER_UTILS_TYPECHECKERUTILS_H_
#define REFLEX_SRC_TYPECHECKER_UTILS_TYPECHECKERUTILS_H_

#include <ostream>

namespace reflex {

class Declaration;
class ClassDecl;
class InterfaceDecl;

bool isClassDecl(Declaration *decl);
bool isInterfaceOrClassDecl(Declaration *decl);
void printIndent(std::ostream &os, size_t indent);

}

#endif //REFLEX_SRC_TYPECHECKER_UTILS_TYPECHECKERUTILS_H_
