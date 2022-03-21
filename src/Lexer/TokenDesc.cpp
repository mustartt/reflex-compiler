//
// Created by henry on 2022-03-20.
//

#include "TokenDesc.h"

namespace reflex {

TokenDesc getTokenDescription() {
    return {
        {TokenType::WhiteSpace, "\\s+"},
        {TokenType::NameSeparator, "\\:\\:"},
        {TokenType::Colon, "\\:"},
        {TokenType::SemiColon, "\\;"},
        {TokenType::Comma, "\\,"},
        {TokenType::Period, "\\."},
        {TokenType::Complement, "\\~"},
        {TokenType::Compare, "=="},
        {TokenType::CompareNot, "\\!="},
        {TokenType::CompareLessEqual, "\\<="},
        {TokenType::CompareGreaterEqual, "\\>="},
        {TokenType::PostInc, "\\+\\+"},
        {TokenType::PostDec, "\\-\\-"},
        {TokenType::LAngleBracket, "<"},
        {TokenType::RAngleBracket, ">"},
        {TokenType::ReturnArrow, "->"},
        {TokenType::AssignAdd, "\\+="},
        {TokenType::AssignSub, "\\-="},

        {TokenType::Add, "\\+"},
        {TokenType::Sub, "\\-"},
        {TokenType::Star, "\\*"},
        {TokenType::Div, "\\/"},
        {TokenType::Mod, "\\%"},
        {TokenType::LogicalOr, "\\|"},
        {TokenType::LogicalAnd, "\\&"},
        {TokenType::LogicalNot, "\\!"},
        {TokenType::Assign, "\\="},

        {TokenType::LBracket, "\\["},
        {TokenType::RBracket, "\\["},
        {TokenType::LParen, "\\("},
        {TokenType::RParen, "\\)"},
        {TokenType::LBrace, "\\{"},
        {TokenType::RBrace, "\\}"},

        {TokenType::Annotation, "@"},

        {TokenType::SingleComment, "//.*"},
        {TokenType::MultiComment, R"(/\*[\s\S]*?\*/)"},

        {TokenType::Identifier, "[_a-zA-Z][_a-zA-Z0-9]*"},
        {TokenType::NumberLiteral, "([0-9]*[.])?[0-9]+"},
        {TokenType::StringLiteral, R"(\"[^\"]*\)"},
    };
}

KeywordDesc getKeywordDescription() {
    return {
        {"const", TokenType::Const},
        {"var", TokenType::Var},
        {"func", TokenType::Func},
        {"return", TokenType::Return},
        {"true", TokenType::BoolLiteral},
        {"false", TokenType::BoolLiteral},
        {"null", TokenType::NullLiteral},
        {"class", TokenType::Class},
        {"new", TokenType::New},
        {"interface", TokenType::Interface},
        {"extends", TokenType::Extends},
        {"implements", TokenType::Implements},
        {"or", TokenType::Or},
        {"and", TokenType::And},
        {"if", TokenType::If},
        {"else", TokenType::Else},
        {"for", TokenType::For},
        {"while", TokenType::While},
        {"cast", TokenType::Cast},
        {"try", TokenType::Try},
        {"in", TokenType::In},
        {"catch", TokenType::Catch},
        {"break", TokenType::Break},
        {"continue", TokenType::Continue}
    };
}

}

