//
// Created by Admininstrator on 2022-04-19.
//

#include "LLVMTypeGenerator.h"

namespace reflex {

llvm::Type *LLVMTypeGenerator::getFloatingType() {
    return llvm::Type::getDoubleTy(ctx);
}

llvm::IntegerType *LLVMTypeGenerator::getBooleanType() {
    return llvm::IntegerType::getInt1Ty(ctx);
}

llvm::IntegerType *LLVMTypeGenerator::getCharacterType() {
    return llvm::IntegerType::getInt8Ty(ctx);
}

llvm::IntegerType *LLVMTypeGenerator::getIntegerType() {
    return llvm::IntegerType::getInt64Ty(ctx);
}

llvm::FunctionType *LLVMTypeGenerator::getFunctionType(const std::vector<llvm::Type *> &paramTypes,
                                                       llvm::Type *returnType) {
    return llvm::FunctionType::get(returnType, paramTypes, false);
}

llvm::StructType *LLVMTypeGenerator::getArrayType(llvm::Type *elementType) {
    std::vector<llvm::Type *> arrayStructTypes;
    arrayStructTypes.push_back(getIntegerType());
    arrayStructTypes.push_back(getPointerType(elementType));
    return llvm::StructType::get(ctx, arrayStructTypes);
}

llvm::Type *LLVMTypeGenerator::getPointerType(llvm::Type *baseType) {
    return llvm::PointerType::get(baseType, 0);
}

llvm::Type *LLVMTypeGenerator::getLLVMBasicType(PrimType *type) {
    switch (type->getBaseTyp()) {
        case PrimType::Integer: return getIntegerType();
        case PrimType::Number: return getFloatingType();
        case PrimType::Character: return getCharacterType();
        case PrimType::Boolean: return getBooleanType();
        default: break;
    }
    throw std::runtime_error{"Invalid type conversion: " + type->getTypeString()};
}

llvm::Type *LLVMTypeGenerator::getVoidType() {
    return llvm::Type::getVoidTy(ctx);
}

llvm::PointerType *LLVMTypeGenerator::getOpaquePtrType() {
    return llvm::PointerType::get(ctx, 0);
}

llvm::StructType *LLVMTypeGenerator::getInterfaceType(InterfaceType *interfaceType) {
    auto name = "interface." + interfaceType->getName();
    if (namedSymbolTable.contains(name)) return namedSymbolTable[name];
    auto vtabPointerType = getOpaquePtrType();
    return llvm::StructType::create(ctx, {vtabPointerType}, name);
}

void LLVMTypeGenerator::predeclareClassType(ClassType *classType) {
    auto name = "class." + classType->getName();
    if (namedSymbolTable.contains(name)) return;
    namedSymbolTable[name] = llvm::StructType::create(ctx, name);
}

llvm::StructType *LLVMTypeGenerator::getClassType(ClassType *classType, llvm::Type *vtable,
                                                  const std::vector<llvm::Type *> &memberLayout) {
    auto name = "class." + classType->getName();
    std::vector<llvm::Type *> classLayout{memberLayout};
    classLayout.insert(classLayout.begin(), vtable);
    namedSymbolTable[name]->setBody(classLayout);
    return namedSymbolTable[name];
}

llvm::StructType *LLVMTypeGenerator::getClassVtabLayoutType(ClassType *classType,
                                                            const std::vector<llvm::FunctionType *> &methods) {
    auto name = "vtable." + classType->getName();
    std::vector<llvm::Type *> vtabLayout{getOpaquePtrType(), getOpaquePtrType(), getOpaquePtrType()};
    for (auto methodType: methods) {
        vtabLayout.push_back(getPointerType(methodType));
    }
    auto layout = llvm::StructType::create(ctx, vtabLayout, name);
    namedSymbolTable[name] = layout;
    return layout;
}

}
