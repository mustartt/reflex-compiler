//
// Created by henry on 2022-03-21.
//

#include "Operator.h"

namespace reflex {

BinaryOperator createBinaryOperatorFromToken(const Token &token) {
    switch (token.getTokenType().getValue()) {
        case TokenType::Or: return BinaryOperator::Or;
        case TokenType::And: return BinaryOperator::And;
        case TokenType::Compare: return BinaryOperator::Compare;
        case TokenType::CompareNot: return BinaryOperator::CompareNot;
        case TokenType::LAngleBracket: return BinaryOperator::Less;
        case TokenType::RAngleBracket: return BinaryOperator::Greater;
        case TokenType::CompareLessEqual: return BinaryOperator::CompareLessThanEqual;
        case TokenType::CompareGreaterEqual: return BinaryOperator::CompareGreaterThanEqual;
        case TokenType::Add: return BinaryOperator::Add;
        case TokenType::Sub: return BinaryOperator::Sub;
        case TokenType::LogicalOr: return BinaryOperator::LogicalOr;
        case TokenType::Star: return BinaryOperator::Mult;
        case TokenType::Div: return BinaryOperator::Div;
        case TokenType::Mod: return BinaryOperator::Mod;
        case TokenType::LogicalAnd: return BinaryOperator::LogicalAnd;
        default: throw InvalidOperator("Invalid binary operator.");
    }
}

UnaryOperator createUnaryOperatorFromToken(const Token &tok) {
    switch (tok.getTokenType().getValue()) {
        case TokenType::LogicalNot: return UnaryOperator::LogicalNot;
        case TokenType::Sub: return UnaryOperator::Negative;
        default: throw InvalidOperator("Invalid unary operator.");
    }
}

}
