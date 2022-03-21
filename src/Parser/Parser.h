//
// Created by henry on 2022-03-21.
//

#ifndef REFLEX_SRC_PARSER_PARSER_H_
#define REFLEX_SRC_PARSER_PARSER_H_

#include <stdexcept>
#include "../AST/Ast.h"
#include "../Lexer/Token.h"

namespace reflex {

class ParsingError : public std::runtime_error {
  public:
    explicit ParsingError(const std::string &msg);
};

class ExpectToken : public ParsingError {
  public:
    ExpectToken(TokenType type, const Token &token);
};

class Lexer;
class Parser {
    Lexer *lexer;
    Token tok;
  public:
    explicit Parser(Lexer *lexer);

    Token next();
    [[nodiscard]] bool check(TokenType::Value tokenType) const;
    Token expect(TokenType::Value expectedType);
};

}

#endif //REFLEX_SRC_PARSER_PARSER_H_
