//
// Created by henry on 2022-03-21.
//

#include "Parser.h"

#include "../Lexer/Lexer.h"

namespace reflex {

ParsingError::ParsingError(const std::string &msg)
    : runtime_error("Parsing Error: " + msg) {}

ExpectToken::ExpectToken(TokenType type, const Token &token)
    : ParsingError("Expected token of value " + type.getTypeString()
                       + " but got " + token.toString() + " instead.") {}

Parser::Parser(Lexer *lexer, AstContextManager *ctx) : lexer(lexer), ctx(ctx) {
    tok = next();
}

Token Parser::next() {
    if (!lexer->hasNext())
        return {TokenType::EndOfFile, "EOF", lexer->getCurrentPosition()};
    tok = lexer->nextToken();
    if (!check(TokenType::WhiteSpace)) return tok;
    return next();
}

bool Parser::check(TokenType::Value tokenType) const {
    return tok.getTokenType().getValue() == tokenType;
}

Token Parser::expect(TokenType::Value expectedType) {
    auto curr = tok;
    if (!check(expectedType)) throw ExpectToken(TokenType(expectedType), curr);
    next();
    return curr;
}

}
