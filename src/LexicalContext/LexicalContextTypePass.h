//
// Created by henry on 2022-04-28.
//

#ifndef REFLEX_SRC_LEXICALCONTEXT_LEXICALCONTEXTTYPEPASS_H_
#define REFLEX_SRC_LEXICALCONTEXT_LEXICALCONTEXTTYPEPASS_H_

#include <ASTVisitor.h>

namespace reflex {

class LexicalContextTypePass : public ASTDeclVisitor,
                               public ASTStmtVisitor,
                               public ASTExprVisitor {

  public:
  private:
};

} // reflex

#endif //REFLEX_SRC_LEXICALCONTEXT_LEXICALCONTEXTTYPEPASS_H_
