//
// Created by henry on 2022-03-27.
//

#include "RecordTypeCodeGen.h"

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

#include "AstNodes.h"

namespace reflex {

void RecordTypeCodeGen::test() {
    llvm::LLVMContext TheContext;
    llvm::IRBuilder<> Builder(TheContext);
    llvm::Module TheModule("test_module.reflex", TheContext);

    ClassType *classType = typeContext.getClassTyp("Object").value_or(nullptr);
    auto allInheritedMembers = classType->getAllInheritedMember();

    // generate vtable
    for (auto &[name, member]: allInheritedMembers) {
        if (auto method = dynamic_cast<VariableDecl *>(member->getMemberTyp())) {

        }
    }

    // generate

}

}
