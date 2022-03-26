//
// Created by henry on 2022-03-25.
//

#include "Token.h"

#include "gtest/gtest.h"

namespace reflex {
namespace {

TEST(TokenTest, DefaultConstructs) {
    Token token;
    EXPECT_EQ(token.getTokenType().getValue(), TokenType::EndOfFile);
}

TEST(TokenTest, Construction) {
    Token token{TokenType::Identifier, "Identifier", {}};
    EXPECT_EQ(token.getTokenType().getValue(), TokenType::Identifier);
    EXPECT_EQ(token.getLexeme(), "Identifier");
}

}
}
