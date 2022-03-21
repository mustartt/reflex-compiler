//
// Created by henry on 2022-03-20.
//

#include "Lexer.h"

namespace reflex {

LexerError::LexerError(const std::string &arg) : runtime_error(arg) {}

Lexer::Lexer(Source *source, std::string content, const TokenDesc &tokDesc, KeywordDesc keyDesc)
    : index(0), source(source), content(std::move(content)), keywordDesc(std::move(keyDesc)) {
    for (const auto&[type, regexExpr]: tokDesc) {
        std::regex parsedRegex{"^" + regexExpr};
        tokenDesc.emplace_back(type, std::move(parsedRegex));
    }
}

bool Lexer::hasNext() const {
    return index < content.size();
}

std::string getErrorTokenLookahead(const std::string &content, size_t lookahead = 20) {
    size_t end = content.length();
    return content.substr(0, std::min(end, lookahead));
}

Token Lexer::nextToken() {
    if (!hasNext()) throw LexerError("Expected Token but got EOF instead.");

    const std::string current = content.substr(index);

    for (const auto &[type, regex]: tokenDesc) {
        std::smatch matches;
        if (std::regex_search(content, matches, regex)) {
            index += matches[0].length();
            auto lexeme = matches[0].str();
            if (lexeme.starts_with('\n')) {
                line += (int)std::count(lexeme.begin(), lexeme.end(), '\n');
                col = 1;
            } else {
                col += (int)lexeme.length();
            }
            Loc loc(source, line, col, lexeme.length());
            if (type == TokenType::Identifier) {
                return {keywordDesc.at(lexeme), lexeme, loc};
            }
            return {type, lexeme, loc};
        }
    }

    throw LexerError("Unknown Token: " + getErrorTokenLookahead(content) + ".");
}

}
