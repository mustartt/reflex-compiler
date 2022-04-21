//
// Created by Admininstrator on 2022-04-19.
//

#ifndef REFLEX_SRC_CODEGEN_FUNCTIONGENERATOR_H_
#define REFLEX_SRC_CODEGEN_FUNCTIONGENERATOR_H_

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/Support/Casting.h>
#include <Type.h>
#include <LLVMTypeGenerator.h>
#include <llvm/IR/IRBuilder.h>

namespace reflex {

class FunctionGenerator {
  public:
    FunctionGenerator(llvm::LLVMContext &ctx, llvm::Module &module,
                      LLVMTypeGenerator &typeGen,
                      const std::string &name, const std::vector<std::string> &params,
                      FunctionType *type,
                      bool external = false)
        : C(ctx), M(module), TC(typeGen), entryBBuilder(ctx), builder(ctx) {

        auto funcType = llvm::dyn_cast<llvm::FunctionType>(type->createLLVMType(TC));
        Func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, name, module);

        if (external) return;

        auto entry = llvm::BasicBlock::Create(ctx, "entry", Func);
        entryBBuilder.SetInsertPoint(entry);
        auto body = llvm::BasicBlock::Create(ctx, "body", Func);
//        entryBBuilder.CreateBr(body);
        builder.SetInsertPoint(body);
        body->moveAfter(entry);

        size_t idx = 0;
        for (auto &arg: Func->args()) {
            arg.setName(params[idx]);
            createLocalVariable(std::string(arg.getName()), funcType->getParamType(idx), &arg);
            ++idx;
        }
    }

    void createLocalVariable(const std::string &name, llvm::Type *ty,
                             llvm::Value *initializer = nullptr) {
        createEntryAlloca(ty, name);
        if (initializer) storeToAlloca(name, initializer);
    }

    void createReturnValue(llvm::Value *value = nullptr) {
        if (value) builder.CreateRet(value);
        builder.CreateRetVoid();
    }

    llvm::Function *getFunction() { return Func; }

  private:
    llvm::AllocaInst *createEntryAlloca(llvm::Type *ty, const std::string &name) {
        if (symbolTable.contains(name))
            throw std::runtime_error{"Symbol already exists: " + name};
        auto alloca = entryBBuilder.CreateAlloca(ty, nullptr, name);
        symbolTable[name] = alloca;
        return alloca;
    }

    llvm::LoadInst *loadFromAlloca(const std::string &name) {
        if (!symbolTable.contains(name))
            throw std::runtime_error{"Symbol doesn't exist: " + name};
        auto alloca = symbolTable[name];
        auto ty = alloca->getAllocatedType();
        return builder.CreateLoad(ty, alloca, name);
    }

    void storeToAlloca(const std::string &name, llvm::Value *value) {
        if (!symbolTable.contains(name))
            throw std::runtime_error{"Symbol doesn't exist: " + name};
        auto alloca = symbolTable[name];
        builder.CreateStore(value, alloca);
    }
  private:
    llvm::LLVMContext &C;
    llvm::Module &M;
    LLVMTypeGenerator &TC;

    llvm::IRBuilder<> entryBBuilder;
    llvm::IRBuilder<> builder;

    llvm::Function *Func;

    std::map<std::string, llvm::AllocaInst *> symbolTable;
};

}

#endif //REFLEX_SRC_CODEGEN_FUNCTIONGENERATOR_H_
