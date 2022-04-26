//
// Created by henry on 2022-04-26.
//

#include "Parser.h"

#include "ASTContext.h"

namespace reflex {

ASTTypeExpr *Parser::parseType() {
    if (check(TokenType::LParen)) {
        next();
        auto nestedTyp = parseType();
        expect(TokenType::RParen);
        if (check(TokenType::LBracket)) {
            next();
            NumberLiteral *size = nullptr;
            if (!check(TokenType::RBracket)) {
                size = parseNumberLit();
            }
            expect(TokenType::RBracket);
            auto loc = nestedTyp->location();
            auto arrTyp = parseElementType1(context.create<ArrayTypeExpr>(loc, nestedTyp, size));
            return parseElementType1(
                context.create<ArrayTypeExpr>(loc, arrTyp, size)
            );
        }
        return nestedTyp;
    }
    ASTTypeExpr *baseTyp;
    if (!check(TokenType::Func)) {
        ReferenceTypenameExpr *typ = parseBaseTypenameType();
        if (check(TokenType::NameSeparator)) {
            typ = parseQualifiedType(typ);
        }
        baseTyp = typ;
    } else {
        baseTyp = parseFunctionType();
    }
    if (check(TokenType::LBracket)) {
        next();
        NumberLiteral *size = nullptr;
        if (!check(TokenType::RBracket)) {
            size = parseNumberLit();
        }
        expect(TokenType::RBracket);
        return parseElementType1(
            context.create<ArrayTypeExpr>(baseTyp->location(), baseTyp, size)
        );
    }
    return baseTyp;
}

BaseTypenameExpr *Parser::parseBaseTypenameType() {
    auto token = expect(TokenType::Identifier);
    return context.create<BaseTypenameExpr>(token.getLocInfo(), token.getLexeme());
}

QualifiedTypenameExpr *Parser::parseQualifiedType(ReferenceTypenameExpr *base) {
    expect(TokenType::NameSeparator);
    auto name = parseBaseTypenameType();
    auto selector = context.create<QualifiedTypenameExpr>(
        name->location(),
        name->getTypeName(),
        base
    );
    while (check(TokenType::NameSeparator)) {
        next();
        name = parseBaseTypenameType();
        selector = context.create<QualifiedTypenameExpr>(
            name->location(),
            name->getTypeName(),
            base
        );
    }
    return selector;
}

ArrayTypeExpr *Parser::parseArrayType() {
    ASTTypeExpr *base;
    if (check(TokenType::LParen)) {
        next();
        base = parseType();
        expect(TokenType::RParen);
    } else if (check(TokenType::Func)) {
        base = parseFunctionType();
    } else {
        ReferenceTypenameExpr *type = parseBaseTypenameType();
        if (check(TokenType::NameSeparator)) {
            type = parseQualifiedType(type);
        }
        base = type;
    }
    NumberLiteral *size = nullptr;
    expect(TokenType::LBracket);
    if (!check(TokenType::RBracket)) {
        size = parseNumberLit();
    }
    expect(TokenType::RBracket);
    return parseElementType1(
        context.create<ArrayTypeExpr>(base->location(), base, size)
    );
}

ArrayTypeExpr *Parser::parseElementType1(ArrayTypeExpr *baseType) {
    if (check(TokenType::LBracket)) {
        NumberLiteral *size = nullptr;
        expect(TokenType::LBracket);
        if (!check(TokenType::RBracket)) {
            size = parseNumberLit();
        }
        expect(TokenType::RBracket);
        return parseElementType1(
            context.create<ArrayTypeExpr>(baseType->location(), baseType, size)
        );
    }
    return baseType;
}

FunctionTypeExpr *Parser::parseFunctionType() {
    auto start = expect(TokenType::Func);
    expect(TokenType::LParen);
    auto typeParams = parseParamTypeList();
    auto end = expect(TokenType::RParen);

    ASTTypeExpr *returnType = context.create<BaseTypenameExpr>(end.getLocInfo(), "void");
    if (check(TokenType::ReturnArrow)) {
        next();
        returnType = parseType();
    }
    return context.create<FunctionTypeExpr>(
        start.getLocInfo(),
        returnType,
        typeParams
    );
}

std::vector<ASTTypeExpr *> Parser::parseParamTypeList() {
    if (check(TokenType::RParen)) return {};
    std::vector<ASTTypeExpr *> params;
    params.push_back(parseType());
    while (!check(TokenType::RParen)) {
        expect(TokenType::Comma);
        params.push_back(parseType());
    }
    return params;
}

}
