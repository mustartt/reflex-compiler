//
// Created by henry on 2022-03-20.
//

#ifndef REFLEX_SRC_LEXER_TOKEN_H_
#define REFLEX_SRC_LEXER_TOKEN_H_

#include <string>
#include <ostream>
#include "TokenType.h"

namespace reflex {

class SourceLocation;

class Token {
    TokenType tokenType;
    std::string lexeme;
    const SourceLocation *loc;
  public:
    Token(TokenType::Value tokenType, std::string lexeme, const SourceLocation *loc);

    [[nodiscard]]
    const std::string &getLexeme() const { return lexeme; }
    [[nodiscard]] const SourceLocation *getLocInfo() const { return loc; }
    [[nodiscard]] TokenType getTokenType() const { return tokenType; }

    [[nodiscard]] std::string getTokenTypeString() const;
    [[nodiscard]] std::string toString() const;

    [[nodiscard]] bool isBasicLiteral() const;
    [[nodiscard]] bool isUnaryOp() const;

    [[nodiscard]] int getTokenPrec() const;
    [[nodiscard]] bool isBinaryOp() const;

    [[nodiscard]] bool isAssignment() const;
    [[nodiscard]] bool isPostfixOp() const;

    [[nodiscard]] bool isDeclaration() const;
    [[nodiscard]] bool isTrivial() const;

    friend std::ostream &operator<<(std::ostream &os, const Token &token);
};

}

#endif //REFLEX_SRC_LEXER_TOKEN_H_
