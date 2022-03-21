//
// Created by henry on 2022-03-20.
//

#ifndef REFLEX_SRC_LEXER_TOKENDESC_H_
#define REFLEX_SRC_LEXER_TOKENDESC_H_

#include <vector>
#include <unordered_map>
#include <string>

#include "TokenType.h"

namespace reflex {

using TokenDesc = std::vector<std::pair<TokenType, std::string>>;
using KeywordDesc = std::unordered_map<std::string, TokenType>;

TokenDesc getTokenDescription();
KeywordDesc getKeywordDescription();

}

#endif //REFLEX_SRC_LEXER_TOKENDESC_H_
