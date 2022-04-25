//
// Created by henry on 2022-04-24.
//

#ifndef REFLEX_SRC_PARSER_ERRORHANDLER_H_
#define REFLEX_SRC_PARSER_ERRORHANDLER_H_

#include <TokenType.h>

namespace reflex {

class Parser;

class ErrorHandler {
  public:
    explicit ErrorHandler(Parser &parser) : parser(parser) {}
    virtual ~ErrorHandler() = default;

    virtual void resumeParsingHandler() = 0;
  private:
    Parser &parser;
};

class TokenSyncErrorHandler : public ErrorHandler {
  public:
    TokenSyncErrorHandler(Parser &parser, const TokenType &syncTokenType)
        : ErrorHandler(parser), syncTokenType(syncTokenType) {}
  private:
    TokenType syncTokenType;
};

}

#endif //REFLEX_SRC_PARSER_ERRORHANDLER_H_
