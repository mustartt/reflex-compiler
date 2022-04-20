//
// Created by Admininstrator on 2022-04-19.
//

#ifndef REFLEX_SRC_CODEGEN_CODEGENERATOR_H_
#define REFLEX_SRC_CODEGEN_CODEGENERATOR_H_

#include <vector>

#include <AstNodes.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Constants.h>
#include <Type.h>
#include <TypeContextManager.h>

#include "LLVMTypeGenerator.h"

namespace reflex {

class CodeGenerator {
  public:
    CodeGenerator() : ctx(), module("test_module", ctx) {}

    void generate(Type *type) {
        LLVMTypeGenerator TG(ctx);
        llvm::Type *ty = type->createLLVMType(TG);

        llvm::StructType::getTypeByName(ctx, "class.Object")->print(llvm::outs());
        llvm::StructType::getTypeByName(ctx, "vtable.Object")->print(llvm::outs());
    }

  private:
    llvm::LLVMContext ctx;
    llvm::Module module;
};

}

#endif //REFLEX_SRC_CODEGEN_CODEGENERATOR_H_
