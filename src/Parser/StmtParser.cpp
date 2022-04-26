//
// Created by henry on 2022-04-26.
//

#include "Parser.h"

#include "ASTContext.h"
#include "Operator.h"

namespace reflex {

BlockStmt *Parser::parseBlockStmt() {
    auto start = expect(TokenType::LBrace);
    auto stmts = parseStmtList();
    auto end = expect(TokenType::RBrace);
    return context.create<BlockStmt>(
        SourceManager::mergeSourceLocation(start.getLocInfo(), end.getLocInfo()),
        std::move(stmts)
    );
}

std::vector<Statement *> Parser::parseStmtList() {
    std::vector<Statement *> stmts;
    while (!check(TokenType::RBrace)) {
        stmts.push_back(parseStatement());
    }
    return stmts;
}

SimpleStmt *Parser::parseSimpleStmt() {
    if (check(TokenType::SemiColon)) {
        return context.create<EmptyStmt>(lookahead.getLocInfo());
    }
    auto expr = parseExpr();
    if (lookahead.isAssignment()) {
        auto op = lookahead;
        next();
        auto value = parseExpr();
        return context.create<AssignmentStmt>(
            op.getLocInfo(),
            Operator::createAssignOperatorFromToken(op),
            expr, value
        );
    }
    if (lookahead.isPostfixOp()) {
        auto postfix = lookahead;
        next();
        return context.create<IncDecStmt>(
            postfix.getLocInfo(),
            Operator::createPostfixOperatorFromToken(postfix),
            expr
        );
    }
    return context.create<ExpressionStmt>(expr->location(), expr);
}

Statement *Parser::parseStatement() {
    if (lookahead.isDeclaration()) {
        auto isVarDecl = check(TokenType::Var);
        auto ret = parseDeclStmt();
        if (isVarDecl) expect(TokenType::SemiColon);
        return ret;
    }
    if (check(TokenType::Return)) {
        auto ret = parseReturnStmt();
        expect(TokenType::SemiColon);
        return ret;
    }
    if (check(TokenType::Break)) {
        auto ret = parseBreakStmt();
        expect(TokenType::SemiColon);
        return ret;
    }
    if (check(TokenType::Continue)) {
        auto ret = parseContinueStmt();
        expect(TokenType::SemiColon);
        return ret;
    }
    if (check(TokenType::LBrace)) {
        return parseBlockStmt();
    }
    if (check(TokenType::If)) {
        return parseIfStmt();
    }
    if (check(TokenType::While)) {
        return parseWhileStmt();
    }
    if (check(TokenType::For)) {
        return parseForStmt();
    }
    auto ret = parseSimpleStmt();
    expect(TokenType::SemiColon);
    return ret;
}

Statement *Parser::parseReturnStmt() {
    auto token = lookahead;
    expect(TokenType::Return);
    return context.create<ReturnStmt>(
        token.getLocInfo(),
        parseExpr()
    );
}

Statement *Parser::parseBreakStmt() {
    auto token = lookahead;
    expect(TokenType::Break);
    return context.create<BreakStmt>(
        token.getLocInfo()
    );
}

Statement *Parser::parseContinueStmt() {
    auto token = lookahead;
    expect(TokenType::Continue);
    return context.create<ContinueStmt>(
        token.getLocInfo()
    );
}

Statement *Parser::parseIfStmt() {
    auto startToken = lookahead;
    expect(TokenType::If);
    expect(TokenType::LParen);
    auto cond = parseSimpleStmt();
    expect(TokenType::RParen);
    BlockStmt *bodyBlock = parseBlockStmt();
    BlockStmt *elseBlock = nullptr;
    if (check(TokenType::Else)) {
        elseBlock = parseBlockStmt();
    }
    return context.create<IfStmt>(
        startToken.getLocInfo(),
        cond, bodyBlock, elseBlock
    );
}

Statement *Parser::parseForStmt() {
    auto startToken = lookahead;
    expect(TokenType::For);
    expect(TokenType::LParen);
    auto clause = parseForClause();
    expect(TokenType::RParen);
    return context.create<ForStmt>(
        startToken.getLocInfo(),
        clause, parseBlockStmt()
    );
}

ForClause *Parser::parseForClause() {
    // todo: impl parsing for ForClause
    return nullptr;
}

Statement *Parser::parseWhileStmt() {
    auto startToken = lookahead;
    expect(TokenType::While);
    expect(TokenType::LParen);
    auto cond = parseSimpleStmt();
    expect(TokenType::RParen);
    return context.create<WhileStmt>(
        startToken.getLocInfo(),
        cond, parseBlockStmt()
    );
}

}
