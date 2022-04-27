//
// Created by henry on 2022-04-27.
//

#include "Type.h"
#include "TypeContext.h"

#include "gtest/gtest.h"

namespace reflex {
namespace {

class TypeTest : public ::testing::Test {
  protected:
    TypeContext context;
};

TEST_F(TypeTest, VoidType) {
    auto type = context.getVoidType();
    EXPECT_EQ(type->getTypeString(), "void");
    EXPECT_FALSE(type->isReferenceType());
}

TEST_F(TypeTest, BuiltinType) {
    auto type1 = context.getBuiltinType(BuiltinType::Integer);
    EXPECT_EQ(type1->getTypeString(), "int");
    EXPECT_FALSE(type1->isReferenceType());
    auto type2 = context.getBuiltinType(BuiltinType::Boolean);
    EXPECT_EQ(type2->getTypeString(), "bool");
    EXPECT_FALSE(type2->isReferenceType());
}

TEST_F(TypeTest, ArrayType) {
    auto base = context.getBuiltinType(BuiltinType::Integer);
    auto arr1 = context.getArrayType(base);
    auto arr2 = context.getArrayType(base, 10);
    EXPECT_EQ(arr1->getTypeString(), "int[]");
    EXPECT_EQ(arr2->getTypeString(), "int[10]");
}

TEST_F(TypeTest, FunctionType) {
    auto func = context.getFunctionType(
        context.getVoidType(),
        {context.getBuiltinType(BuiltinType::Integer)}
    );
    EXPECT_EQ(func->getTypeString(), "func(int)->void");
    std::vector<Type *> args{context.getBuiltinType(BuiltinType::Integer)};
    EXPECT_TRUE(func->canApplyArguments(args));
    std::vector<Type *> incorrectArgs{context.getBuiltinType(BuiltinType::Boolean)};
    EXPECT_FALSE(func->canApplyArguments(incorrectArgs));
}

TEST_F(TypeTest, ReferenceType) {
    auto base = context.getArrayType(context.getBuiltinType(BuiltinType::Integer));
    auto refNullable = context.getReferenceType(base, true);
    auto refNonNullable = context.getReferenceType(base, false);
    EXPECT_EQ(refNullable->getTypeString(), "&int[]?");
    EXPECT_EQ(refNonNullable->getTypeString(), "&int[]");
    EXPECT_NE(refNullable, refNonNullable);
}

}
}
