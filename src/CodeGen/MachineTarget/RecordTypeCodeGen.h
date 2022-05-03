//
// Created by henry on 2022-03-27.
//

#ifndef REFLEX_SRC_CODEGEN_RECORDTYPECODEGEN_H_
#define REFLEX_SRC_CODEGEN_RECORDTYPECODEGEN_H_

#include <memory>
#include <iostream>
#include <vector>

#include <TypeContextManager.h>

namespace reflex {

class RecordTypeCodeGen {
    TypeContextManager &typeContext;

  public:
    explicit RecordTypeCodeGen(TypeContextManager &typeContext)
        : typeContext(typeContext) {}

    void test();
};

}

#endif //REFLEX_SRC_CODEGEN_RECORDTYPECODEGEN_H_
