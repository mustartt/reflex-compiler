//
// Created by Admininstrator on 2022-04-23.
//

#ifndef REFLEX_SRC_AST_ASTEXPRESSION_H_
#define REFLEX_SRC_AST_ASTEXPRESSION_H_

#include "AST.h"

namespace reflex {

class SourceLocation;

class Expression : public ASTNode {
  private:
    const SourceLocation *loc;
};

class ReferenceExpr : public Expression {};
class Identifier : public ReferenceExpr {};
class ModuleSelector : public ReferenceExpr {};

class UnaryExpr : public Expression {};
class BinaryExpr : public Expression {};

class NewExpr : public Expression {};
class CastExpr : public Expression {};
class IndexExpr : public Expression {};
class SelectorExpr : public Expression {};
class ArgumentExpr : public Expression {};

}
#endif //REFLEX_SRC_AST_ASTEXPRESSION_H_
