//
// Created by henry on 2022-04-30.
//

#include "gtest/gtest.h"

#include "Type.h"
#include "TypeContext.h"

#include <memory>

namespace reflex {
namespace {

class InterfaceTypeTest : public ::testing::Test {
  protected:
    TypeContext typeContext;
    FunctionType *funcType = typeContext.getFunctionType(typeContext.getVoidType(), {});
    FunctionType *otherFuncType = typeContext.getFunctionType(typeContext.getVoidType(),
                                                              {typeContext.getBuiltinType(BuiltinType::Boolean)});
};

TEST_F(InterfaceTypeTest, AddMethod) {
    auto interface = typeContext.getInterfaceType("IA");
    auto memberType = typeContext.getMemberType(Visibility::Public, interface, funcType);
    auto otherMemberType = typeContext.getMemberType(Visibility::Public, interface, otherFuncType);

    EXPECT_NO_THROW(interface->addMethod("method", memberType));
    EXPECT_NO_THROW(interface->addMethod("method2", memberType));

    EXPECT_THROW(interface->addMethod("method", memberType), TypeError);

    EXPECT_TRUE(interface->getMethods().contains("method"));
    EXPECT_TRUE(interface->getMethods().contains("method2"));
}

TEST_F(InterfaceTypeTest, AddMethodShadowing) {
    auto IA = typeContext.getInterfaceType("IA");
    auto IB = typeContext.getInterfaceType("IB");
    ASSERT_NO_THROW(IB->addInterface(IA));

    auto memberType1 = typeContext.getMemberType(Visibility::Public, IA, funcType);
    auto memberType2 = typeContext.getMemberType(Visibility::Public, IB, funcType);
    auto memberType3 = typeContext.getMemberType(Visibility::Public, IB, otherFuncType);

    EXPECT_NO_THROW(IA->addMethod("method", memberType1));
    EXPECT_THROW(IB->addMethod("method", memberType3), TypeError);
    EXPECT_NO_THROW(IB->addMethod("method", memberType2));
}

TEST_F(InterfaceTypeTest, DiamondInheritance) {
    auto IA = typeContext.getInterfaceType("IA");
    auto IB = typeContext.getInterfaceType("IB");
    auto IC = typeContext.getInterfaceType("IC");
    auto ID = typeContext.getInterfaceType("ID");

    EXPECT_NO_THROW(IB->addInterface(IA));
    EXPECT_NO_THROW(IC->addInterface(IA));
    EXPECT_NO_THROW(ID->addInterface(IB));
    EXPECT_NO_THROW(ID->addInterface(IC));
}

TEST_F(InterfaceTypeTest, SelfInheritance) {
    auto IA = typeContext.getInterfaceType("IA");
    EXPECT_THROW(IA->addInterface(IA), TypeError);
}

TEST_F(InterfaceTypeTest, CyclicInheritance) {
    auto IA = typeContext.getInterfaceType("IA");
    auto IB = typeContext.getInterfaceType("IB");
    auto IC = typeContext.getInterfaceType("IC");

    ASSERT_NO_THROW(IB->addInterface(IA));
    ASSERT_NO_THROW(IA->addInterface(IC));

    EXPECT_THROW(IC->addInterface(IB), TypeError);
}

TEST_F(InterfaceTypeTest, InterfaceTrait) {
    auto IA = typeContext.getInterfaceType("IA");
    auto IB = typeContext.getInterfaceType("IB");
    auto IC = typeContext.getInterfaceType("IC");
    auto ID = typeContext.getInterfaceType("ID");

    auto MA = typeContext.getMemberType(Visibility::Public, IA, funcType);
    ASSERT_NO_THROW(IA->addMethod("methodA", MA));
    auto MB = typeContext.getMemberType(Visibility::Public, IB, funcType);
    ASSERT_NO_THROW(IB->addMethod("methodB", MB));
    auto MC = typeContext.getMemberType(Visibility::Public, IC, funcType);
    ASSERT_NO_THROW(IC->addMethod("methodC", MC));
    auto MD = typeContext.getMemberType(Visibility::Public, ID, funcType);
    ASSERT_NO_THROW(ID->addMethod("methodD", MD));

    ASSERT_NO_THROW(IB->addInterface(IA));
    ASSERT_NO_THROW(IC->addInterface(IA));
    ASSERT_NO_THROW(ID->addInterface(IB));
    ASSERT_NO_THROW(ID->addInterface(IC));

    auto traits = ID->getInterfaceTraits();
    EXPECT_EQ(traits.size(), 4);
    EXPECT_EQ(std::count_if(traits.begin(), traits.end(),
                            [](const auto &method) {
                              return method.first == "methodA";
                            }), 1);
}

}
}
