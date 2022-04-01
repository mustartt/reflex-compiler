//
// Created by henry on 2022-03-25.
//

#ifndef REFLEX_SRC_TYPES_TYPEPARSER_H_
#define REFLEX_SRC_TYPES_TYPEPARSER_H_

#include "Type.h"

namespace reflex {

class TypeContextManager;
class ScopeSymbolTypeTable;
class TypeParser {
    TypeContextManager &typeContext;
  public:
    explicit TypeParser(TypeContextManager &typeContext) : typeContext(typeContext) {}

    Type *parseTypeExpr(TypeExpr *decl, ScopeSymbolTypeTable *scope);
  private:
    AggregateType *parseAggregateExpr(IdentifierTypeExpr *decl, ScopeSymbolTypeTable *scope);
    VoidType *parseVoidTypeExpr(IdentifierTypeExpr *decl, ScopeSymbolTypeTable *scope);
    PrimType *parsePrimTypeExpr(IdentifierTypeExpr *decl, ScopeSymbolTypeTable *scope);
    ArrayType *parseArrTypeExpr(ArrayTypeExpr *decl, ScopeSymbolTypeTable *scope);
    FunctionType *parseFuncTypeExpr(FunctionTypeExpr *decl, ScopeSymbolTypeTable *scope);
};

}

#endif //REFLEX_SRC_TYPES_TYPEPARSER_H_
