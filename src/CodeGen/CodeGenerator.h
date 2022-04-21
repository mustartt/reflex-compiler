//
// Created by Admininstrator on 2022-04-19.
//

#ifndef REFLEX_SRC_CODEGEN_CODEGENERATOR_H_
#define REFLEX_SRC_CODEGEN_CODEGENERATOR_H_

#include <vector>
#include <iostream>

#include <AstNodes.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Constants.h>
#include <Type.h>
#include <TypeContextManager.h>

#include "LLVMTypeGenerator.h"
#include "FunctionGenerator.h"

namespace reflex {

class CodeGenerator {
  public:
    CodeGenerator() : ctx(), module("test_module", ctx) {}



  private:
    llvm::LLVMContext ctx;
    llvm::Module module;
};

}

#endif //REFLEX_SRC_CODEGEN_CODEGENERATOR_H_
