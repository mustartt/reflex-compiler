//
// Created by henry on 2022-03-20.
//

#include "Token.h"
#include "SourceManager.h"

namespace reflex {

Token::Token(TokenType::Value tokenType, std::string lexeme, const SourceLocation *loc)
    : tokenType(tokenType), lexeme(std::move(lexeme)), loc(loc) {}

std::string Token::getTokenTypeString() const {
    return tokenType.getTypeString();
}

std::string Token::toString() const {
    return "<\'" + tokenType.getTypeString() + "\' : \'" + lexeme +
        "\' " + loc->getStringRepr() + ">";
}

std::ostream &operator<<(std::ostream &os, const Token &token) {
    os << token.toString();
    return os;
}

bool Token::isBasicLiteral() const {
    auto val = tokenType.getValue();
    return val == TokenType::BoolLiteral
        || val == TokenType::NumberLiteral
        || val == TokenType::StringLiteral
        || val == TokenType::NullLiteral;
}

bool Token::isUnaryOp() const {
    auto val = tokenType.getValue();
    return val == TokenType::LogicalNot
        || val == TokenType::Sub;
}

int Token::getTokenPrec() const {
    auto val = tokenType.getValue();
    if (val == TokenType::Or) return 1;
    if (val == TokenType::And) return 2;
    if (val == TokenType::Compare ||
        val == TokenType::CompareNot ||
        val == TokenType::LAngleBracket ||
        val == TokenType::RAngleBracket ||
        val == TokenType::CompareLessEqual ||
        val == TokenType::CompareGreaterEqual)
        return 3;
    if (val == TokenType::Add ||
        val == TokenType::Sub ||
        val == TokenType::LogicalOr)
        return 4;
    if (val == TokenType::Star ||
        val == TokenType::Div ||
        val == TokenType::Mod ||
        val == TokenType::LogicalAnd)
        return 5;
    return 0;
}

bool Token::isBinaryOp() const {
    return getTokenPrec() > 0;
}

bool Token::isAssignment() const {
    auto val = tokenType.getValue();
    return val == TokenType::Assign
        || val == TokenType::AssignAdd
        || val == TokenType::AssignSub;
}

bool Token::isPostfixOp() const {
    auto val = tokenType.getValue();
    return val == TokenType::PostInc || val == TokenType::PostDec;
}

bool Token::isDeclaration() const {
    auto val = tokenType.getValue();
    return val == TokenType::Var
        || val == TokenType::Class
        || val == TokenType::Interface
        || val == TokenType::Annotation;
}

bool Token::isTrivial() const {
    auto val = tokenType.getValue();
    return val == TokenType::WhiteSpace
        || val == TokenType::SingleComment
        || val == TokenType::MultiComment;
}

}
