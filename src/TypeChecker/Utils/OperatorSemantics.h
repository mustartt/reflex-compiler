//
// Created by henry on 2022-03-26.
//

#ifndef REFLEX_SRC_TYPECHECKER_UTILS_OPERATORSEMANTICS_H_
#define REFLEX_SRC_TYPECHECKER_UTILS_OPERATORSEMANTICS_H_

#include <Operator.h>
#include <TypeContextManager.h>
#include "Type.h"

namespace reflex {

class OperatorSemantics {
  public:
    static bool primitiveTypeImplicitConvertible(Type *from, Type *to);
    static PrimType::BaseType getPromotedType(PrimType::BaseType lhs, PrimType::BaseType rhs);
    static PrimType *getBinaryOperatorResultType(TypeContextManager *ctx,
                                                 BinaryOperator op, Type *from, Type *to);
    static PrimType *getUnaryOperatorResultType(TypeContextManager *ctx,
                                                UnaryOperator op, Type *from);
};

}

#endif //REFLEX_SRC_TYPECHECKER_UTILS_OPERATORSEMANTICS_H_
