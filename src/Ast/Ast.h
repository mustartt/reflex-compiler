//
// Created by henry on 2022-03-20.
//

#ifndef REFLEX_SRC_AST_AST_H_
#define REFLEX_SRC_AST_AST_H_

#include "../Lexer/Token.h"
#include "Type.h"

namespace reflex {

class AstVisitor;
class AstVisitable {
  public:
    virtual void accept(AstVisitor *visitor) = 0;
};

class AstExpr : public AstVisitable {
    Type *typ = nullptr;
    Loc loc;
  public:
    explicit AstExpr(const Loc &loc) : loc(loc) {}
    virtual ~AstExpr() = default;

    [[nodiscard]] const Loc &getLoc() const { return loc; }
    [[nodiscard]] Type *getTyp() const { return typ; }
    void setTyp(Type *type) { AstExpr::typ = type; }
};

}

#endif //REFLEX_SRC_AST_AST_H_
