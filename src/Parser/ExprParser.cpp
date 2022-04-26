//
// Created by henry on 2022-04-26.
//

#include "Parser.h"

#include "Operator.h"
#include "ASTContext.h"

namespace reflex {

Identifier *Parser::parseBaseDeclRef() {
    auto token = expect(TokenType::Identifier);
    return context.create<Identifier>(token.getLocInfo(), nullptr, token.getLexeme());
}

ModuleSelector *Parser::parseModuleSelector(DeclRefExpr *base) {
    expect(TokenType::NameSeparator);
    auto name = parseBaseTypenameType();
    auto selector = context.create<ModuleSelector>(
        name->location(), nullptr,
        name->getTypeName(), base
    );
    while (check(TokenType::NameSeparator)) {
        next();
        name = parseBaseTypenameType();
        selector = context.create<ModuleSelector>(
            name->location(), nullptr,
            name->getTypeName(), base
        );
    }
    return selector;
}

Expression *Parser::parseNamedOperand() {
    if (check(TokenType::Identifier)) {
        DeclRefExpr *ident = parseBaseDeclRef();
        if (check(TokenType::NameSeparator)) {
            ident = parseModuleSelector(ident);
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
    if (lookahead.isUnaryOp()) {
        auto unaryOp = lookahead;
        next();
        return context.create<UnaryExpr>(
            unaryOp.getLocInfo(),
            Operator::createUnaryOperatorFromToken(unaryOp),
            parseExpr()
        );
    }
    return parsePrimaryExpr();
}

Expression *Parser::parseOperand() {
    if (check(TokenType::Identifier)) {
        DeclRefExpr *ident = parseBaseDeclRef();
        if (check(TokenType::NameSeparator)) {
            ident = parseModuleSelector(ident);
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
    auto type = lookahead.getTokenType().getValue();
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
    auto instanceTyp = parseType();
    return context.create<NewExpr>(
        instanceTyp->location(),
        instanceTyp
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
    return context.create<CastExpr>(
        startToken.getLocInfo(),
        typ, expr
    );
}

Expression *Parser::parseSelectorExpr(Expression *base) {
    expect(TokenType::Period);
    auto ident = parseBaseDeclRef();
    return context.create<SelectorExpr>(
        ident->location(),
        base, ident->getBaseRefName()
    );
}

IndexExpr *Parser::parseIndexExpr(Expression *base) {
    auto startToken = expect(TokenType::LBracket);
    auto index = parseExpr();
    expect(TokenType::RBracket);
    return context.create<IndexExpr>(
        startToken.getLocInfo(),
        base, index
    );
}

ArgumentExpr *Parser::parseArgument(Expression *base) {
    auto startToken = expect(TokenType::LParen);
    auto args = parseArgumentList();
    expect(TokenType::RParen);
    return context.create<ArgumentExpr>(
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
    while (lookahead.isBinaryOp() && lookahead.getTokenPrec() >= minPrec) {
        auto binOp = lookahead;
        next();
        auto rhs = parseUnaryExpr();
        while (lookahead.getTokenPrec() > binOp.getTokenPrec()) {
            auto prec = binOp.getTokenPrec() + 1;
            rhs = parseExpr1(prec, rhs);
        }
        lhs = context.create<BinaryExpr>(
            binOp.getLocInfo(),
            Operator::createBinaryOperatorFromToken(binOp),
            lhs, rhs
        );
    }
    return lhs;
}

}
