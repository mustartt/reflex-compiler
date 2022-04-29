//
// Created by henry on 2022-04-28.
//

#include "gtest/gtest.h"

#include "LexicalContext.h"
#include "ASTContext.h"
#include "TypeContext.h"
#include "ASTStatement.h"
#include "ASTDeclaration.h"
#include "ASTLiteral.h"

#include <vector>
#include <string>

namespace reflex {

namespace {

class LexicalContextTest : public ::testing::Test {
  protected:
    LexicalContext context;
    ASTContext astContext;
    TypeContext typeContext;
    CompilationUnit *CU;
    LexicalScope *globalScope;

    void SetUp() override {
        CU = astContext.create<CompilationUnit>(nullptr, "TestCompilationUnit");
        globalScope = context.createGlobalScope(CU);
    }
};

TEST_F(LexicalContextTest, CreateGlobalScope) {
    auto global = context.createGlobalScope(CU);
    EXPECT_TRUE(global->isGlobalScope());
    EXPECT_TRUE(global->getMembers().empty());
    EXPECT_EQ(global->getNodeDecl(), CU);
    EXPECT_EQ(global->getScopename(), "__global__");
}

TEST_F(LexicalContextTest, CreateCompositeScope) {
    auto classDecl = astContext.create<ClassDecl>(
        nullptr,
        Visibility::Public,
        std::string("TestClass"),
        nullptr,
        std::vector<ReferenceTypenameExpr *>()
    );
    auto [classScope, member] = globalScope->createCompositeScope(classDecl);

    ASSERT_TRUE(classScope);
    EXPECT_EQ(classScope->getScopename(), "TestClass");
    EXPECT_EQ(classScope->getParentScope(), member);
    EXPECT_EQ(classScope->getNodeDecl(), classDecl);
    EXPECT_EQ(classScope->getScopeType(), LexicalScopeType::Composite);
    EXPECT_FALSE(classScope->isGlobalScope());

    EXPECT_TRUE(member->hasScope());
    EXPECT_EQ(member->getMembername(), "TestClass");
    EXPECT_EQ(member->getChild(), classScope);
    EXPECT_EQ(member->getParent(), globalScope);
    EXPECT_EQ(member->getStringQualifier(), "TestClass");
}

TEST_F(LexicalContextTest, CreateFunctionScope) {
    auto funcDecl = astContext.create<FunctionDecl>(
        nullptr, "TestFunction",
        std::vector<ParamDecl *>(),
        nullptr, nullptr
    );
    auto [funcScope, member] = globalScope->createFunctionScope(funcDecl);

    ASSERT_TRUE(funcScope);
    EXPECT_EQ(funcScope->getScopename(), "TestFunction");
    EXPECT_EQ(funcScope->getParentScope(), member);
    EXPECT_EQ(funcScope->getNodeDecl(), funcDecl);
    EXPECT_EQ(funcScope->getScopeType(), LexicalScopeType::Function);
    EXPECT_FALSE(funcScope->isGlobalScope());

    EXPECT_TRUE(member->hasScope());
    EXPECT_EQ(member->getMembername(), "TestFunction");
    EXPECT_EQ(member->getChild(), funcScope);
    EXPECT_EQ(member->getParent(), globalScope);
    EXPECT_EQ(member->getStringQualifier(), "TestFunction");
}

TEST_F(LexicalContextTest, CreateMethodScope) {
    auto methodDecl = astContext.create<MethodDecl>(
        nullptr, "TestMethod",
        std::vector<ParamDecl *>(),
        nullptr, nullptr, nullptr,
        Visibility::Public
    );
    auto [methodScope, member] = globalScope->createMethodScope(methodDecl);

    ASSERT_TRUE(methodScope);
    EXPECT_EQ(methodScope->getScopename(), "TestMethod");
    EXPECT_EQ(methodScope->getScopeType(), LexicalScopeType::Method);
    EXPECT_TRUE(member->hasScope());
    EXPECT_EQ(member->getMembername(), "TestMethod");
    EXPECT_EQ(member->getStringQualifier(), "TestMethod");
}

TEST_F(LexicalContextTest, CreateBlockScope) {
    auto funcDecl = astContext.create<FunctionDecl>(
        nullptr, "TestFunction",
        std::vector<ParamDecl *>(),
        nullptr, nullptr
    );
    auto [funcScope, member] = globalScope->createFunctionScope(funcDecl);

    auto blockDecl1 = astContext.create<BlockStmt>(nullptr, std::vector<Statement *>());
    auto blockDecl2 = astContext.create<BlockStmt>(nullptr, std::vector<Statement *>());

    auto [blockScope1, blockMember1] = funcScope->createBlockScope(blockDecl1);
    auto [blockScope2, blockMember2] = funcScope->createBlockScope(blockDecl2);

    ASSERT_TRUE(blockScope1 && blockScope2);
    EXPECT_EQ(blockScope1->getScopename(), "__block(0)__");
    EXPECT_EQ(blockScope2->getScopename(), "__block(1)__");
    EXPECT_EQ(blockScope1->getParentScope()->getParent(), blockScope2->getParentScope()->getParent());
    EXPECT_NO_THROW(blockScope1->bind("TestFunction"));
}

TEST_F(LexicalContextTest, CreateLambdaScope) {
    auto funcDecl = astContext.create<FunctionDecl>(
        nullptr, "TestFunction",
        std::vector<ParamDecl *>(),
        nullptr, nullptr
    );
    auto [funcScope, member] = globalScope->createFunctionScope(funcDecl);

    auto lambdaDecl1 = astContext.create<FunctionLiteral>(nullptr, std::vector<ParamDecl *>(), nullptr, nullptr);
    auto lambdaDecl2 = astContext.create<FunctionLiteral>(nullptr, std::vector<ParamDecl *>(), nullptr, nullptr);

    auto [lambdaScope1, lambdaMember1] = funcScope->createLambdaScope(lambdaDecl1);
    auto [lambdaScope2, lambdaMember2] = funcScope->createLambdaScope(lambdaDecl2);

    ASSERT_TRUE(lambdaScope1 && lambdaScope2);
    EXPECT_EQ(lambdaScope1->getScopename(), "__lambda(0)__");
    EXPECT_EQ(lambdaScope2->getScopename(), "__lambda(1)__");
    EXPECT_EQ(lambdaScope1->getParentScope()->getParent(), lambdaScope2->getParentScope()->getParent());
    EXPECT_NO_THROW(lambdaScope1->bind("TestFunction"));
}

TEST_F(LexicalContextTest, DebugDump) {
    auto funcDecl = astContext.create<FunctionDecl>(
        nullptr, "TestFunction",
        std::vector<ParamDecl *>(),
        nullptr, nullptr
    );
    auto [funcScope, member] = globalScope->createFunctionScope(funcDecl);

    std::stringstream os;
    context.dump(os, globalScope);
    EXPECT_FALSE(os.str().empty());
}

}
}
