//
// Created by henry on 2022-03-20.
//

#include "TokenDesc.h"

namespace reflex {

TokenDesc getTokenDescription() {
    TokenDesc desc;
    desc.emplace_back(TokenType::WhiteSpace, "\\s+");
    desc.emplace_back(TokenType::NameSeparator, "\\:\\:");
    desc.emplace_back(TokenType::Colon, "\\:");
    desc.emplace_back(TokenType::SemiColon, "\\;");
    desc.emplace_back(TokenType::Comma, "\\,");
    desc.emplace_back(TokenType::Period, "\\.");
    desc.emplace_back(TokenType::Complement, "\\~");
    desc.emplace_back(TokenType::Compare, "==");
    desc.emplace_back(TokenType::CompareNot, "\\!=");
    desc.emplace_back(TokenType::CompareLessEqual, "\\<=");
    desc.emplace_back(TokenType::CompareGreaterEqual, "\\>=");
    desc.emplace_back(TokenType::PostInc, "\\+\\+");
    desc.emplace_back(TokenType::PostDec, "\\-\\-");
    desc.emplace_back(TokenType::LAngleBracket, "<");
    desc.emplace_back(TokenType::RAngleBracket, ">");
    desc.emplace_back(TokenType::ReturnArrow, "->");
    desc.emplace_back(TokenType::AssignAdd, "\\+=");
    desc.emplace_back(TokenType::AssignSub, "\\-=");

    desc.emplace_back(TokenType::Add, "\\+");
    desc.emplace_back(TokenType::Sub, "\\-");
    desc.emplace_back(TokenType::Star, "\\*");
    desc.emplace_back(TokenType::Div, "\\/");
    desc.emplace_back(TokenType::Mod, "\\%");
    desc.emplace_back(TokenType::LogicalOr, "\\|");
    desc.emplace_back(TokenType::LogicalAnd, "\\&");
    desc.emplace_back(TokenType::LogicalNot, "\\!");
    desc.emplace_back(TokenType::Assign, "\\=");
    desc.emplace_back(TokenType::Annotation, "@");

    desc.emplace_back(TokenType::LBracket, "\\[");
    desc.emplace_back(TokenType::RBracket, "\\[");
    desc.emplace_back(TokenType::LParen, "\\(");
    desc.emplace_back(TokenType::RParen, "\\)");
    desc.emplace_back(TokenType::LBrace, "\\{");
    desc.emplace_back(TokenType::RBrace, "\\}");

    desc.emplace_back(TokenType::SingleComment, "//.*");
    desc.emplace_back(TokenType::MultiComment, R"(/\*[\s\S]*?\*/)");
    desc.emplace_back(TokenType::Identifier, "[_a-zA-Z][_a-zA-Z0-9]*");
    desc.emplace_back(TokenType::NumberLiteral, "([0-9]*[.])?[0-9]+");
    desc.emplace_back(TokenType::StringLiteral, R"("[^"]*")");

    return desc;
}

KeywordDesc getKeywordDescription() {
    KeywordDesc desc;
    desc.emplace("const", TokenType::Const);
    desc.emplace("var", TokenType::Var);
    desc.emplace("func", TokenType::Func);
    desc.emplace("return", TokenType::Return);
    desc.emplace("true", TokenType::BoolLiteral);
    desc.emplace("false", TokenType::BoolLiteral);
    desc.emplace("null", TokenType::NullLiteral);
    desc.emplace("class", TokenType::Class);
    desc.emplace("new", TokenType::New);
    desc.emplace("interface", TokenType::Interface);
    desc.emplace("extends", TokenType::Extends);
    desc.emplace("implements", TokenType::Implements);
    desc.emplace("or", TokenType::Or);
    desc.emplace("and", TokenType::And);
    desc.emplace("if", TokenType::If);
    desc.emplace("else", TokenType::Else);
    desc.emplace("for", TokenType::For);
    desc.emplace("while", TokenType::While);
    desc.emplace("cast", TokenType::Cast);
    desc.emplace("try", TokenType::Try);
    desc.emplace("in", TokenType::In);
    desc.emplace("catch", TokenType::Catch);
    desc.emplace("break", TokenType::Break);
    desc.emplace("continue", TokenType::Continue);
    return desc;
}

}

