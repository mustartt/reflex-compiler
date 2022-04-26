//
// Created by henry on 2022-04-24.
//

#include "Parser.h"

#include <Lexer.h>

namespace reflex {

Parser::Parser(ASTContext &context, Lexer &lex)
    : context(context), lexer(lex),
      lookahead(TokenType::EndOfFile, "EOF",
                lex.getSource().createSourceLocation(1, 1, 1, 1)),
      state{} {
    lookahead = next();
}

Token Parser::next() {
    lookahead = lexer.nextToken();
    while (lookahead.isTrivial()) {
        lookahead = lexer.nextToken();
    }
    return lookahead;
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

std::string Parser::parseString() {
    auto name = expect(TokenType::Identifier);
    return name.getLexeme();
}

}
