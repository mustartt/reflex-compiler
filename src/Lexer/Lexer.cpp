//
// Created by henry on 2022-03-20.
//

#include "Lexer.h"

#include "../Source/SourceManager.h"

namespace reflex {

LexerError::LexerError(const std::string &arg) : runtime_error(arg) {}

Lexer::Lexer(SourceFile &source, std::string content, const TokenDesc &tokDesc, KeywordDesc keyDesc)
    : state{0, 1, 1}, source(source), content(std::move(content)), keywordDesc(std::move(keyDesc)) {
    for (const auto&[type, regexExpr]: tokDesc) {
        std::regex parsedRegex{"^(" + regexExpr + ")"};
        tokenDesc.emplace_back(type, std::move(parsedRegex));
    }
}

bool Lexer::hasNext() const {
    return state.index < content.size();
}

std::string getErrorTokenLookahead(const std::string &content, size_t lookahead = 20) {
    size_t end = content.length();
    return content.substr(0, std::min(end, lookahead));
}

void Lexer::updateInternalState(const std::string &lexeme) {
    state.index += lexeme.length();
    for (const char c: lexeme) {
        if (c == '\n') {
            state.line += 1;
            state.col = 1;
        } else {
            state.col += 1;
        }
    }
}

Lexer::LexerState Lexer::getLastValidSourceLoc() {
    if (state.col == 1) {
        if (state.line == 1) return {0, 1, 1};
        return {state.index,
                state.line - 1,
                source.line(state.line - 1).size() + 1};
    }
    return state;
}

Token Lexer::nextToken() {
    if (!hasNext())
        return {TokenType::EndOfFile, "EOF", source.getLastValidPosition()};

    const std::string current = content.substr(state.index);

    for (const auto &[type, regex]: tokenDesc) {
        std::smatch matches;
        if (std::regex_search(current, matches, regex)) {
            auto lexeme = matches[0].str();
            auto lastState = state;
            updateInternalState(lexeme);
            auto currState = getLastValidSourceLoc();
            const auto &loc = SourceManager::createSourceLocation(
                source,
                lastState.line, lastState.col,
                currState.line, currState.col - 1
            );
            if (type.getValue() == TokenType::Identifier && keywordDesc.count(lexeme)) {
                return {keywordDesc[lexeme].getValue(), lexeme, loc};
            }
            return {type.getValue(), lexeme, loc};
        }
    }

    throw LexerError("Unknown Token: " + getErrorTokenLookahead(current) + ".");
}

}
