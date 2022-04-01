//
// Created by henry on 2022-03-27.
//

#ifndef REFLEX_SRC_CODEGEN_CLASSINTERFACECODEGEN_H_
#define REFLEX_SRC_CODEGEN_CLASSINTERFACECODEGEN_H_

#include <memory>
#include <iostream>
#include <vector>

#include <TypeContextManager.h>

namespace reflex {

class ClassInterfaceCodeGen {
    TypeContextManager *typeContext;
  public:
    explicit ClassInterfaceCodeGen(TypeContextManager *typeContext) : typeContext(typeContext) {}

    void test() {

    }
};

}

#endif //REFLEX_SRC_CODEGEN_CLASSINTERFACECODEGEN_H_
