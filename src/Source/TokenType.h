//
// Created by henry on 2022-03-20.
//

#ifndef REFLEX_SRC_LEXER_TOKENTYPE_H_
#define REFLEX_SRC_LEXER_TOKENTYPE_H_

#include <string>
#include <cstdint>

namespace reflex {

class TokenType {
  public:
    enum Value : uint8_t {
      EndOfFile,
      WhiteSpace,
      NameSeparator,
      Colon,
      SemiColon,
      Comma,
      Period,
      Complement,
      Compare,
      CompareNot,
      CompareLessEqual,
      CompareGreaterEqual,
      PostInc,
      PostDec,
      LAngleBracket,
      RAngleBracket,
      ReturnArrow,
      AssignAdd,
      AssignSub,
      Add,
      Sub,
      Star,
      Div,
      Mod,
      LogicalOr,
      LogicalAnd,
      LogicalNot,
      Assign,
      LBracket,
      RBracket,
      LParen,
      RParen,
      LBrace,
      RBrace,
      Annotation,
      SingleComment,
      MultiComment,
      Identifier,
      NumberLiteral,
      StringLiteral,
      Const,
      Var,
      Func,
      Return,
      BoolLiteral,
      NullLiteral,
      Class,
      New,
      Interface,
      Extends,
      Implements,
      Or,
      And,
      If,
      Else,
      For,
      While,
      Cast,
      Try,
      In,
      Catch,
      Break,
      Continue
    };
  public:
    TokenType() = default;
    constexpr explicit TokenType(Value tokType) : value(tokType) {}
    constexpr explicit operator Value() const { return value; }
    TokenType &operator=(Value v) {
        value = v;
        return *this;
    }
    explicit operator bool() = delete;

    constexpr bool operator==(TokenType a) const { return value == a.value; }
    constexpr bool operator!=(TokenType a) const { return value != a.value; }

    [[nodiscard]] std::string getTypeString() const;
    [[nodiscard]] TokenType::Value getValue() const;
  private:
    Value value;
};

}

#endif //REFLEX_SRC_LEXER_TOKENTYPE_H_
