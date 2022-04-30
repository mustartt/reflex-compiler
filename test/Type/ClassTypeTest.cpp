//
// Created by henry on 2022-04-30.
//

#include "gtest/gtest.h"

#include "Type.h"
#include "TypeContext.h"

#include <memory>

namespace reflex {
namespace {

class ClassTypeTest : public ::testing::Test {
  protected:
    TypeContext typeContext;
    BuiltinType *intType = typeContext.getBuiltinType(BuiltinType::Integer);
    FunctionType *funcType = typeContext.getFunctionType(typeContext.getVoidType(), {});
    FunctionType *otherFuncType = typeContext.getFunctionType(typeContext.getVoidType(),
                                                              {typeContext.getBuiltinType(BuiltinType::Boolean)});
};

TEST_F(ClassTypeTest, AddField) {
    auto base = typeContext.getClassType("Base");
    auto derived = typeContext.getClassType("Derived");
    ASSERT_NO_THROW(derived->setBaseclass(base));

    auto fieldType1 = typeContext.getMemberType(Visibility::Public, base, intType);
    auto methodType = typeContext.getMemberType(Visibility::Public, base, funcType);
    ASSERT_NO_THROW(base->addField("field", fieldType1));
    ASSERT_NO_THROW(base->addMethod("method", methodType));

    auto fieldType2 = typeContext.getMemberType(Visibility::Public, derived, intType);
    EXPECT_THROW(derived->addField("field", fieldType2), TypeError);
    EXPECT_THROW(derived->addField("method", fieldType2), TypeError);
}

TEST_F(ClassTypeTest, AddMethod) {
    auto base = typeContext.getClassType("Base");
    auto derived = typeContext.getClassType("Derived");
    ASSERT_NO_THROW(derived->setBaseclass(base));

    auto fieldType1 = typeContext.getMemberType(Visibility::Public, base, intType);
    auto methodType1 = typeContext.getMemberType(Visibility::Public, base, funcType);
    ASSERT_NO_THROW(base->addField("field", fieldType1));
    ASSERT_NO_THROW(base->addMethod("method", methodType1));
    ASSERT_NO_THROW(base->addMethod("method_other", methodType1));

    auto methodType2 = typeContext.getMemberType(Visibility::Public, derived, funcType);
    EXPECT_NO_THROW(derived->addMethod("method", methodType2));

    auto methodTypeMismatched = typeContext.getMemberType(Visibility::Public, derived, otherFuncType);
    EXPECT_THROW(derived->addMethod("field", methodType2), TypeError);
    EXPECT_THROW(derived->addMethod("method_other", methodTypeMismatched), TypeError);
}

TEST_F(ClassTypeTest, SelfInheritance) {
    auto klass = typeContext.getClassType("A");
    EXPECT_THROW(klass->setBaseclass(klass), TypeError);
}

TEST_F(ClassTypeTest, CyclicInheritance) {
    auto IA = typeContext.getClassType("IA");
    auto IB = typeContext.getClassType("IB");
    auto IC = typeContext.getClassType("IC");

    ASSERT_NO_THROW(IB->setBaseclass(IA));
    ASSERT_NO_THROW(IA->setBaseclass(IC));

    EXPECT_THROW(IC->setBaseclass(IB), TypeError);
}

TEST_F(ClassTypeTest, ClassTrait) {
    auto IA = typeContext.getClassType("IA");
    auto IB = typeContext.getClassType("IB");

    auto MA = typeContext.getMemberType(Visibility::Public, IA, funcType);
    ASSERT_NO_THROW(IA->addMethod("methodA", MA));
    auto MB = typeContext.getMemberType(Visibility::Public, IB, funcType);
    ASSERT_NO_THROW(IB->addMethod("methodB", MB));

    ASSERT_NO_THROW(IB->setBaseclass(IA));

    auto traits = IB->getClassImplTraits();
    EXPECT_EQ(traits.size(), 2);
    EXPECT_EQ(std::count_if(traits.begin(), traits.end(),
                            [](const auto &method) {
                              return method.first == "methodA";
                            }), 1);
}

TEST_F(ClassTypeTest, IsAbstract) {
    auto IA = typeContext.getInterfaceType("IA");
    auto MA = typeContext.getMemberType(Visibility::Public, IA, funcType);
    IA->addMethod("method", MA);

    auto A = typeContext.getClassType("A");
    A->addInterface(IA);

    EXPECT_TRUE(A->isAbstract());

    A->addMethod("method", MA);
    EXPECT_FALSE(A->isAbstract());
}

}
}
