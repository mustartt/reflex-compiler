//
// Created by henry on 2022-05-03.
//

#ifndef REFLEX_SRC_CODEGEN_JSTARGET_CODEGENJSTARGET_H_
#define REFLEX_SRC_CODEGEN_JSTARGET_CODEGENJSTARGET_H_

#include <ostream>
#include "CodeGenTarget.h"

namespace reflex {

class TypeContext;
class ASTContext;
class CompilationUnit;

class CodeGenJSTarget : public CodeGenTarget {
  public:
    CodeGenJSTarget(TypeContext &typeContext, ASTContext &astContext, CompilationUnit *astRoot, std::ostream &os)
        : typeContext(typeContext), astContext(astContext), astRoot(astRoot), os(os) {}

    void CodeGen() override;

  private:
    TypeContext &typeContext;
    ASTContext &astContext;
    CompilationUnit *astRoot;
    std::ostream &os;
};

} // reflex

#endif //REFLEX_SRC_CODEGEN_JSTARGET_CODEGENJSTARGET_H_
