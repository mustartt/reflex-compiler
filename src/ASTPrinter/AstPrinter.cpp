//
// Created by henry on 2022-03-21.
//

#include "AstPrinter.h"

#include <vector>

#include "SourceManager.h"
#include "ASTDeclaration.h"
#include "ASTStatement.h"
#include "ASTExpression.h"

namespace reflex {

Scope::Scope(AstPrinter &printer, bool isLast) : printer(printer) {
    printer.isLast.push(isLast);
    ++printer.depth;
}

Scope::~Scope() {
    printer.isLast.pop();
    --printer.depth;
}

AstPrinter::AstPrinter(std::ostream &output)
    : output(output), depth(0), isLast() {
    std::vector<bool> tmp(100, true);
    depthFlag = std::move(tmp);
    isLast.push(false);
}

void AstPrinter::printTreePrefix() {
    for (size_t i = 1; i < depth; ++i) {
        if (depthFlag[i]) {
            output << "| ";
        } else {
            output << "  ";
        }
    }
}

std::string AstPrinter::printAstType(Type *type) {
//    if (!type) {
//        return "<<unknown>>";
//    }
//    return "<" + type->getTypeString() + ">";
    return "<<unknown>>";
}

void AstPrinter::printNodePrefix(const std::string &message, bool end) {
    printTreePrefix();
    if (depth == 0) {
        output << message;
    } else if (isLast.top()) {
        output << "`-" << message;
        depthFlag[depth] = false;
    } else {
        output << "|-" << message;
    }
    if (end) output << std::endl;
}

OpaqueType AstPrinter::visit(CompilationUnit &CU) {
    printNodePrefix("CompilationUnit: "
                        + CU.location()->getStringRepr() + " "
                        + printAstType(nullptr));
    {
        int it = 0;
        for (auto i = CU.getDecls().begin(); i != CU.getDecls().end(); ++i, ++it) {
            auto isLastNode = it == CU.getDecls().size() - 1;
            Scope _(*this, isLastNode);
            (*i)->accept(this);
        }
    }
    depthFlag[depth] = true;
    return {};
}

OpaqueType AstPrinter::visit(ClassDecl &klass) {
    printNodePrefix("ClassDecl: "
                        + klass.location()->getStringRepr() + " "
                        + printAstType(nullptr));
    {
        std::vector<Declaration *> members;
        for (auto decl: klass.getDecls()) members.push_back(decl);
        for (auto decl: klass.getFields()) members.push_back(decl);
        for (auto decl: klass.getMethods()) members.push_back(decl);

        int it = 0;
        for (auto i = members.begin(); i != members.end(); ++i, ++it) {
            auto isLastNode = it == members.size() - 1;
            Scope _s(*this, isLastNode);
            (*i)->accept(this);
        }
    }
    depthFlag[depth] = true;
    return {};
}

OpaqueType AstPrinter::visit(InterfaceDecl &inf) {
    printNodePrefix("InterfaceDecl: "
                        + inf.location()->getStringRepr() + " "
                        + printAstType(nullptr));
    {
        std::vector<Declaration *> members;
        for (auto decl: inf.getDecls()) members.push_back(decl);
        for (auto decl: inf.getMethods()) members.push_back(decl);

        int it = 0;
        for (auto i = members.begin(); i != members.end();
             ++i, ++it) {
            auto isLastNode = it == members.size() - 1;
            Scope _(*this, isLastNode);
            (*i)->accept(this);
        }
    }
    depthFlag[depth] = true;
    return {};
}

}
