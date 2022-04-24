//
// Created by henry on 2022-03-20.
//

#ifndef REFLEX_SRC_AST_AST_H_
#define REFLEX_SRC_AST_AST_H_

#include "../Source/Token.h"
#include "Type.h"
#include "../Source/SourceManager.h"

namespace reflex {

class AstVisitor;
class AstVisitable {
  public:
    virtual void accept(AstVisitor *visitor) = 0;
};

class AstExpr : public AstVisitable {
    Type *typ = nullptr;
    SourceLocation &loc;
  public:
    explicit AstExpr(SourceLocation &loc) : loc(loc) {}
    virtual ~AstExpr() = default;

    [[nodiscard]] const SourceLocation &getLoc() const { return loc; }
    [[nodiscard]] Type *getTyp() const { return typ; }
    void setTyp(Type *type) { AstExpr::typ = type; }
};

}

#endif //REFLEX_SRC_AST_AST_H_
