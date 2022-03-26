//
// Created by henry on 2022-03-26.
//

#include "OperatorSemantics.h"

namespace reflex {

PrimType *OperatorSemantics::getBinaryOperatorResultType(TypeContextManager *ctx,
                                                         BinaryOperator op,
                                                         Type *from,
                                                         Type *to) {
    auto fromType = dynamic_cast<PrimType *>(from);
    auto toType = dynamic_cast<PrimType *>(to);
    if (!fromType || !toType) return nullptr;
    if (op == BinaryOperator::Or ||
        op == BinaryOperator::And ||
        op == BinaryOperator::LogicalOr ||
        op == BinaryOperator::LogicalAnd) {
        auto boolTy = ctx->getPrimitiveTy(PrimType::Boolean);
        if (primitiveTypeImplicitConvertible(from, boolTy) &&
            primitiveTypeImplicitConvertible(from, boolTy)) {
            return boolTy;
        } else { return nullptr; }
    }
    if (BinaryOperator::Compare <= op && op <= BinaryOperator::CompareGreaterThanEqual) {
        if (getPromotedType(fromType->getBaseTyp(), toType->getBaseTyp()) != PrimType::Null) {
            return ctx->getPrimitiveTy(PrimType::Boolean);
        } else {
            return nullptr;
        }
    }
    if (getPromotedType(fromType->getBaseTyp(), toType->getBaseTyp()) != PrimType::Null) {
        return ctx->getPrimitiveTy(getPromotedType(fromType->getBaseTyp(), toType->getBaseTyp()));
    }
    return nullptr;
}

PrimType::BaseType OperatorSemantics::getPromotedType(PrimType::BaseType lhs, PrimType::BaseType rhs) {
    PrimType::BaseType PromotionTable[5][5] = {
        {PrimType::Integer, PrimType::Number, PrimType::Integer, PrimType::Integer, PrimType::Null},
        {PrimType::Number, PrimType::Number, PrimType::Number, PrimType::Number, PrimType::Null},
        {PrimType::Integer, PrimType::Number, PrimType::Character, PrimType::Character, PrimType::Null},
        {PrimType::Integer, PrimType::Number, PrimType::Character, PrimType::Boolean, PrimType::Null},
        {PrimType::Null, PrimType::Null, PrimType::Null, PrimType::Boolean, PrimType::Null}
    };
    return PromotionTable[lhs][rhs];
}

bool OperatorSemantics::primitiveTypeImplicitConvertible(Type *from, Type *to) {
    auto fromType = dynamic_cast<PrimType *>(from);
    auto toType = dynamic_cast<PrimType *>(to);
    if (!fromType || !toType) return false;
    bool ConversionTable[5][5] = {
        //        int    num    char   bool   null
        /*int*/  {true, true, false, true, false},
        /*num*/  {false, true, false, true, false},
        /*char*/ {true, false, true, true, false},
        /*bool*/ {true, true, true, true, false},
        /*null*/ {false, false, false, true, true},
    };
    return ConversionTable[fromType->getBaseTyp()][toType->getBaseTyp()];
}

PrimType *OperatorSemantics::getUnaryOperatorResultType(TypeContextManager *ctx, UnaryOperator op, Type *from) {
    auto boolTy = ctx->getPrimitiveTy(PrimType::Boolean);
    if (op == UnaryOperator::LogicalNot) {
        if (primitiveTypeImplicitConvertible(from, boolTy)) {
            return boolTy;
        } else {
            return nullptr;
        }
    }
    if (op == UnaryOperator::Negative) {
        if (auto pTy = dynamic_cast<PrimType *>(from)) {
            auto baseTy = pTy->getBaseTyp();
            if (baseTy == PrimType::Null || baseTy == PrimType::Boolean || baseTy == PrimType::Character) {
                return nullptr;
            }
            return pTy;
        }
    }
    return nullptr;
}

}
