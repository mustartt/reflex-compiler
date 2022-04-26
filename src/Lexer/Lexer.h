//
// Created by henry on 2022-03-20.
//

#ifndef REFLEX_SRC_LEXER_LEXER_H_
#define REFLEX_SRC_LEXER_LEXER_H_

#include <string>
#include <regex>
#include "../Source/Token.h"
#include <TokenDesc.h>

namespace reflex {

using TokenRegexDesc = std::vector<std::pair<TokenType, std::regex>>;

class LexerError : public std::runtime_error {
  public:
    explicit LexerError(const std::string &arg);
};

class SourceFile;

class Lexer {
    struct LexerState {
      size_t index;
      size_t line;
      size_t col;
    };
  public:
    explicit Lexer(SourceFile &source,
                   std::string content,
                   const TokenDesc &tokDesc,
                   KeywordDesc keyDesc);

    [[nodiscard]] bool hasNext() const;
    Token nextToken();
    SourceFile &getSource() { return source; }
  private:
    void updateInternalState(const std::string &lexeme);
    LexerState getLastValidSourceLoc();

    SourceFile &source;
    std::string content;
    LexerState state;
    TokenRegexDesc tokenDesc;
    KeywordDesc keywordDesc;
};

}

#endif //REFLEX_SRC_LEXER_LEXER_H_
