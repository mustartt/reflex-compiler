//
// Created by henry on 2022-04-24.
//

#ifndef REFLEX_SRC_PARSER_PARSER_H_
#define REFLEX_SRC_PARSER_PARSER_H_

#include <utility>
#include <vector>
#include <string>
#include <memory>
#include <exception>

#include <SourceManager.h>
#include <Token.h>
#include <ErrorHandler.h>
#include <ASTType.h>
#include <ParsingError.h>

namespace reflex {

class ASTContext;
class Lexer;
class Parser;

/// UnrecoverableError is thrown to propagate unrecoverable parsing error to the
/// parent parsing context to handle
class UnrecoverableError : public std::exception {
  public:
    UnrecoverableError(const SourceLocation *loc, std::string msg)
        : loc(loc), msg(std::move(msg)) {}
    const SourceLocation *getErrorLocation() const { return loc; }
    const std::string &getErrorMessage() const { return msg; }
  private:
    const SourceLocation *loc;
    std::string msg;
};

class ParsingContext final {
  public:
    explicit ParsingContext(Parser &parser, std::string context)
        : parser(parser), context(std::move(context)) {}
    ParsingContext(const ParsingContext &) = delete;
    ParsingContext(ParsingContext &&) = default;

  private:
    Parser &parser;
    std::string context;
};

class Parser final {
    struct ParserState {
      int depth = 0;
    };
    friend class ErrorHandler;
    friend class ParsingContext;
  public:
    Parser(ASTContext &context, Lexer &lexer)
        : context(context), lexer(lexer), lookahead(next()), state{} {}

    /// sets lookahead to next available token in the lexer
    /// @note skips all whitespace and comment tokens
    Token next();

    [[nodiscard]] bool check(TokenType::Value tokenType) const;

    /// check if lookahead is of @param expectedType returns the consumed token
    /// @param handler invokes the resume handler if token type does not match
    ///                if handler is not provided, then throws the ExpectToken
    Token expect(TokenType::Value expectedType, ErrorHandler *handler = nullptr);

  public:
    ASTTypeExpr *parseType();
    ArrayTypeExpr *parseArrayType();
    ArrayTypeExpr *parseElementType1(ArrayTypeExpr *baseTyp);
    FunctionTypeExpr *parseFunctionType();
    std::vector<ASTTypeExpr *> parseParamTypeList();

  private:
    ASTContext &context;
    Lexer &lexer;
    Token lookahead;
    ParserState state;

    std::vector<ParsingContext> contextStack;
    std::vector<std::unique_ptr<ParsingErrorMessage>> errorList;
};

}

#endif //REFLEX_SRC_PARSER_PARSER_H_
