//
// Created by henry on 2022-04-26.
//

#include "Parser.h"

#include "ASTContext.h"

namespace reflex {

DeclStmt *Parser::parseDeclStmt() {
    Declaration *decl;
    if (check(TokenType::Class)) {
        decl = parseClassDecl(Visibility::Private);
    } else if (check(TokenType::Interface)) {
        decl = parseInterfaceDecl(Visibility::Private);
    } else {
        decl = parseVariableDecl();
    }
    return context.create<DeclStmt>(decl->location(), decl);
}

std::pair<std::vector<ParamDecl *>, ASTTypeExpr *> Parser::parseSignature() {
    expect(TokenType::LParen);
    auto params = parseParamList();
    auto end = expect(TokenType::RParen);
    ASTTypeExpr *returnType = context.create<BaseTypenameExpr>(end.getLocInfo(), "void");
    if (check(TokenType::ReturnArrow)) {
        next();
        returnType = parseType();
    }
    return {params, returnType};
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
    auto ident = parseBaseTypenameType();
    expect(TokenType::Colon);
    return context.create<ParamDecl>(
        ident->location(),
        ident->getTypeName(),
        parseType()
    );
}

FunctionDecl *Parser::parseFunctionDecl() {
    auto startToken = expect(TokenType::Func);
    auto name = parseBaseTypenameType();
    auto[params, ret] = parseSignature();
    BlockStmt *body = nullptr;
    if (!check(TokenType::SemiColon)) {
        body = parseBlockStmt();
    }
    return context.create<FunctionDecl>(
        name->location(),
        name->getTypeName(), params, ret, body
    );
}

MethodDecl *Parser::parseMethodDecl(Visibility visibility, AggregateDecl *parent) {
    auto startToken = expect(TokenType::Func);
    auto name = parseBaseTypenameType();
    auto[params, ret] = parseSignature();
    BlockStmt *body = nullptr;
    if (!check(TokenType::SemiColon)) {
        body = parseBlockStmt();
    }
    return context.create<MethodDecl>(
        name->location(),
        name->getTypeName(), params, ret, body, parent, visibility
    );
}

VariableDecl *Parser::parseVariableDecl() {
    expect(TokenType::Var);
    auto name = parseBaseDeclRef();
    ASTTypeExpr *varType = nullptr;
    Expression *initializer = nullptr;
    if (check(TokenType::Colon)) {
        next();
        varType = parseType();
    }
    if (check(TokenType::Assign)) {
        next();
        initializer = parseExpr();
    }
    return context.create<VariableDecl>(
        name->location(),
        name->getBaseRefName(), varType, initializer
    );
}

FieldDecl *Parser::parseFieldDecl(Visibility visibility, ClassDecl *parent) {
    expect(TokenType::Var);
    auto name = parseBaseDeclRef();
    ASTTypeExpr *varType = nullptr;
    Expression *initializer = nullptr;
    if (check(TokenType::Colon)) {
        next();
        varType = parseType();
    }
    if (check(TokenType::Assign)) {
        next();
        initializer = parseExpr();
    }
    return context.create<FieldDecl>(
        name->location(),
        name->getBaseRefName(), varType, parent, visibility, initializer
    );
}

ReferenceTypenameExpr *Parser::parseBaseClass() {
    if (!check(TokenType::LParen)) return nullptr;
    next();
    ReferenceTypenameExpr *baseclass = parseBaseTypenameType();
    if (check(TokenType::NameSeparator)) {
        baseclass = parseQualifiedType(baseclass);
    }
    expect(TokenType::RParen);
    return baseclass;
}

ReferenceTypenameExpr *Parser::parseDerivedInterface() {
    ReferenceTypenameExpr *baseclass = parseBaseTypenameType();
    if (check(TokenType::NameSeparator)) {
        baseclass = parseQualifiedType(baseclass);
    }
    return baseclass;
}

std::vector<ReferenceTypenameExpr *> Parser::parseInterfaceList() {
    if (!check(TokenType::Colon)) return {};
    next();
    std::vector<ReferenceTypenameExpr *> interfaces;
    interfaces.push_back(parseDerivedInterface());
    while (check(TokenType::Comma)) {
        next();
        interfaces.push_back(parseDerivedInterface());
    }
    return interfaces;
}

ClassDecl *Parser::parseClassDecl(Visibility visibility) {
    auto startToken = expect(TokenType::Class);
    auto classname = parseBaseTypenameType();
    auto baseclass = parseBaseClass();
    auto interfaces = parseInterfaceList();
    auto klassDecl = context.create<ClassDecl>(
        startToken.getLocInfo(), visibility,
        classname->getTypeName(), baseclass, interfaces
    );
    parseClassBody(klassDecl);
    return klassDecl;
}

InterfaceDecl *Parser::parseInterfaceDecl(Visibility visibility) {
    auto startToken = expect(TokenType::Interface);
    auto name = parseBaseTypenameType();
    auto interfaces = parseInterfaceList();
    auto interfaceDecl = context.create<InterfaceDecl>(
        startToken.getLocInfo(), visibility,
        name->getTypeName(), interfaces
    );
    parseInterfaceBody(interfaceDecl);
    return interfaceDecl;
}

void Parser::parseClassBody(ClassDecl *klass) {
    expect(TokenType::LBrace);
    while (!check(TokenType::RBrace)) {
        auto visToken = expect(TokenType::Identifier);
        auto visibility = getVisibilityFromString(visToken.getLexeme());
        if (check(TokenType::Class)) {
            klass->addMemberDecl(parseClassDecl(visibility));
        } else if (check(TokenType::Interface)) {
            klass->addMemberDecl(parseInterfaceDecl(visibility));
        } else if (check(TokenType::Func)) {
            klass->addMemberDecl(parseMethodDecl(visibility, klass));
        } else {
            klass->addMemberDecl(parseFieldDecl(visibility, klass));
            expect(TokenType::SemiColon);
        }
    }
    expect(TokenType::RBrace);
}

void Parser::parseInterfaceBody(InterfaceDecl *interface) {
    expect(TokenType::LBrace);
    while (!check(TokenType::RBrace)) {
        auto visToken = expect(TokenType::Identifier);
        auto visibility = getVisibilityFromString(visToken.getLexeme());
        if (check(TokenType::Interface)) {
            interface->addMemberDecl(parseInterfaceDecl(visibility));
        } else if (check(TokenType::Func)) {
            interface->addMemberDecl(parseMethodDecl(visibility, interface));
            expect(TokenType::SemiColon);
        }
    }
    expect(TokenType::RBrace);
}

CompilationUnit *Parser::parseCompilationUnit() {
    std::vector<Declaration *> decls;
    auto startToken = lookahead;
    while (!check(TokenType::EndOfFile) && !check(TokenType::WhiteSpace)) {
        if (check(TokenType::Func)) {
            decls.push_back(parseFunctionDecl());
        } else if (check(TokenType::Var)) {
            decls.push_back(parseVariableDecl());
            expect(TokenType::SemiColon);
        } else if (check(TokenType::Class)) {
            decls.push_back(parseClassDecl(Visibility::Public));
        } else {
            decls.push_back(parseInterfaceDecl(Visibility::Public));
        }
    }
    return context.create<CompilationUnit>(
        startToken.getLocInfo(),
        "CompilationUnit", decls
    );
}

}
