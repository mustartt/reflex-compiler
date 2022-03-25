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
    AggregateType *parseAggregateExpr(IdentifierTypeExpr *decl);
    VoidType *parseVoidTypeExpr(IdentifierTypeExpr *decl);
    PrimType *parsePrimTypeExpr(IdentifierTypeExpr *decl);
    ArrayType *parseArrTypeExpr(ArrayTypeExpr *decl);
    FunctionType *parseFuncTypeExpr(FunctionTypeExpr *decl);
};

}

#endif //REFLEX_SRC_TYPES_TYPEPARSER_H_
