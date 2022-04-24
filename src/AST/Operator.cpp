//
// Created by henry on 2022-03-21.
//

#include "Operator.h"

namespace reflex::Operator {

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

std::string getBinaryOperator(BinaryOperator op) {
    switch (op) {
        case BinaryOperator::Or: return "or";
        case BinaryOperator::And: return "and";
        case BinaryOperator::Compare: return "==";
        case BinaryOperator::CompareNot: return "!=";
        case BinaryOperator::Less: return "<";
        case BinaryOperator::Greater: return ">";
        case BinaryOperator::CompareLessThanEqual: return "<=";
        case BinaryOperator::CompareGreaterThanEqual: return ">=";
        case BinaryOperator::Add: return "+";
        case BinaryOperator::Sub: return "-";
        case BinaryOperator::LogicalOr: return "|";
        case BinaryOperator::Mult: return "*";
        case BinaryOperator::Div: return "/";
        case BinaryOperator::Mod: return "%";
        case BinaryOperator::LogicalAnd: return "&";
    }
}

UnaryOperator createUnaryOperatorFromToken(const Token &tok) {
    switch (tok.getTokenType().getValue()) {
        case TokenType::LogicalNot: return UnaryOperator::LogicalNot;
        case TokenType::Sub: return UnaryOperator::Negative;
        default: throw InvalidOperator("Invalid unary operator.");
    }
}

std::string getUnaryOperator(UnaryOperator op) {
    switch (op) {
        case UnaryOperator::Negative: return "-";
        case UnaryOperator::LogicalNot: return "!";
    }
}

AssignOperator createAssignOperatorFromToken(const Token &tok) {
    switch (tok.getTokenType().getValue()) {
        case TokenType::Assign: return AssignOperator::Equal;
        case TokenType::AssignAdd: return AssignOperator::AddEqual;
        case TokenType::AssignSub: return AssignOperator::SubEqual;
        default: throw InvalidOperator("Invalid assignment operator.");
    }
}

std::string getAssignOperator(AssignOperator op) {
    switch (op) {
        case AssignOperator::Equal: return "=";
        case AssignOperator::AddEqual: return "+=";
        case AssignOperator::SubEqual: return "-=";
        default: throw InvalidOperator("Invalid assignment operator.");
    }
}

PostfixOperator createPostfixOperatorFromToken(const Token &tok) {
    switch (tok.getTokenType().getValue()) {
        case TokenType::PostInc: return PostfixOperator::PostfixInc;
        case TokenType::PostDec: return PostfixOperator::PostfixDec;
        default: throw InvalidOperator("Invalid postfix operator.");
    }
}

std::string getPostfixOperator(PostfixOperator op) {
    switch (op) {
        case PostfixOperator::PostfixInc: return "++";
        case PostfixOperator::PostfixDec: return "--";
        default: throw InvalidOperator("Invalid postfix operator.");
    }
}

}

