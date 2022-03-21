//
// Created by henry on 2022-03-21.
//

#ifndef REFLEX_SRC_PARSER_PARSER_H_
#define REFLEX_SRC_PARSER_PARSER_H_

#include <stdexcept>
#include "../AST/Ast.h"
#include "../AST/AstNodes.h"
#include "../Lexer/Token.h"

namespace reflex {

class ParsingError : public std::runtime_error {
  public:
    explicit ParsingError(const std::string &msg);
};

class ExpectToken : public ParsingError {
  public:
    ExpectToken(TokenType type, const Token &token);
};

class Lexer;
class AstContextManager;
class Parser {
    AstContextManager *ctx;
    Lexer *lexer;
    Token tok;
  public:
    explicit Parser(Lexer *lexer, AstContextManager *ctx);

    Token next();
    [[nodiscard]] bool check(TokenType::Value tokenType) const;
    Token expect(TokenType::Value expectedType);

  public:
    IdentExpr *parseIdent();
    IdentExpr *parseModuleIdent(IdentExpr *base);

    TypeExpr *parseType();
    ArrayType *parseArrayType();
    ArrayType *parseElementType1(ArrayType *baseTyp);
    FunctionType *parseFunctionType();
    std::vector<TypeExpr *> parseParamTypeList();

    Literal *parseLiteral();
    BasicLiteral *parseBasicLit();
    NumberLit *parseNumberLit();
    StringLit *parseStringLit();
    BoolLit *parseBoolLit();
    NullLit *parseNullLit();

    ArrayLit *parseArrayLit();
    AstExpr *parseLiteralValue();
    AstExpr *parseElementList();
    AstExpr *parseElement();
    AstExpr *parseFunctionLit();
    AstExpr *parseSignature();
    AstExpr *parseParamList();
    AstExpr *parseFuncParam();

    Expression *parseExpr();
    AstExpr *parseExpr1(int tokePrec, AstExpr *lhs);
    AstExpr *parseUnaryExpr();
    AstExpr *parsePrimaryExpr1(AstExpr *base);
    AstExpr *parsePrimaryExpr2(AstExpr *base);
    AstExpr *parseNamedOperand();
    AstExpr *parseNewExpr();
    AstExpr *parseConversion();
    AstExpr *parsePrimaryExpr();
    AstExpr *parseSelectorExpr(AstExpr *base);
    AstExpr *parseIndexExpr(AstExpr *base);
    AstExpr *parseArgumentList();
    AstExpr *parseArgument(AstExpr *base);

    AstExpr *parseBlock();
    AstExpr *parseStmtList();
    AstExpr *parseStatement();

    AstExpr *parseDeclaration();
    AstExpr *parseTypeDecl();
    AstExpr *parseVarDecl();
    AstExpr *parseReturnStmt();
    AstExpr *parseBreakStmt();
    AstExpr *parseContinueStmt();
    AstExpr *parseBlockStmt();
    AstExpr *parseIfStmt();
    AstExpr *parseForStmt();
    AstExpr *parseForClause();
    AstExpr *parseWhileStmt();
    AstExpr *parseSimpleStmt();

    AstExpr *parseFunctionDecl();

    AstExpr *parseInheritanceList();
    AstExpr *parseInheritance();
    AstExpr *parseClassDecl();
    AstExpr *parseClassBody();
    AstExpr *parseClassBodyDecl();
    AstExpr *parseInterfaceDecl();
    AstExpr *parseAnnotationDecl();

};

}

#endif //REFLEX_SRC_PARSER_PARSER_H_
