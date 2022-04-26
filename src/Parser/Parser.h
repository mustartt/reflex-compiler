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

#include <Token.h>
#include <SourceManager.h>
#include <ErrorHandler.h>
#include <ParsingError.h>
#include <ASTType.h>
#include <ASTLiteral.h>
#include <ASTDeclaration.h>
#include <ASTStatement.h>

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
    Parser(ASTContext &context, Lexer &lex);

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
    BaseTypenameExpr *parseBaseTypenameType();
    QualifiedTypenameExpr *parseQualifiedType(ReferenceTypenameExpr *expr);
    ArrayTypeExpr *parseArrayType();
    ArrayTypeExpr *parseElementType1(ArrayTypeExpr *baseType);
    FunctionTypeExpr *parseFunctionType();
    std::vector<ASTTypeExpr *> parseParamTypeList();

    Literal *parseLiteral();
    Literal *parseBasicLit();
    NumberLiteral *parseNumberLit();
    StringLiteral *parseStringLit();
    BooleanLiteral *parseBoolLit();
    NullLiteral *parseNullLit();

    Literal *parseArrayLit();
    std::vector<Expression *> parseLiteralValue();
    std::vector<Expression *> parseElementList();
    Expression *parseElement();
    Literal *parseFunctionLit();

    std::pair<std::vector<ParamDecl *>, ASTTypeExpr *> parseSignature();
    std::vector<ParamDecl *> parseParamList();
    ParamDecl *parseFuncParam();

    Expression *parseExpr();
    Identifier *parseBaseDeclRef();
    ModuleSelector *parseModuleSelector(DeclRefExpr *base);
    Expression *parseNamedOperand();
    Expression *parseExpr1(int minPrec, Expression *lhs);
    Expression *parseUnaryExpr();
    Expression *parseOperand();
    Expression *parsePrimaryExpr1(Expression *base);
    Expression *parsePrimaryExpr2(Expression *base);
    Expression *parseNewExpr();
    Expression *parseConversion();
    Expression *parsePrimaryExpr();
    Expression *parseSelectorExpr(Expression *base);
    IndexExpr *parseIndexExpr(Expression *base);
    std::vector<Expression *> parseArgumentList();
    ArgumentExpr *parseArgument(Expression *base);

    std::vector<Statement *> parseStmtList();
    Statement *parseStatement();

    BlockStmt *parseBlockStmt();

    DeclStmt *parseDeclStmt();

    Statement *parseReturnStmt();
    Statement *parseBreakStmt();
    Statement *parseContinueStmt();
    Statement *parseIfStmt();
    Statement *parseForStmt();
    ForClause *parseForClause();
    Statement *parseWhileStmt();
    SimpleStmt *parseSimpleStmt();

    FunctionDecl *parseFunctionDecl();
    MethodDecl *parseMethodDecl(Visibility visibility, AggregateDecl *parent);
    VariableDecl *parseVariableDecl();
    FieldDecl *parseFieldDecl(Visibility visibility, ClassDecl *parent);

    ReferenceTypenameExpr *parseBaseClass();
    ReferenceTypenameExpr *parseDerivedInterface();
    std::vector<ReferenceTypenameExpr *> parseInterfaceList();

    ClassDecl *parseClassDecl(Visibility visibility);
    InterfaceDecl *parseInterfaceDecl(Visibility visibility);
    void parseClassBody(ClassDecl *klass);
    void parseInterfaceBody(InterfaceDecl *interface);

    CompilationUnit *parseCompilationUnit();

    std::string parseString();
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
