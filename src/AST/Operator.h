//
// Created by henry on 2022-03-20.
//

#ifndef REFLEX_SRC_AST_OPERATOR_H_
#define REFLEX_SRC_AST_OPERATOR_H_

namespace reflex {

enum class BinaryOperator {
  Or, And,
  Compare, CompareNot, Less, Greater, CompareLessThanEqual, CompareLessThanGreater,
  Add, Sub, LogicalOr,
  Mult, Div, Mod, LogicalAnd
};

enum class UnaryOperator {
  Negative, LogicalNot
};

enum class AssignOperator {
  Equal, AddEqual, SubEqual
};

enum class PostfixOperator {
  PostfixInc, PostfixDec
};

}

#endif //REFLEX_SRC_AST_OPERATOR_H_
