//
// Created by henry on 2022-03-20.
//

#ifndef REFLEX_SRC_AST_AST_H_
#define REFLEX_SRC_AST_AST_H_

#include "../Lexer/Token.h"

namespace reflex {

class AstExpr {
    Loc loc;
  public:
    explicit AstExpr(const Loc &loc) : loc(loc) {}

    virtual ~AstExpr() = default;
};

}

#endif //REFLEX_SRC_AST_AST_H_
