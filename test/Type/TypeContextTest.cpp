//
// Created by henry on 2022-04-27.
//

#include "gtest/gtest.h"

#include "TypeContext.h"

namespace reflex {
namespace {

class TypeContextTest : public ::testing::Test {
  protected:
    TypeContext context;
    BuiltinType *IntType = context.getBuiltinType(BuiltinType::Integer);
    AggregateDecl *AggDecl = reinterpret_cast<AggregateDecl *>(0x1);
    CompositeType *CompType = context.getClassType("TestClass", AggDecl);
};

TEST_F(TypeContextTest, VoidTypePtrIdentity) {
    auto type1 = context.getVoidType();
    auto type2 = context.getVoidType();
    EXPECT_TRUE(type1 && type2);
    EXPECT_EQ(type1, type2);
}

TEST_F(TypeContextTest, BuiltinTypePtrIdentitySame) {
    auto type1 = context.getBuiltinType(BuiltinType::Integer);
    auto type2 = context.getBuiltinType(BuiltinType::Integer);
    EXPECT_TRUE(type1 && type2);
    EXPECT_EQ(type1, type2);
}

TEST_F(TypeContextTest, BuiltinTypePtrIdentityDiff) {
    auto type1 = context.getBuiltinType(BuiltinType::Integer);
    auto type2 = context.getBuiltinType(BuiltinType::Boolean);
    EXPECT_TRUE(type1 && type2);
    EXPECT_NE(type1, type2);
}

TEST_F(TypeContextTest, ArrayTypeInvalidTypeException) {
    EXPECT_THROW(
        context.getArrayType(context.getVoidType()),
        TypeError
    );
}

TEST_F(TypeContextTest, ArrayTypePtrIdentity) {
    auto type1 = context.getArrayType(IntType);
    auto type2 = context.getArrayType(IntType);
    EXPECT_TRUE(type1 && type2);
    EXPECT_EQ(type1, type2);
    auto type3 = context.getArrayType(IntType, 4);
    auto type4 = context.getArrayType(IntType, 4);
    EXPECT_TRUE(type3 && type4);
    EXPECT_EQ(type3, type4);
}

TEST_F(TypeContextTest, ArrayTypePtrDiff) {
    auto type1 = context.getArrayType(IntType);
    auto type2 = context.getArrayType(IntType, 5);
    EXPECT_TRUE(type1 && type2);
    EXPECT_NE(type1, type2);
    auto type3 = context.getArrayType(IntType, 1);
    auto type4 = context.getArrayType(IntType, 5);
    EXPECT_TRUE(type3 && type4);
    EXPECT_NE(type3, type4);
}

TEST_F(TypeContextTest, FunctionTypePtrIdentity) {
    auto type1 = context.getFunctionType(context.getVoidType(), {IntType, IntType});
    auto type2 = context.getFunctionType(context.getVoidType(), {IntType, IntType});
    EXPECT_TRUE(type1 && type2);
    EXPECT_EQ(type1, type2);
}

TEST_F(TypeContextTest, FunctionTypePtrDiff) {
    auto type1 = context.getFunctionType(context.getVoidType(), {IntType, IntType});
    auto type2 = context.getFunctionType(context.getVoidType(), {IntType});
    EXPECT_TRUE(type1 && type2);
    EXPECT_NE(type1, type2);
    auto type3 = context.getFunctionType(context.getVoidType(), {IntType});
    auto type4 = context.getFunctionType(IntType, {IntType});
    EXPECT_TRUE(type1 && type2);
    EXPECT_NE(type1, type2);
}

TEST_F(TypeContextTest, MemberAttrTypePtrIdentity) {
    auto type1 = context.getMemberType(Visibility::Public, CompType, IntType);
    auto type2 = context.getMemberType(Visibility::Public, CompType, IntType);
    EXPECT_TRUE(type1 && type2);
    EXPECT_EQ(type1, type2);
}

TEST_F(TypeContextTest, MemberAttrTypePtrDiff) {
    auto type1 = context.getMemberType(
        Visibility::Public, CompType, IntType
    );
    auto type2 = context.getMemberType(
        Visibility::Public, CompType, context.getBuiltinType(BuiltinType::Boolean)
    );
    EXPECT_TRUE(type1 && type2);
    EXPECT_NE(type1, type2);
}

TEST_F(TypeContextTest, MemberAttrTypeException) {
    EXPECT_THROW(context.getMemberType(Visibility::Public, nullptr, IntType), TypeError);
    EXPECT_THROW(context.getMemberType(Visibility::Public, CompType, context.getVoidType()), TypeError);
    EXPECT_THROW(context.getMemberType(
        Visibility::Public, CompType,
        context.getMemberType(Visibility::Public, CompType, context.getBuiltinType(BuiltinType::Boolean))
    ), TypeError);
}

TEST_F(TypeContextTest, ClassTypeNameIdentity) {
    auto type1 = context.getClassType("Class1", AggDecl);
    auto type2 = context.getClassType("Class1", AggDecl);
    auto type3 = context.getClassType("ClassDiff", AggDecl);
    EXPECT_TRUE(type1 && type2 && type3);
    EXPECT_EQ(type1, type2);
    EXPECT_NE(type3, type2);
}

TEST_F(TypeContextTest, InterfaceTypeNameIdentity) {
    auto type1 = context.getInterfaceType("Interface1", AggDecl);
    auto type2 = context.getInterfaceType("Interface1", AggDecl);
    auto type3 = context.getInterfaceType("InterfaceDiff", AggDecl);
    EXPECT_TRUE(type1 && type2 && type3);
    EXPECT_EQ(type1, type2);
    EXPECT_NE(type3, type2);
}

TEST_F(TypeContextTest, ReferenceTypeIdentity) {
    ASSERT_FALSE(dynamic_cast<ReferenceableType *>(IntType));
    auto type1 = context.getReferenceType(CompType);
    auto type2 = context.getReferenceType(CompType);
    auto type3 = context.getReferenceType(CompType, false);
    EXPECT_TRUE(type1 && type2 && type3);
    EXPECT_EQ(type1, type2);
    EXPECT_NE(type3, type2);
}

}
}
