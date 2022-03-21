//
// Created by henry on 2022-03-20.
//

#ifndef REFLEX_SRC_LEXER_TOKEN_H_
#define REFLEX_SRC_LEXER_TOKEN_H_

#include <string>
#include <ostream>
#include "TokenType.h"

namespace reflex {

class Source;

class Loc {
    Source *source;
    size_t line;
    size_t col;
    size_t size;
  public:
    Loc() = default;
    Loc(Source *source, size_t line, size_t col, size_t size);

    [[nodiscard]] std::string getFormattedRepr() const;
    void highlightToken(std::ostream &ostream) const;

    [[nodiscard]] size_t getLine() const { return line; }
    [[nodiscard]] size_t getCol() const { return col; }
    [[nodiscard]] size_t getSize() const { return size; }
};

class Token {
    TokenType tokenType{TokenType::EndOfFile};
    std::string lexeme;
    Loc loc{};
  public:
    Token() = default;
    Token(TokenType::Value tokenType, std::string lexeme, Loc loc);

    [[nodiscard]] Loc getLocInfo() const { return loc; }
    [[nodiscard]] TokenType getTokenType() const { return tokenType; }

    [[nodiscard]] std::string getTokenTypeString() const;
    [[nodiscard]] std::string toString() const;

    friend std::ostream &operator<<(std::ostream &os, const Token &token);
};

}

#endif //REFLEX_SRC_LEXER_TOKEN_H_
