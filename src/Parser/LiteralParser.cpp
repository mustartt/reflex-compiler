//
// Created by henry on 2022-04-26.
//

#include "Parser.h"

#include "ASTContext.h"

namespace reflex {

Literal *Parser::parseLiteral() {
    if (lookahead.isBasicLiteral())
        return parseBasicLit();
    if (check(TokenType::LBrace))
        return parseArrayLit();
    return parseFunctionLit();
}

Literal *Parser::parseBasicLit() {
    switch (lookahead.getTokenType().getValue()) {
        case TokenType::BoolLiteral: return parseBoolLit();
        case TokenType::NumberLiteral: return parseNumberLit();
        case TokenType::StringLiteral: return parseStringLit();
        default: return parseNullLit();
    }
}

NumberLiteral *Parser::parseNumberLit() {
    auto token = expect(TokenType::NumberLiteral);
    return context.create<NumberLiteral>(token.getLocInfo(), token.getLexeme());
}

StringLiteral *Parser::parseStringLit() {
    auto token = expect(TokenType::StringLiteral);
    auto &lit = token.getLexeme();
    return context.create<StringLiteral>(
        token.getLocInfo(),
        lit.substr(1, lit.size() - 2)
    );
}

BooleanLiteral *Parser::parseBoolLit() {
    auto token = expect(TokenType::BoolLiteral);
    return context.create<BooleanLiteral>(token.getLocInfo(), token.getLexeme());
}

NullLiteral *Parser::parseNullLit() {
    auto token = expect(TokenType::NullLiteral);
    return context.create<NullLiteral>(token.getLocInfo(), token.getLexeme());
}

Literal *Parser::parseArrayLit() {
    auto startToken = lookahead;
    return context.create<ArrayLiteral>(
        startToken.getLocInfo(),
        parseLiteralValue()
    );
}

std::vector<Expression *> Parser::parseLiteralValue() {
    expect(TokenType::LBrace);
    auto elements = parseElementList();
    expect(TokenType::RBrace);
    return elements;
}

std::vector<Expression *> Parser::parseElementList() {
    if (check(TokenType::RBrace))
        return {};
    std::vector<Expression *> elements;
    elements.push_back(parseElement());
    while (!check(TokenType::RBrace)) {
        expect(TokenType::Comma);
        elements.push_back(parseElement());
    }
    return elements;
}

Expression *Parser::parseElement() {
    if (check(TokenType::LBrace)) {
        return parseArrayLit();
    }
    return parseExpr();
}

Literal *Parser::parseFunctionLit() {
    auto start = expect(TokenType::Func);
    auto[params, ret] = parseSignature();
    auto body = parseBlockStmt();
    return context.create<FunctionLiteral>(
        start.getLocInfo(),
        params, ret, body
    );
}

}

