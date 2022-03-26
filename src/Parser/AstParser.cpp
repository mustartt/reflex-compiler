//
// Created by henry on 2022-03-21.
//

#include "Parser.h"

#include "../AST/AstContextManager.h"

namespace reflex {

Identifier *Parser::parseIdent() {
    auto token = expect(TokenType::Identifier);
    return ctx->create<Identifier>(token.getLocInfo(), token.getLexeme());
}

IdentExpr *Parser::parseModuleIdent(IdentExpr *base) {
    expect(TokenType::NameSeparator);
    IdentExpr *name = parseIdent();
    auto selector = ctx->create<ModuleSelector>(
        name->getLoc(),
        base,
        name->getIdent()
    );
    while (check(TokenType::NameSeparator)) {
        next();
        name = parseIdent();
        selector = ctx->create<ModuleSelector>(
            name->getLoc(),
            selector,
            name->getIdent()
        );
    }
    return selector;
}

TypeExpr *Parser::parseType() {
    if (check(TokenType::LParen)) {
        next();
        auto nestedTyp = parseType();
        expect(TokenType::RParen);
        if (check(TokenType::LBracket)) {
            next();
            auto lenExpr = parseExpr();
            expect(TokenType::RBracket);
            Loc loc = nestedTyp->getLoc();
            auto arrTyp = parseElementType1(ctx->create<ArrayTypeExpr>(loc, nestedTyp, lenExpr));
            return parseElementType1(
                ctx->create<ArrayTypeExpr>(loc, arrTyp, lenExpr)
            );
        }
        return nestedTyp;
    }
    TypeExpr *baseTyp;
    if (!check(TokenType::Func)) {
        IdentExpr *typ = parseIdent();
        if (check(TokenType::NameSeparator)) {
            typ = parseModuleIdent(typ);
        }
        baseTyp = ctx->create<IdentifierTypeExpr>(typ->getLoc(), typ);
    } else {
        baseTyp = parseFunctionType();
    }
    if (check(TokenType::LBracket)) {
        next();
        auto lenExpr = parseExpr();
        expect(TokenType::RBracket);
        return parseElementType1(
            ctx->create<ArrayTypeExpr>(baseTyp->getLoc(), baseTyp, lenExpr)
        );
    }
    return baseTyp;
}

ArrayTypeExpr *Parser::parseArrayType() {
    TypeExpr *base;
    if (check(TokenType::LParen)) {
        next();
        base = parseType();
        expect(TokenType::RParen);
    } else if (check(TokenType::Func)) {
        base = parseFunctionType();
    } else {
        IdentExpr *typ = parseIdent();
        if (check(TokenType::NameSeparator)) {
            typ = parseModuleIdent(typ);
        }
        base = ctx->create<IdentifierTypeExpr>(typ->getLoc(), typ);
    }
    expect(TokenType::LBracket);
    auto lenExpr = parseExpr();
    expect(TokenType::RBracket);
    return parseElementType1(
        ctx->create<ArrayTypeExpr>(base->getLoc(), base, lenExpr)
    );
}

ArrayTypeExpr *Parser::parseElementType1(ArrayTypeExpr *baseTyp) {
    if (check(TokenType::LBracket)) {
        expect(TokenType::LBracket);
        auto lenExpr = parseExpr();
        expect(TokenType::RBracket);
        return parseElementType1(
            ctx->create<ArrayTypeExpr>(baseTyp->getLoc(), baseTyp, lenExpr)
        );
    }
    return baseTyp;
}

FunctionTypeExpr *Parser::parseFunctionType() {
    auto start = expect(TokenType::Func);
    expect(TokenType::LParen);
    auto typeParams = parseParamTypeList();
    auto end = expect(TokenType::RParen);

    TypeExpr *retTyp = ctx->create<IdentifierTypeExpr>(
        end.getLocInfo(),
        ctx->create<Identifier>(end.getLocInfo(), "void")
    );
    if (check(TokenType::ReturnArrow)) {
        next();
        retTyp = parseType();
    }
    return ctx->create<FunctionTypeExpr>(
        start.getLocInfo(),
        retTyp,
        typeParams
    );
}

std::vector<TypeExpr *> Parser::parseParamTypeList() {
    if (check(TokenType::RParen)) {
        return {};
    }
    std::vector<TypeExpr *> params;
    params.push_back(parseType());
    while (!check(TokenType::RParen)) {
        expect(TokenType::Comma);
        params.push_back(parseType());
    }
    return params;
}

Literal *Parser::parseLiteral() {
    if (tok.isBasicLiteral())
        return parseBasicLit();
    if (check(TokenType::LBrace))
        return parseArrayLit();
    return parseFunctionLit();
}

Literal *Parser::parseBasicLit() {
    switch (tok.getTokenType().getValue()) {
        case TokenType::BoolLiteral: return parseBoolLit();
        case TokenType::NumberLiteral: return parseNumberLit();
        case TokenType::StringLiteral: return parseStringLit();
        default: return parseNullLit();
    }
}

NumberLit *Parser::parseNumberLit() {
    auto token = expect(TokenType::NumberLiteral);
    return ctx->create<NumberLit>(token.getLocInfo(), token.getLexeme());
}

StringLit *Parser::parseStringLit() {
    auto token = expect(TokenType::StringLiteral);
    auto &lit = token.getLexeme();
    return ctx->create<StringLit>(
        token.getLocInfo(),
        lit.substr(1, lit.size() - 2)
    );
}

BoolLit *Parser::parseBoolLit() {
    auto token = expect(TokenType::BoolLiteral);
    return ctx->create<BoolLit>(token.getLocInfo(), token.getLexeme());
}

NullLit *Parser::parseNullLit() {
    auto token = expect(TokenType::NullLiteral);
    return ctx->create<NullLit>(token.getLocInfo(), token.getLexeme());
}

Literal *Parser::parseArrayLit() {
    auto startToken = tok;
    return ctx->create<ArrayLit>(
        startToken.getLocInfo(),
        parseLiteralValue()
    );
}

std::vector<AstExpr *> Parser::parseLiteralValue() {
    expect(TokenType::LBrace);
    auto elements = parseElementList();
    expect(TokenType::RBrace);
    return elements;
}

std::vector<AstExpr *> Parser::parseElementList() {
    if (check(TokenType::RBrace))
        return {};
    std::vector<AstExpr *> elements;
    elements.push_back(parseElement());
    while (!check(TokenType::RBrace)) {
        expect(TokenType::Comma);
        elements.push_back(parseElement());
    }
    return elements;
}

AstExpr *Parser::parseElement() {
    if (check(TokenType::LBrace)) {
        return parseArrayLit();
    }
    return parseExpr();
}

Literal *Parser::parseFunctionLit() {
    auto start = expect(TokenType::Func);
    auto[params, ret] = parseSignature();
    auto body = parseBlock();
    return ctx->create<FunctionLit>(
        start.getLocInfo(),
        params, ret, body
    );
}

std::pair<std::vector<ParamDecl *>, TypeExpr *> Parser::parseSignature() {
    expect(TokenType::LParen);
    auto params = parseParamList();
    auto end = expect(TokenType::RParen);
    TypeExpr *retTyp = ctx->create<IdentifierTypeExpr>(
        end.getLocInfo(),
        ctx->create<Identifier>(end.getLocInfo(), "void")
    );
    if (check(TokenType::ReturnArrow)) {
        next();
        retTyp = parseType();
    }
    return {params, retTyp};
}

std::vector<ParamDecl *> Parser::parseParamList() {
    if (check(TokenType::RParen)) {
        return {};
    }
    std::vector<ParamDecl *> params;
    params.push_back(parseFuncParam());
    while (!check(TokenType::RParen)) {
        expect(TokenType::Comma);
        params.push_back(parseFuncParam());
    }
    return params;
}

ParamDecl *Parser::parseFuncParam() {
    auto ident = parseIdent();
    expect(TokenType::Colon);
    return ctx->create<ParamDecl>(
        ident->getLoc(),
        ident,
        parseType()
    );
}

Expression *Parser::parseNamedOperand() {
    if (check(TokenType::Identifier)) {
        IdentExpr *ident = parseIdent();
        if (check(TokenType::NameSeparator)) {
            ident = parseModuleIdent(ident);
        }
        return ident;
    }
    if (check(TokenType::LParen)) {
        next();
        auto subexpr = parseExpr();
        expect(TokenType::RParen);
        return subexpr;
    }
    return parseLiteral();
}

Expression *Parser::parseUnaryExpr() {
    if (tok.isUnaryOp()) {
        auto unaryOp = tok;
        next();
        return ctx->create<UnaryExpr>(
            unaryOp.getLocInfo(),
            createUnaryOperatorFromToken(unaryOp),
            parseExpr()
        );
    }
    return parsePrimaryExpr();
}

Expression *Parser::parseOperand() {
    if (check(TokenType::Identifier)) {
        IdentExpr *ident = parseIdent();
        if (check(TokenType::NameSeparator)) {
            ident = parseModuleIdent(ident);
        }
        return ident;
    }
    if (check(TokenType::LParen)) {
        next();
        auto subexpr = parseExpr();
        expect(TokenType::RParen);
        return subexpr;
    }
    return parseLiteral();
}

Expression *Parser::parsePrimaryExpr() {
    Expression *base;
    if (check(TokenType::New)) {
        base = parseNewExpr();
    } else if (check(TokenType::Cast)) {
        base = parseConversion();
    } else {
        base = parseOperand();
    }
    return parsePrimaryExpr2(base);
}

Expression *Parser::parsePrimaryExpr1(Expression *base) {
    if (check(TokenType::Period)) {
        return parseSelectorExpr(base);
    }
    if (check(TokenType::LBracket)) {
        return parseIndexExpr(base);
    }
    return parseArgument(base);
}

Expression *Parser::parsePrimaryExpr2(Expression *base) {
    auto type = tok.getTokenType().getValue();
    if (type == TokenType::Period ||
        type == TokenType::LBracket ||
        type == TokenType::LParen) {
        auto newbase = parsePrimaryExpr1(base);
        return parsePrimaryExpr2(newbase);
    }
    return base;
}

Expression *Parser::parseNewExpr() {
    expect(TokenType::New);
    auto instanceTyp = parseType();
    return ctx->create<NewExpr>(
        instanceTyp->getLoc(),
        instanceTyp
    );
}

Expression *Parser::parseConversion() {
    auto startToken = expect(TokenType::Cast);
    expect(TokenType::LAngleBracket);
    auto typ = parseType();
    expect(TokenType::RAngleBracket);
    expect(TokenType::LParen);
    auto expr = parseExpr();
    expect(TokenType::RParen);
    return ctx->create<CastExpr>(
        startToken.getLocInfo(),
        typ, expr
    );
}

Expression *Parser::parseSelectorExpr(Expression *base) {
    expect(TokenType::Period);
    auto ident = parseIdent();
    return ctx->create<SelectorExpr>(
        ident->getLoc(),
        base, ident
    );
}

Expression *Parser::parseIndexExpr(Expression *base) {
    auto startToken = expect(TokenType::LBracket);
    auto index = parseExpr();
    expect(TokenType::RBracket);
    return ctx->create<IndexExpr>(
        startToken.getLocInfo(),
        base, index
    );
}

Expression *Parser::parseArgument(Expression *base) {
    auto startToken = expect(TokenType::LParen);
    auto args = parseArgumentList();
    expect(TokenType::RParen);
    return ctx->create<ArgumentExpr>(
        startToken.getLocInfo(),
        base, args
    );
}

std::vector<Expression *> Parser::parseArgumentList() {
    if (check(TokenType::RParen)) {
        return {};
    }
    std::vector<Expression *> args;
    args.push_back(parseExpr());
    while (!check(TokenType::RParen)) {
        expect(TokenType::Comma);
        args.push_back(parseExpr());
    }
    return args;
}

Expression *Parser::parseExpr() {
    if (check(TokenType::RBracket)) return nullptr;     // IndexExpr
    if (check(TokenType::SemiColon)) return nullptr;    // EmptyExpr
    return parseExpr1(0, parseUnaryExpr());
}

Expression *Parser::parseExpr1(int minPrec, Expression *lhs) {
    while (tok.isBinaryOp() && tok.getTokenPrec() >= minPrec) {
        auto binOp = tok;
        next();
        auto rhs = parseUnaryExpr();
        while (tok.getTokenPrec() > binOp.getTokenPrec()) {
            auto prec = binOp.getTokenPrec() + 1;
            rhs = parseExpr1(prec, rhs);
        }
        lhs = ctx->create<BinaryExpr>(
            binOp.getLocInfo(),
            createBinaryOperatorFromToken(binOp),
            lhs, rhs
        );
    }
    return lhs;
}

Block *Parser::parseBlock() {
    auto start = expect(TokenType::LBrace);
    auto stmts = parseStmtList();
    expect(TokenType::RBrace);
    return ctx->create<Block>(
        start.getLocInfo(),
        stmts
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
        return ctx->create<EmptyStmt>(tok.getLocInfo());
    }
    auto expr = parseExpr();
    if (tok.isAssignment()) {
        auto op = tok;
        next();
        auto value = parseExpr();
        return ctx->create<AssignmentStmt>(
            op.getLocInfo(),
            createAssignOperatorFromToken(op),
            expr, value
        );
    }
    if (tok.isPostfixOp()) {
        auto postfix = tok;
        next();
        return ctx->create<IncDecStmt>(
            postfix.getLocInfo(),
            createPostfixOperatorFromToken(postfix),
            expr
        );
    }
    return ctx->create<ExpressionStmt>(expr->getLoc(), expr);
}

Statement *Parser::parseStatement() {
    if (tok.isDeclaration()) {
        auto isVarDecl = check(TokenType::Var);
        auto ret = parseVarOrTypeDecl();
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

Declaration *Parser::parseVarOrTypeDecl() {
    if (check(TokenType::Var)) {
        return parseVarDecl();
    }
    return parseTypeDecl();
}

Declaration *Parser::parseTypeDecl() {
    if (check(TokenType::Interface)) {
        return parseInterfaceDecl();
    }
    if (check(TokenType::Annotation)) {
        return parseAnnotationDecl();
    }
    return parseClassDecl();
}

Declaration *Parser::parseVarDecl() {
    expect(TokenType::Var);
    auto name = parseIdent();
    TypeExpr *varType = nullptr;
    Expression *initializer = nullptr;
    if (check(TokenType::Colon)) {
        next();
        varType = parseType();
    }
    if (check(TokenType::Assign)) {
        next();
        initializer = parseExpr();
    }
    return ctx->create<VariableDecl>(
        name->getLoc(),
        name, varType, initializer
    );
}

Statement *Parser::parseReturnStmt() {
    auto token = tok;
    expect(TokenType::Return);
    return ctx->create<ReturnStmt>(
        token.getLocInfo(),
        parseExpr()
    );
}

Statement *Parser::parseBreakStmt() {
    auto token = tok;
    expect(TokenType::Break);
    return ctx->create<BreakStmt>(
        token.getLocInfo()
    );
}

Statement *Parser::parseContinueStmt() {
    auto token = tok;
    expect(TokenType::Continue);
    return ctx->create<ContinueStmt>(
        token.getLocInfo()
    );
}

Statement *Parser::parseBlockStmt() {
    return parseBlock();
}

Statement *Parser::parseIfStmt() {
    auto startToken = tok;
    expect(TokenType::If);
    expect(TokenType::LParen);
    auto cond = parseSimpleStmt();
    expect(TokenType::RParen);
    Block *bodyBlock = parseBlock();
    Block *elseBlock = nullptr;
    if (check(TokenType::Else)) {
        elseBlock = parseBlock();
    }
    return ctx->create<IfStmt>(
        startToken.getLocInfo(),
        cond, bodyBlock, elseBlock
    );
}
Statement *Parser::parseForStmt() {
    auto startToken = tok;
    expect(TokenType::For);
    expect(TokenType::LParen);
    auto clause = parseForClause();
    expect(TokenType::RParen);
    return ctx->create<ForStmt>(
        startToken.getLocInfo(),
        clause, parseBlock()
    );
}

ForClause *Parser::parseForClause() {
    Statement *init = nullptr;
    if (check(TokenType::Var)) {
        init = parseVarDecl();
        auto varDecl = dynamic_cast<VariableDecl *>(init);
        if (!varDecl) throw ParsingError("For range stmt must be a var decl.");
        if (check(TokenType::In)) {
            next();
            auto rangeExpr = parseExpr();
            return ctx->create<ForRangeClause>(
                varDecl->getLoc(),
                varDecl, rangeExpr
            );
        }
    } else {
        init = parseSimpleStmt();
    }
    expect(TokenType::SemiColon);
    auto cond = parseExpr();
    expect(TokenType::SemiColon);
    auto post = parseSimpleStmt();
    return ctx->create<ForNormalClause>(
        init->getLoc(),
        init, cond, post
    );
}

Statement *Parser::parseWhileStmt() {
    auto startToken = tok;
    expect(TokenType::While);
    expect(TokenType::LParen);
    auto cond = parseSimpleStmt();
    expect(TokenType::RParen);
    return ctx->create<WhileStmt>(
        startToken.getLocInfo(),
        cond, parseBlock()
    );
}

FunctionDecl *Parser::parseFunctionDecl() {
    auto startToken = expect(TokenType::Func);
    auto name = parseIdent();
    auto[params, ret] = parseSignature();
    Block *body = nullptr;
    if (!check(TokenType::SemiColon)) {
        body = parseBlock();
    }
    return ctx->create<FunctionDecl>(
        name->getLoc(),
        name, params, ret, body
    );
}

Declaration *Parser::parseClassDecl() {
    auto startToken = expect(TokenType::Class);
    auto classname = parseIdent();
    auto baseclass = parseBaseClass();
    auto interfaces = parseInterfaceList();
    auto members = parseClassBody();
    return ctx->create<ClassDecl>(
        startToken.getLocInfo(),
        classname, baseclass, interfaces, members
    );
}

IdentExpr *Parser::parseBaseClass() {
    if (!check(TokenType::LParen)) return nullptr;
    next();
    auto baseclass = parseIdent();
    expect(TokenType::RParen);
    return baseclass;
}

std::vector<MemberDecl *> Parser::parseClassBody() {
    auto startToken = expect(TokenType::LBrace);
    std::vector<MemberDecl *> members;
    while (!check(TokenType::RBrace)) {
        members.push_back(parseClassMemberDecl());
    }
    expect(TokenType::RBrace);
    return members;
}

MemberDecl *Parser::parseClassMemberDecl() {
    auto modifier = parseIdent();
    Declaration *decl;
    if (check(TokenType::Func)) {
        decl = parseFunctionDecl();
    } else {
        bool isVarDecl = check(TokenType::Var);
        decl = parseVarOrTypeDecl();
        if (isVarDecl) expect(TokenType::SemiColon);
    }
    return ctx->create<MemberDecl>(
        modifier->getLoc(),
        modifier, decl
    );
}

IdentExpr *Parser::parseDerivedInterface() {
    IdentExpr *ident = parseIdent();
    if (check(TokenType::NameSeparator)) {
        ident = parseModuleIdent(ident);
    }
    return ident;
}

std::vector<IdentExpr *> Parser::parseInterfaceList() {
    if (!check(TokenType::Colon)) return {};
    next();
    std::vector<IdentExpr *> interfaces;
    interfaces.push_back(parseDerivedInterface());
    while (check(TokenType::Comma)) {
        next();
        interfaces.push_back(parseDerivedInterface());
    }
    return interfaces;
}

Declaration *Parser::parseInterfaceDecl() {
    auto startToken = expect(TokenType::Interface);
    auto name = parseIdent();
    auto interfaces = parseInterfaceList();
    auto body = parseInterfaceBody();
    return ctx->create<InterfaceDecl>(
        startToken.getLocInfo(),
        name, interfaces, body
    );
}

std::vector<MemberDecl *> Parser::parseInterfaceBody() {
    auto start = expect(TokenType::LBrace);
    std::vector<MemberDecl *> methods;
    while (!check(TokenType::RBrace)) {
        methods.push_back(parseInterfaceMemberDecl());
    }
    expect(TokenType::RBrace);
    return methods;
}

MemberDecl *Parser::parseInterfaceMemberDecl() {
    Declaration *decl;
    if (check(TokenType::Interface)) {
        decl = parseInterfaceDecl();
    } else {
        auto func = parseFunctionDecl();
        if (!func->getBody()) expect(TokenType::SemiColon);
        decl = func;
    }
    return ctx->create<MemberDecl>(
        decl->getLoc(),
        ctx->create<Identifier>(decl->getLoc(), "public"), decl
    );
}

Declaration *Parser::parseAnnotationDecl() {
    return nullptr;
}
CompilationUnit *Parser::parseCompilationUnit() {
    std::vector<Declaration *> decls;
    auto startToken = tok;
    while (!check(TokenType::EndOfFile) && !check(TokenType::WhiteSpace)) {
        if (check(TokenType::Func)) {
            decls.push_back(parseFunctionDecl());
        } else {
            bool isVarDecl = check(TokenType::Var);
            decls.push_back(parseVarOrTypeDecl());
            if (isVarDecl) expect(TokenType::SemiColon);
        }
    }
    return ctx->create<CompilationUnit>(
        startToken.getLocInfo(),
        decls
    );
}

}

