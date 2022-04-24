//
// Created by Admininstrator on 2022-04-23.
//

#ifndef REFLEX_SRC_AST_ASTLITERAL_H_
#define REFLEX_SRC_AST_ASTLITERAL_H_

#include "ASTExpression.h"

namespace reflex {

class Literal : public Expression {};

class BasicLiteral : public Literal {};
class NumberLiteral : public BasicLiteral {};
class StringLiteral : public BasicLiteral {};
class BooleanLiteral : public BasicLiteral {};
class NullLiteral : public BasicLiteral {};

class ArrayLiteral : public Literal {};
class FunctionLiteral : public Literal {};

}

#endif //REFLEX_SRC_AST_ASTLITERAL_H_
