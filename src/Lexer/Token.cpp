//
// Created by henry on 2022-03-20.
//

#include "Token.h"

#include <sstream>

#include "SourceManager.h"

namespace reflex {

Loc::Loc(Source *source, size_t line, size_t col, size_t size)
    : source(source), line(line), col(col), size(size) {}

std::string Loc::getFormattedRepr() const {
    std::stringstream output;
    output << "line " << line << ", " << col << ":" << size;
    return output.str();
}

void Loc::highlightToken(std::ostream &ostream) const {
    ostream << std::endl;
    std::string padding = "     ";

    auto[tokLine, tokStart] = source->getSurroundingRegion(line, col, size);
    ostream << padding << tokLine << padding << std::endl;
    for (size_t i = 0; i < padding.length() + tokStart - 1; ++i) ostream << " ";
    for (size_t i = 0; i < size; ++i) ostream << "^";

    ostream << std::endl;
}

Token::Token(TokenType::Value tokenType, std::string lexeme, Loc loc)
    : tokenType(tokenType), lexeme(std::move(lexeme)), loc(loc) {}

std::string Token::getTokenTypeString() const {
    return tokenType.getTypeString();
}

std::string Token::toString() const {
    return "<\'" + tokenType.getTypeString() + "\' : \'" + lexeme +
        "\' " + loc.getFormattedRepr() + ">";
}

std::ostream &operator<<(std::ostream &os, const Token &token) {
    os << token.toString();
    return os;
}

bool Token::isBasicLiteral() const {
    auto val = tokenType.getValue();
    return val == TokenType::BoolLiteral
        || val == TokenType::NumberLiteral
        || val == TokenType::StringLiteral
        || val == TokenType::NullLiteral;
}

}
