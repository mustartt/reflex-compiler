//
// Created by henry on 2022-03-20.
//

#ifndef REFLEX_SRC_LEXER_TOKEN_H_
#define REFLEX_SRC_LEXER_TOKEN_H_

#include <string>
#include "TokenType.h"

namespace reflex {

class Source;

class Loc {
    Source *source;
    size_t line;
    size_t col;
    size_t size;
  public:
    Loc() = delete;
    Loc(Source *source, size_t line, size_t col, size_t size);

    [[nodiscard]] std::string getFormattedRepr() const;
    void highlightToken(std::ostream &ostream) const;

    [[nodiscard]] size_t getLine() const { return line; }
    [[nodiscard]] size_t getCol() const { return col; }
    [[nodiscard]] size_t getSize() const { return size; }
};

class Token {
    TokenType tokenType;
    std::string lexeme;
    Loc loc;
  public:
    Token(TokenType tokenType, std::string lexeme, Loc loc);

};

}

#endif //REFLEX_SRC_LEXER_TOKEN_H_
