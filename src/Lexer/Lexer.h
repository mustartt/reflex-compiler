//
// Created by henry on 2022-03-20.
//

#ifndef REFLEX_SRC_LEXER_LEXER_H_
#define REFLEX_SRC_LEXER_LEXER_H_

#include <string>
#include <regex>
#include "Token.h"
#include "TokenDesc.h"

namespace reflex {

using TokenRegexDesc = std::vector<std::pair<TokenType, std::regex>>;

class LexerError : public std::runtime_error {
  public:
    explicit LexerError(const std::string &arg);
};

class Lexer {
    Source *source;
    std::string content;
    size_t index;
    size_t line = 1;
    size_t col = 1;
    TokenRegexDesc tokenDesc;
    KeywordDesc keywordDesc;
  public:
    explicit Lexer(Source *source,
                   std::string content,
                   const TokenDesc &tokDesc,
                   KeywordDesc keyDesc);

    [[nodiscard]] bool hasNext() const;
    Token nextToken();
    Loc getCurrentPosition() const;
};

}

#endif //REFLEX_SRC_LEXER_LEXER_H_
