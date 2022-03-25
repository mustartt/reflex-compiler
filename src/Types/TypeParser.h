//
// Created by henry on 2022-03-25.
//

#ifndef REFLEX_SRC_TYPES_TYPEPARSER_H_
#define REFLEX_SRC_TYPES_TYPEPARSER_H_

#include "Type.h"
#include "TypeContextManager.h"
#include "../AST/AstNodes.h"

namespace reflex {

class TypeParser {
    TypeContextManager *typeContext;
  public:
    explicit TypeParser(TypeContextManager *typeContext) : typeContext(typeContext) {}
    Type *parseTypeExpr(TypeExpr *decl);
  private:
    AggregateType *parseAggregateExpr(IdentifierType *decl);
    VoidType *parseVoidTypeExpr(IdentifierType *decl);
    PrimType *parsePrimTypeExpr(IdentifierType *decl);
    ArrType *parseArrTypeExpr(ArrayType *decl);
    FuncType *parseFuncTypeExpr(FunctionType *decl);
};

}

#endif //REFLEX_SRC_TYPES_TYPEPARSER_H_
