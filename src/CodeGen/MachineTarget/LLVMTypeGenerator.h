//
// Created by Admininstrator on 2022-04-19.
//

#ifndef REFLEX_SRC_CODEGEN_LLVMTYPEGENERATOR_H_
#define REFLEX_SRC_CODEGEN_LLVMTYPEGENERATOR_H_

#include <map>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Constants.h>
#include <Type.h>

namespace reflex {

class LLVMTypeGenerator {
  public:
    explicit LLVMTypeGenerator(llvm::LLVMContext &context)
        : ctx(context) {}

    llvm::PointerType *getOpaquePtrType();
    llvm::Type *getVoidType();
    llvm::Type *getLLVMBasicType(PrimType *type);
    static llvm::Type *getPointerType(llvm::Type *baseType);
    llvm::StructType *getArrayType(llvm::Type *elementType);
    static llvm::FunctionType *getFunctionType(const std::vector<llvm::Type *> &paramTypes,
                                               llvm::Type *returnType);
    llvm::StructType *getInterfaceType(InterfaceType *interfaceType);
    llvm::StructType *getClassType(ClassType *classType, llvm::Type *vtable,
                                   const std::vector<llvm::Type *> &memberLayout);
    void predeclareClassType(ClassType *classType);
    llvm::StructType *getClassVtabLayoutType(ClassType *classType,
                                             const std::vector<llvm::FunctionType *> &methods);
    std::optional<llvm::StructType *> getClassType(ClassType *classType) const {
        auto name = "class." + classType->getName();
        if (!namedSymbolTable.contains(name)) return std::nullopt;
        return {namedSymbolTable.at(name)};
    }

  protected:
    llvm::IntegerType *getIntegerType();
    llvm::IntegerType *getCharacterType();
    llvm::IntegerType *getBooleanType();
    llvm::Type *getFloatingType();

  private:
    llvm::LLVMContext &ctx;
    std::map<std::string, llvm::StructType *> namedSymbolTable{};
};

}

#endif //REFLEX_SRC_CODEGEN_LLVMTYPEGENERATOR_H_
