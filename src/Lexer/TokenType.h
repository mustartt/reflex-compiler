//
// Created by henry on 2022-03-20.
//

#ifndef REFLEX_SRC_LEXER_TOKENTYPE_H_
#define REFLEX_SRC_LEXER_TOKENTYPE_H_

namespace reflex {

enum class TokenType {
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

}

#endif //REFLEX_SRC_LEXER_TOKENTYPE_H_
