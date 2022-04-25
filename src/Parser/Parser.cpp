//
// Created by henry on 2022-04-24.
//

#include "Parser.h"

#include <Lexer.h>

namespace reflex {

Token Parser::next() {
    lookahead = lexer.nextToken();
    if (!lookahead.isTrivial())
        return lookahead;
    return next();
}

bool Parser::check(TokenType::Value tokenType) const {
    return lookahead.getTokenType().getValue() == tokenType;
}

Token Parser::expect(TokenType::Value expectedType, ErrorHandler *handler) {
    auto curr = lookahead;
    if (!check(expectedType)) {
        if (handler) {
            errorList.push_back(std::make_unique<ParsingExpectedTokenError>(
                curr.getLocInfo(), TokenType(expectedType), curr
            ));
            handler->resumeParsingHandler();
        } else {
            throw UnrecoverableError(
                curr.getLocInfo(),
                "error: expected " + TokenType(expectedType).getTypeString() +
                    " but got " + curr.getTokenType().getTypeString()
            );
        }
    } else {
        next();
    }
    return curr;
}

}
