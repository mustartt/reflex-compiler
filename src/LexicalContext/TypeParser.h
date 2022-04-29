//
// Created by henry on 2022-04-28.
//

#ifndef REFLEX_SRC_LEXICALCONTEXT_TYPEPARSER_H_
#define REFLEX_SRC_LEXICALCONTEXT_TYPEPARSER_H_

#include "Type.h"
#include "ASTType.h"

namespace reflex {

class TypeContext;
class LexicalScope;

class TypeParser {
  public:
    explicit TypeParser(TypeContext &context) : context(context) {}

    ClassType *parseClassType(ReferenceTypenameExpr *expr, LexicalScope *starting);
    InterfaceType *parseInterfaceType(ReferenceTypenameExpr *expr, LexicalScope *starting);

    Type *parseBaseType(ASTTypeExpr *expr, LexicalScope *starting);
    Type *parseReferenceTypeExpr(ASTTypeExpr *expr, LexicalScope *starting);

  private:
    CompositeType *parseCompositeType(ReferenceTypenameExpr *expr, LexicalScope *starting);
    VoidType *parseVoidType(ReferenceTypenameExpr *expr, LexicalScope *starting);
    BuiltinType *parseBuiltinType(ReferenceTypenameExpr *expr, LexicalScope *starting);

    ArrayType *parseRefArrayType(ArrayTypeExpr *expr, LexicalScope *starting);
    FunctionType *parseRefFunctionType(FunctionTypeExpr *expr, LexicalScope *starting);

    ArrayType *parseArrayType(ArrayTypeExpr *expr, LexicalScope *starting);
    FunctionType *parseFunctionType(FunctionTypeExpr *expr, LexicalScope *starting);

    TypeContext &context;
};

} // reflex

#endif //REFLEX_SRC_LEXICALCONTEXT_TYPEPARSER_H_
