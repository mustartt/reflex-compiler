//
// Created by henry on 2022-03-21.
//

#include "Parser.h"

#include "../AST/AstContextManager.h"

namespace reflex {

IdentExpr *Parser::parseIdent() {
    auto token = expect(TokenType::Identifier);
    return ctx->create<Identifier>(token.getLocInfo(), token.getLexeme());
}

IdentExpr *Parser::parseModuleIdent(IdentExpr *base) {
    expect(TokenType::NameSeparator);
    IdentExpr *name = parseIdent();
    auto selector = ctx->create<ModuleSelector>(
        name->getLoc(),
        base,
        name->getIdent()
    );
    while (check(TokenType::NameSeparator)) {
        next();
        name = parseIdent();
        selector = ctx->create<ModuleSelector>(
            name->getLoc(),
            selector,
            name->getIdent()
        );
    }
    return selector;
}

TypeExpr *Parser::parseType() {
    if (check(TokenType::LParen)) {
        next();
        auto nestedTyp = parseType();
        expect(TokenType::RParen);
        if (check(TokenType::LBracket)) {
            next();
            auto lenExpr = parseExpr();
            expect(TokenType::RBracket);
            Loc loc = nestedTyp->getLoc();
            auto arrTyp = parseElementType1(ctx->create<ArrayType>(loc, nestedTyp, lenExpr));
            return parseElementType1(
                ctx->create<ArrayType>(loc, arrTyp, lenExpr)
            );
        }
        return nestedTyp;
    }
    TypeExpr *baseTyp;
    if (!check(TokenType::Func)) {
        auto typ = parseIdent();
        if (check(TokenType::NameSeparator)) {
            typ = parseModuleIdent(typ);
        }
        baseTyp = ctx->create<IdentifierType>(typ->getLoc(), typ);
    } else {
        baseTyp = parseFunctionType();
    }
    if (check(TokenType::LBracket)) {
        next();
        auto lenExpr = parseExpr();
        expect(TokenType::RBracket);
        return parseElementType1(
            ctx->create<ArrayType>(baseTyp->getLoc(), baseTyp, lenExpr)
        );
    }
    return baseTyp;
}

ArrayType *Parser::parseArrayType() {
    TypeExpr *base;
    if (check(TokenType::LParen)) {
        next();
        base = parseType();
        expect(TokenType::RParen);
    } else if (check(TokenType::Func)) {
        base = parseFunctionType();
    } else {
        auto typ = parseIdent();
        if (check(TokenType::NameSeparator)) {
            typ = parseModuleIdent(typ);
        }
        base = ctx->create<IdentifierType>(typ->getLoc(), typ);
    }
    expect(TokenType::LBracket);
    auto lenExpr = parseExpr();
    expect(TokenType::RBracket);
    return parseElementType1(
        ctx->create<ArrayType>(base->getLoc(), base, lenExpr)
    );
}

ArrayType *Parser::parseElementType1(ArrayType *baseTyp) {
    if (check(TokenType::LBracket)) {
        expect(TokenType::LBracket);
        auto lenExpr = parseExpr();
        expect(TokenType::RBracket);
        return parseElementType1(
            ctx->create<ArrayType>(baseTyp->getLoc(), baseTyp, lenExpr)
        );
    }
    return baseTyp;
}

FunctionType *Parser::parseFunctionType() {
    auto start = expect(TokenType::Func);
    expect(TokenType::LParen);
    auto typeParams = parseParamTypeList();
    auto end = expect(TokenType::RParen);

    TypeExpr *retTyp = ctx->create<IdentifierType>(
        end.getLocInfo(),
        ctx->create<Identifier>(end.getLocInfo(), "void")
    );
    if (check(TokenType::ReturnArrow)) {
        next();
        retTyp = parseType();
    }
    return ctx->create<FunctionType>(
        start.getLocInfo(),
        retTyp,
        typeParams
    );
}

std::vector<TypeExpr *> Parser::parseParamTypeList() {
    if (check(TokenType::RParen)) {
        return {};
    }
    std::vector<TypeExpr *> params;
    params.push_back(parseType());
    while (!check(TokenType::RParen)) {
        expect(TokenType::Comma);
        params.push_back(parseType());
    }
    return params;
}

Expression *Parser::parseExpr() {
    return nullptr;
}

}

