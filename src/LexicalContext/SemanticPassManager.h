//
// Created by henry on 2022-04-28.
//

#ifndef REFLEX_SRC_LEXICALCONTEXT_SEMANTICPASSMANAGER_H_
#define REFLEX_SRC_LEXICALCONTEXT_SEMANTICPASSMANAGER_H_

namespace reflex {

class TypeContext;
class LexicalContext;

class SemanticPass {
  public:
    virtual ~SemanticPass() = default;
};

class SemanticPassManager {
  public:
    SemanticPassManager(TypeContext &typeContext, LexicalContext &lexicalContext)
        : typeContext(typeContext), lexicalContext(lexicalContext) {}

  private:
    TypeContext &typeContext;
    LexicalContext &lexicalContext;
};

}

#endif //REFLEX_SRC_LEXICALCONTEXT_SEMANTICPASSMANAGER_H_
