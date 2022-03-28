//
// Created by henry on 2022-03-27.
//

#ifndef REFLEX_SRC_CODEGEN_CLASSINTERFACECODEGEN_H_
#define REFLEX_SRC_CODEGEN_CLASSINTERFACECODEGEN_H_

#include <memory>

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

#include <TypeContextManager.h>

namespace reflex {

class ClassInterfaceCodeGen {
    TypeContextManager *typeContext;
  public:
    explicit ClassInterfaceCodeGen(TypeContextManager *typeContext) : typeContext(typeContext) {}

    void test() {
        llvm::LLVMContext TheContext;
        llvm::IRBuilder<> Builder(TheContext);
        std::unique_ptr<llvm::Module> TheModule;


    }
};

}

#endif //REFLEX_SRC_CODEGEN_CLASSINTERFACECODEGEN_H_
