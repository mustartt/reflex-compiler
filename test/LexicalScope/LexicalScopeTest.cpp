//
// Created by henry on 2022-04-28.
//

#include <filesystem>
#include "LexicalContext.h"

#include "gtest/gtest.h"

#include "SourceManager.h"
#include "ASTContext.h"
#include "Lexer.h"
#include "Parser.h"
#include "TypeContext.h"
#include "LexicalContextForwardPass.h"

namespace reflex {
namespace {

class LexicalScopeTest : public ::testing::Test {
  protected:
    SourceManager srcManager;
    ASTContext astContext;
    std::unique_ptr<Lexer> lexer;
    std::unique_ptr<Parser> parser;
    CompilationUnit *root;
    LexicalContext lexicalContext;
    TypeContext typeContext;
    LexicalScope *globalScope;

    virtual void SetUp() {
        auto &file = srcManager.open("TestFiles/ScopeTest.reflex.test");
        lexer = std::make_unique<Lexer>(file, file.content(),
                                        getTokenDescription(), getKeywordDescription());
        parser = std::make_unique<Parser>(astContext, *lexer);
        root = parser->parseCompilationUnit();

        LexicalContextForwardPass forwardPass(lexicalContext, typeContext);
        globalScope = forwardPass.performPass(root);
    }
};

ScopeMember *getReferencedScope(LexicalScope *scope,
                                const std::string &prefix, const std::string &rest) {
    auto start = scope->bind(prefix);
    auto end = start->follow(rest);
    return end;
}

TEST_F(LexicalScopeTest, SanityCheck) {
    ASSERT_NO_THROW(getReferencedScope(globalScope, "A", "B::C"));
}

TEST_F(LexicalScopeTest, QualifyFromGlobalScope) {
    ASSERT_NO_THROW(globalScope->bind("A"));
    ASSERT_NO_THROW(globalScope->bind("X"));
    auto scopeMemberA = globalScope->bind("A");
    auto scopeMemberX = globalScope->bind("X");

    ASSERT_NO_THROW(scopeMemberA->follow("E"));
    ASSERT_NO_THROW(scopeMemberA->follow("B::C"));
    auto innerScope = scopeMemberA->follow("B::C");
    EXPECT_EQ(innerScope->getMembername(), "C");
    EXPECT_EQ(innerScope->getStringQualifier(), "A::B::C");

    EXPECT_THROW(scopeMemberX->follow("Z"), LexicalError);
    EXPECT_THROW(globalScope->bind("NoneExistent"), LexicalError);

    EXPECT_NO_THROW(globalScope->resolve("A::B::C"));
}

TEST_F(LexicalScopeTest, ScopeResolveInterface) {
    EXPECT_NO_THROW(globalScope->resolve("A"));
    EXPECT_THROW(globalScope->resolve(""), LexicalError);
    EXPECT_THROW(globalScope->resolve("DoesNotExist"), LexicalError);
}

TEST_F(LexicalScopeTest, QualifyFromGlobalCorrectType) {
    ASSERT_NO_THROW(globalScope->bind("A"));
    auto scopeMemberA = globalScope->bind("A");

    ASSERT_NO_THROW(scopeMemberA->follow("B::C"));
    auto innerScope = scopeMemberA->follow("B::C");
    EXPECT_EQ(innerScope->getMemberType(), typeContext.getClassType("A::B::C"));
}

TEST_F(LexicalScopeTest, QualifyFromLeafChildren) {
    ASSERT_NO_THROW(globalScope->resolve("X::Y::Z"));
    auto member = globalScope->resolve("X::Y::Z");

    ASSERT_NO_THROW(member->getParent()->resolve("A"));
    ASSERT_NO_THROW(member->getParent()->resolve("A::B"));
    ASSERT_NO_THROW(member->getParent()->resolve("A::B::C"));
    ASSERT_NO_THROW(member->getParent()->resolve("A::B::D"));

    auto resolvedMember = member->getParent()->resolve("A::B");
    EXPECT_EQ(resolvedMember->getMembername(), "B");
}

TEST_F(LexicalScopeTest, NestedScopeResolution) {
    ASSERT_NO_THROW(globalScope->resolve("A::E"));
    auto member = globalScope->resolve("A::E");
    EXPECT_EQ(member->getMembername(), "E");

    ASSERT_NO_THROW(member->getParent()->resolve("B::C"));
    auto inner = member->getParent()->resolve("B::C");
    EXPECT_EQ(inner->getMembername(), "C");
}

TEST_F(LexicalScopeTest, ResolutionCannotBindErrorMessage) {
    try {
        globalScope->resolve("DoesNotExist");
        FAIL() << "did not throw exception";
    } catch (LexicalError &err) {
        auto msg = std::string(err.what());
        EXPECT_NE(msg.find("LexicalError"), std::string::npos);
        EXPECT_NE(msg.find("DoesNotExist"), std::string::npos);
    } catch (...) {
        FAIL() << "incorrect exception";
    }
}

TEST_F(LexicalScopeTest, ResolutionCannotFollowErrorMessage) {
    try {
        globalScope->resolve("A::B::E");
        FAIL() << "did not throw exception";
    } catch (LexicalError &err) {
        auto msg = std::string(err.what());
        EXPECT_NE(msg.find("LexicalError"), std::string::npos);
        EXPECT_NE(msg.find('E'), std::string::npos);
        EXPECT_NE(msg.find("A::B"), std::string::npos);
    } catch (...) {
        FAIL() << "incorrect exception";
    }
}

TEST_F(LexicalScopeTest, OverloadScopeMember) {
    ASSERT_NO_THROW(globalScope->resolve("A"));
    ASSERT_TRUE(globalScope->resolve("A")->getChild());
    auto scope = globalScope->resolve("A")->getChild();

    auto funcType1 = typeContext.getFunctionType(typeContext.getVoidType(), {});
    auto funcType2 = typeContext.getFunctionType(
        typeContext.getVoidType(), {typeContext.getBuiltinType(BuiltinType::Integer)});

    scope->addScopeMember("OverloadFunction", funcType1);
    EXPECT_NO_THROW(scope->addScopeMember("OverloadFunction", funcType2));

    try {
        scope->addScopeMember("OverloadFunction", funcType1);
        FAIL() << "did not throw exception";
    } catch (LexicalError &err) {
        auto msg = std::string(err.what());
        EXPECT_NE(msg.find("LexicalError"), std::string::npos);
        EXPECT_NE(msg.find("OverloadFunction"), std::string::npos);
        EXPECT_NE(msg.find('A'), std::string::npos);
    } catch (...) {
        FAIL() << "incorrect exception";
    }
}

}
}
