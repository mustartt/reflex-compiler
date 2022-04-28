//
// Created by henry on 2022-04-27.
//

#ifndef REFLEX_SRC_TRANSFORMPASS_LEXICALCONTEXT_H_
#define REFLEX_SRC_TRANSFORMPASS_LEXICALCONTEXT_H_

#include <LexicalScope.h>
#include <ASTDeclaration.h>

#include <vector>
#include <memory>

namespace reflex {

class CompilationUnit;
class LexicalContext {
  public:
    LexicalContext() = default;

    LexicalScope *createGlobalScope(CompilationUnit *CU);
    LexicalScope *createCompositeScope(AggregateDecl *decl, LexicalScope *parent);
    LexicalScope *createFunctionScope(FunctionDecl *decl, LexicalScope *parent);
    LexicalScope *createMethodScope(MethodDecl *decl, LexicalScope *parent);
    LexicalScope *createBlockScope(BlockStmt *decl, size_t blockID, LexicalScope *parent);
    LexicalScope *createLambdaScope(FunctionLiteral *decl, size_t lambdaID, LexicalScope *parent);

    void dump(std::ostream &os, LexicalScope *start, size_t indent = 0);

  private:
    LexicalScope *insertScope(std::unique_ptr<LexicalScope> ptr);

    std::vector<std::unique_ptr<LexicalScope>> scopes;
};

}

#endif //REFLEX_SRC_TRANSFORMPASS_LEXICALCONTEXT_H_
