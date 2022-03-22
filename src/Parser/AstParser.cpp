//
// Created by henry on 2022-03-21.
//

#include "Parser.h"

#include "../AST/AstContextManager.h"
#include "../AST/Operator.h"

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

Expression *Parser::parseNamedOperand() {
    if (check(TokenType::Identifier)) {
        auto ident = parseIdent();
        if (check(TokenType::NameSeparator)) {
            ident = parseModuleIdent(ident);
        }
        return ident;
    }
    if (check(TokenType::LParen)) {
        next();
        auto subexpr = parseExpr();
        expect(TokenType::RParen);
        return subexpr;
    }
    return parseLiteral();
}

Expression *Parser::parseUnaryExpr() {
    if (tok.isUnaryOp()) {
        auto unaryOp = tok;
        next();
        return ctx->create<UnaryExpr>(
            unaryOp.getLocInfo(),
            createUnaryOperatorFromToken(unaryOp),
            parseExpr()
        );
    }
    return parsePrimaryExpr();
}

Expression *Parser::parseOperand() {
    if (check(TokenType::Identifier)) {
        auto ident = parseIdent();
        if (check(TokenType::NameSeparator)) {
            ident = parseModuleIdent(ident);
        }
        return ident;
    }
    if (check(TokenType::LParen)) {
        next();
        auto subexpr = parseExpr();
        expect(TokenType::RParen);
        return subexpr;
    }
    return parseLiteral();
}

Expression *Parser::parsePrimaryExpr() {
    Expression *base;
    if (check(TokenType::New)) {
        base = parseNewExpr();
    } else if (check(TokenType::Cast)) {
        base = parseConversion();
    } else {
        base = parseOperand();
    }
    return parsePrimaryExpr2(base);
}

Expression *Parser::parsePrimaryExpr1(Expression *base) {
    if (check(TokenType::Period)) {
        return parseSelectorExpr(base);
    }
    if (check(TokenType::LBracket)) {
        return parseIndexExpr(base);
    }
    return parseArgument(base);
}

Expression *Parser::parsePrimaryExpr2(Expression *base) {
    auto type = tok.getTokenType().getValue();
    if (type == TokenType::Period ||
        type == TokenType::LBracket ||
        type == TokenType::LParen) {
        auto newbase = parsePrimaryExpr1(base);
        return parsePrimaryExpr2(newbase);
    }
    return base;
}

Expression *Parser::parseNewExpr() {
    expect(TokenType::New);
    auto ident = parseIdent();
    if (check(TokenType::NameSeparator)) {
        ident = parseModuleIdent(ident);
    }
    return ctx->create<NewExpr>(
        ident->getLoc(),
        ident
    );
}

Expression *Parser::parseConversion() {
    auto startToken = expect(TokenType::Cast);
    expect(TokenType::LAngleBracket);
    auto typ = parseType();
    expect(TokenType::RAngleBracket);
    expect(TokenType::LParen);
    auto expr = parseExpr();
    expect(TokenType::RParen);
    return ctx->create<CastExpr>(
        startToken.getLocInfo(),
        typ, expr
    );
}

Expression *Parser::parseSelectorExpr(Expression *base) {
    expect(TokenType::Period);
    auto ident = parseIdent();
    return ctx->create<SelectorExpr>(
        ident->getLoc(),
        base, ident
    );
}

Expression *Parser::parseIndexExpr(Expression *base) {
    auto startToken = expect(TokenType::LBracket);
    auto index = parseExpr();
    expect(TokenType::RBracket);
    return ctx->create<IndexExpr>(
        startToken.getLocInfo(),
        base, index
    );
}

Expression *Parser::parseArgument(Expression *base) {
    auto startToken = expect(TokenType::LParen);
    auto args = parseArgumentList();
    expect(TokenType::RParen);
    return ctx->create<ArgumentExpr>(
        startToken.getLocInfo(),
        base, args
    );
}

std::vector<Expression *> Parser::parseArgumentList() {
    if (check(TokenType::RParen)) {
        return {};
    }
    std::vector<Expression *> args;
    args.push_back(parseExpr());
    while (!check(TokenType::RParen)) {
        expect(TokenType::Comma);
        args.push_back(parseExpr());
    }
    return args;
}

Expression *Parser::parseExpr() {
    return parseExpr1(0, parseUnaryExpr());
}

Expression *Parser::parseExpr1(int minPrec, Expression *lhs) {
    while (tok.isBinaryOp() && tok.getTokenPrec() >= minPrec) {
        auto binOp = tok;
        next();
        auto rhs = parseUnaryExpr();
        while (tok.getTokenPrec() > binOp.getTokenPrec()) {
            auto prec = binOp.getTokenPrec() + 1;
            rhs = parseExpr1(prec, rhs);
        }
        lhs = ctx->create<BinaryExpr>(
            binOp.getLocInfo(),
            createBinaryOperatorFromToken(binOp),
            lhs, rhs
        );
    }
    return lhs;
}

Block *Parser::parseBlock() {
    auto start = expect(TokenType::LBrace);
    auto stmts = parseStmtList();
    expect(TokenType::RBrace);
    return ctx->create<Block>(
        start.getLocInfo(),
        stmts
    );
}

std::vector<Statement *> Parser::parseStmtList() {
    std::vector<Statement *> stmts;
    while (!check(TokenType::RBrace)) {
        stmts.push_back(parseStatement());
    }
    return stmts;
}

Statement *Parser::parseStatement() {
    return nullptr;
}

}

