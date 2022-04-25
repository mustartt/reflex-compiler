//
// Created by henry on 2022-04-24.
//

#ifndef REFLEX_SRC_PARSER_PARSINGERROR_H_
#define REFLEX_SRC_PARSER_PARSINGERROR_H_

#include <Token.h>
#include <ostream>

namespace reflex {

class SourceLocation;

class ParsingErrorMessage {
  public:
    explicit ParsingErrorMessage(const SourceLocation *loc) : loc(loc) {}
    virtual ~ParsingErrorMessage() = default;

    virtual void printErrorMessage(std::ostream &os) const = 0;
  private:
    const SourceLocation *loc;
};

class ParsingExpectedTokenError : public ParsingErrorMessage {
  public:
    ParsingExpectedTokenError(const SourceLocation *loc, TokenType expectedType, Token actualToken)
        : ParsingErrorMessage(loc),
          expectedType(expectedType), actualToken(std::move(actualToken)) {}

    void printErrorMessage(std::ostream &os) const override;

  private:
    TokenType expectedType;
    Token actualToken;
};

}

#endif //REFLEX_SRC_PARSER_PARSINGERROR_H_
