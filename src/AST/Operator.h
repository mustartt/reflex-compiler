//
// Created by henry on 2022-03-20.
//

#ifndef REFLEX_SRC_AST_OPERATOR_H_
#define REFLEX_SRC_AST_OPERATOR_H_

#include <stdexcept>
#include <string>
#include "../Lexer/Token.h"

namespace reflex {

class InvalidOperator : public std::runtime_error {
  public:
    explicit InvalidOperator(const std::string &arg) : runtime_error(arg) {}
};

enum class BinaryOperator {
  Or, And,
  Compare, CompareNot, Less, Greater, CompareLessThanEqual, CompareGreaterThanEqual,
  Add, Sub, LogicalOr,
  Mult, Div, Mod, LogicalAnd
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

enum class PostfixOperator {
  PostfixInc, PostfixDec
};

}

#endif //REFLEX_SRC_AST_OPERATOR_H_
