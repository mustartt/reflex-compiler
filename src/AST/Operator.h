//
// Created by henry on 2022-03-20.
//

#ifndef REFLEX_SRC_AST_OPERATOR_H_
#define REFLEX_SRC_AST_OPERATOR_H_

#include <stdexcept>
#include <string>
#include "Token.h"

namespace reflex::Operator {

class InvalidOperator : public std::runtime_error {
  public:
    explicit InvalidOperator(const std::string &arg) : runtime_error(arg) {}
};

enum class BinaryOperator {
  Or = 0, And = 1,
  Compare = 2, CompareNot = 3, Less = 4, Greater = 5, CompareLessThanEqual = 6, CompareGreaterThanEqual = 7,
  Add = 8, Sub = 9, LogicalOr = 10,
  Mult = 11, Div = 12, Mod = 13, LogicalAnd = 14
};

BinaryOperator createBinaryOperatorFromToken(const Token &token);
std::string getBinaryOperator(BinaryOperator op);

enum class UnaryOperator {
  Negative, LogicalNot
};

UnaryOperator createUnaryOperatorFromToken(const Token &tok);
std::string getUnaryOperator(UnaryOperator op);

enum class AssignOperator {
  Equal, AddEqual, SubEqual
};

AssignOperator createAssignOperatorFromToken(const Token &tok);
std::string getAssignOperator(AssignOperator op);

enum class PostfixOperator {
  PostfixInc, PostfixDec
};

PostfixOperator createPostfixOperatorFromToken(const Token &tok);
std::string getPostfixOperator(PostfixOperator op);

enum class ImplicitConversion {
  BoolToChar, BoolToInt, BoolToNum,
  CharToInt, CharToNum,
  IntToNum,
  CharToBool, IntToBool, NumToBool, RefToBool,
  RefDownCast, NullToRef, RefNullable,
  Invalid
};

std::string getImplicitConversion(ImplicitConversion conversion);

}

#endif //REFLEX_SRC_AST_OPERATOR_H_
