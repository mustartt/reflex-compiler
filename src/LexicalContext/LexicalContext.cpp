//
// Created by henry on 2022-04-27.
//

#include "LexicalContext.h"

#include <Type.h>
#include <ASTLiteral.h>
#include <ASTStatement.h>
#include <SourceManager.h>

namespace reflex {

LexicalScope *LexicalContext::createGlobalScope(CompilationUnit *CU) {
    return insertScope(std::make_unique<LexicalScope>(
        *this, nullptr, LexicalScopeType::Global, "__global__", CU));
}

LexicalScope *LexicalContext::createCompositeScope(AggregateDecl *decl, ScopeMember *parent) {
    return insertScope(std::make_unique<LexicalScope>(
        *this, parent, LexicalScopeType::Composite, decl->getDeclname(), decl));
}

LexicalScope *LexicalContext::createFunctionScope(FunctionDecl *decl, ScopeMember *parent) {
    return insertScope(std::make_unique<LexicalScope>(
        *this, parent, LexicalScopeType::Function, decl->getDeclname(), decl));
}

LexicalScope *LexicalContext::createMethodScope(MethodDecl *decl, ScopeMember *parent) {
    return insertScope(std::make_unique<LexicalScope>(
        *this, parent, LexicalScopeType::Method, decl->getDeclname(), decl));
}

LexicalScope *LexicalContext::createBlockScope(BlockStmt *decl, size_t blockID, ScopeMember *parent) {
    return insertScope(std::make_unique<LexicalScope>(
        *this, parent, LexicalScopeType::Block,
        "__block(" + std::to_string(blockID) + ")__", decl));
}

LexicalScope *LexicalContext::createLambdaScope(FunctionLiteral *decl, size_t lambdaID, ScopeMember *parent) {
    return insertScope(std::make_unique<LexicalScope>(
        *this, parent, LexicalScopeType::Lambda,
        "__lambda(" + std::to_string(lambdaID) + ")__", decl));
}

LexicalScope *LexicalContext::insertScope(std::unique_ptr<LexicalScope> ptr) {
    scopes.push_back(std::move(ptr));
    return scopes.back().get();
}

std::ostream &printIndent(std::ostream &os, size_t indent) {
    for (size_t i = 0; i < indent; ++i) os << "  ";
    return os;
}

void LexicalContext::dump(std::ostream &os, LexicalScope *start, size_t indent) {
    printIndent(os, indent) << "Scope(" << start->getScopename() << ") ";
    if (start->getNodeDecl()->location()) {
        os << start->getNodeDecl()->location()->getStringRepr() << " {";
    } else {
        os << " <noloc> {";
    }
    auto hasMembers = !start->getMembers().empty();
    if (hasMembers) os << std::endl;
    for (const auto &member: start->getMembers()) {
        auto type = member->getMemberType();
        printIndent(os, indent + 1)
            << member->getMembername() << ": <"
            << (type ? type->getTypeString() : "<unknown>")
            << ">" << std::endl;
        if (member->hasScope()) {
            dump(os, member->getChild(), indent + 2);
        }
    }
    printIndent(os, hasMembers ? indent : 0) << "}" << std::endl;
}

}
