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

Literal *Parser::parseLiteral() {
    if (tok.isBasicLiteral())
        return parseBasicLit();
    if (check(TokenType::LBrace))
        return parseArrayLit();
    return parseFunctionLit();
}

Literal *Parser::parseBasicLit() {
    switch (tok.getTokenType().getValue()) {
        case TokenType::BoolLiteral: return parseBoolLit();
        case TokenType::NumberLiteral: return parseNumberLit();
        case TokenType::StringLiteral: return parseStringLit();
        default: return parseNullLit();
    }
}

NumberLit *Parser::parseNumberLit() {
    auto token = expect(TokenType::NumberLiteral);
    return ctx->create<NumberLit>(token.getLocInfo(), token.getLexeme());
}

StringLit *Parser::parseStringLit() {
    auto token = expect(TokenType::StringLiteral);
    auto &lit = token.getLexeme();
    return ctx->create<StringLit>(
        token.getLocInfo(),
        lit.substr(1, lit.size() - 2)
    );
}

BoolLit *Parser::parseBoolLit() {
    auto token = expect(TokenType::BoolLiteral);
    return ctx->create<BoolLit>(token.getLocInfo(), token.getLexeme());
}

NullLit *Parser::parseNullLit() {
    auto token = expect(TokenType::NullLiteral);
    return ctx->create<NullLit>(token.getLocInfo(), token.getLexeme());
}

Literal *Parser::parseArrayLit() {
    auto startToken = tok;
    return ctx->create<ArrayLit>(
        startToken.getLocInfo(),
        parseLiteralValue()
    );
}

std::vector<AstExpr *> Parser::parseLiteralValue() {
    expect(TokenType::LBrace);
    auto elements = parseElementList();
    expect(TokenType::RBrace);
    return elements;
}

std::vector<AstExpr *> Parser::parseElementList() {
    if (check(TokenType::RBrace))
        return {};
    std::vector<AstExpr *> elements;
    elements.push_back(parseElement());
    while (!check(TokenType::RBrace)) {
        expect(TokenType::Comma);
        elements.push_back(parseElement());
    }
    return elements;
}

AstExpr *Parser::parseElement() {
    if (check(TokenType::LBrace)) {
        return parseArrayLit();
    }
    return parseExpr();
}

Literal *Parser::parseFunctionLit() {
    auto start = expect(TokenType::Func);
    auto[params, ret] = parseSignature();
    auto body = parseBlock();
    return ctx->create<FunctionLit>(
        start.getLocInfo(),
        params, ret, body
    );
}

std::pair<std::vector<Parameter *>, TypeExpr *> Parser::parseSignature() {
    expect(TokenType::LParen);
    auto params = parseParamList();
    auto end = expect(TokenType::RParen);
    TypeExpr *retTyp = ctx->create<IdentifierType>(
        end.getLocInfo(),
        ctx->create<Identifier>(end.getLocInfo(), "void")
    );
    if (check(TokenType::ReturnArrow)) {
        next();
        retTyp = parseType();
    }
    return {params, retTyp};
}

std::vector<Parameter *> Parser::parseParamList() {
    if (check(TokenType::RParen)) {
        return {};
    }
    std::vector<Parameter *> params;
    params.push_back(parseFuncParam());
    while (!check(TokenType::RParen)) {
        expect(TokenType::Comma);
        params.push_back(parseFuncParam());
    }
    return params;
}

Parameter *Parser::parseFuncParam() {
    auto ident = parseIdent();
    expect(TokenType::Colon);
    return ctx->create<Parameter>(
        ident->getLoc(),
        ident,
        parseType()
    );
}

Expression *Parser::parseExpr() {
    return nullptr;
}

}

