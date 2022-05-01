//
// Created by henry on 2022-03-21.
//

#include "AstPrinter.h"

#include <vector>

#include "Type.h"
#include "SourceManager.h"
#include "ASTDeclaration.h"
#include "ASTStatement.h"
#include "ASTExpression.h"
#include "ASTLiteral.h"

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
    if (!type) {
        return "<<unknown>>";
    }
    return "<" + type->getTypeString() + ">";
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
                        + CU.location()->getStringRepr() + " '"
                        + CU.getDeclname() + "'");
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
                        + klass.location()->getStringRepr() + " '"
                        + klass.getDeclname() + "' "
                        + printAstType(klass.getType()));
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
                        + inf.location()->getStringRepr() + " '"
                        + inf.getDeclname() + "' "
                        + printAstType(inf.getType()));
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

OpaqueType AstPrinter::visit(VariableDecl &decl) {
    printNodePrefix("VariableDecl: "
                        + decl.location()->getStringRepr() + " '"
                        + decl.getDeclname() + "' "
                        + printAstType(decl.getType()));

    if (decl.getInitializer()) {
        Scope _(*this, true);
        decl.getInitializer()->accept(this);
    }

    depthFlag[depth] = true;
    return {};
}

OpaqueType AstPrinter::visit(FieldDecl &decl) {
    printNodePrefix("FieldDecl: "
                        + decl.location()->getStringRepr() + " "
                        + getVisibilityString(decl.getVisibility()) + " member '"
                        + decl.getDeclname() + "' "
                        + printAstType(decl.getType()));

    if (decl.getInitializer()) {
        Scope _(*this, true);
        decl.getInitializer()->accept(this);
    }

    depthFlag[depth] = true;
    return {};
}

OpaqueType AstPrinter::visit(ParamDecl &decl) {
    printNodePrefix("ParamDecl: "
                        + decl.location()->getStringRepr() + " '"
                        + decl.getDeclname() + "' "
                        + printAstType(decl.getType()));

    if (decl.getInitializer()) {
        Scope _(*this, true);
        decl.getInitializer()->accept(this);
    }

    depthFlag[depth] = true;
    return {};
}

OpaqueType AstPrinter::visit(FunctionDecl &decl) {
    printNodePrefix("FunctionDecl: "
                        + decl.location()->getStringRepr() + " '"
                        + decl.getDeclname() + "' "
                        + printAstType(decl.getType()));
    int it = 0;
    for (auto i = decl.getParamDecls().begin(); i != decl.getParamDecls().end(); ++i, ++it) {
        auto isLastNode = it == decl.getParamDecls().size() - 1;
        Scope _s(*this, decl.getBody() == nullptr && isLastNode);
        (*i)->accept(this);
    }
    if (decl.getBody()) {
        Scope _s(*this, true);
        decl.getBody()->accept(this);
    }

    depthFlag[depth] = true;
    return {};
}

OpaqueType AstPrinter::visit(MethodDecl &decl) {
    printNodePrefix("MethodDecl: "
                        + decl.location()->getStringRepr() + " "
                        + getVisibilityString(decl.getVisibility()) + " member '"
                        + decl.getDeclname() + "' "
                        + printAstType(decl.getType()));
    int it = 0;
    for (auto i = decl.getParamDecls().begin(); i != decl.getParamDecls().end(); ++i, ++it) {
        auto isLastNode = it == decl.getParamDecls().size() - 1;
        Scope _s(*this, decl.getBody() == nullptr && isLastNode);
        (*i)->accept(this);
    }
    if (decl.getBody()) {
        Scope _s(*this, true);
        decl.getBody()->accept(this);
    }

    depthFlag[depth] = true;
    return {};
}

OpaqueType AstPrinter::visit(BlockStmt &stmt) {
    printNodePrefix("BlockStmt: " + stmt.location()->getStringRepr());

    int it = 0;
    for (auto i = stmt.getStmts().begin(); i != stmt.getStmts().end(); ++i, ++it) {
        auto isLastNode = it == stmt.getStmts().size() - 1;
        Scope _s(*this, isLastNode);
        (*i)->accept(this);
    }

    depthFlag[depth] = true;
    return {};
}

OpaqueType AstPrinter::visit(ReturnStmt &stmt) {
    printNodePrefix("ReturnStmt: "
                        + stmt.location()->getStringRepr() + " "
                        + printAstType(stmt.getReturnType()));
    if (stmt.getReturnValue()) {
        Scope _(*this, true);
        stmt.getReturnValue()->accept(this);
    }
    depthFlag[depth] = true;
    return {};
}

OpaqueType AstPrinter::visit(BreakStmt &stmt) {
    printNodePrefix("BreakStmt: " + stmt.location()->getStringRepr());
    depthFlag[depth] = true;
    return {};
}

OpaqueType AstPrinter::visit(ContinueStmt &stmt) {
    printNodePrefix("ContinueStmt: " + stmt.location()->getStringRepr());
    depthFlag[depth] = true;
    return {};
}

OpaqueType AstPrinter::visit(IfStmt &stmt) {
    printNodePrefix("IfStmt: " + stmt.location()->getStringRepr());
    {
        Scope _(*this, false);
        stmt.getCond()->accept(this);
    }
    auto hasElseBlock = stmt.getElseBlock();
    {
        Scope _(*this, hasElseBlock);
        stmt.getPrimaryBlock()->accept(this);
    }
    if (hasElseBlock) {
        Scope _(*this, true);
        stmt.getElseBlock()->accept(this);
    }
    depthFlag[depth] = true;
    return {};
}

OpaqueType AstPrinter::visit(ForStmt &stmt) {
    printNodePrefix("ForStmt: " + stmt.location()->getStringRepr());
    // for clause visiting
    {
        Scope _(*this, true);
        stmt.getBody()->accept(this);
    }
    depthFlag[depth] = true;
    return {};
}

OpaqueType AstPrinter::visit(WhileStmt &stmt) {
    printNodePrefix("WhileStmt: " + stmt.location()->getStringRepr());
    {
        Scope _(*this, false);
        stmt.getCond()->accept(this);
    }
    {
        Scope _(*this, true);
        stmt.getBody()->accept(this);
    }
    depthFlag[depth] = true;
    return {};
}

OpaqueType AstPrinter::visit(EmptyStmt &stmt) {
    printNodePrefix("EmptyStmt: " + stmt.location()->getStringRepr());
    depthFlag[depth] = true;
    return {};
}

OpaqueType AstPrinter::visit(AssignmentStmt &stmt) {
    printNodePrefix("AssignmentStmt: '"
                        + getAssignOperator(stmt.getAssignOp()) + "' "
                        + stmt.location()->getStringRepr());
    {
        Scope _s(*this, false);
        stmt.getLhs()->accept(this);
    }
    {
        Scope _s(*this, true);
        stmt.getRhs()->accept(this);
    }
    depthFlag[depth] = true;
    return {};
}

OpaqueType AstPrinter::visit(IncDecStmt &stmt) {
    printNodePrefix("IncDecStmt: '"
                        + getPostfixOperator(stmt.getPostfixOp()) + "' "
                        + stmt.location()->getStringRepr());
    {
        Scope _s(*this, true);
        stmt.getExpr()->accept(this);
    }
    depthFlag[depth] = true;
    return {};
}

OpaqueType AstPrinter::visit(ExpressionStmt &stmt) {
    printNodePrefix("ExpressionStmt: " + stmt.location()->getStringRepr());
    {
        Scope _s(*this, true);
        stmt.getExpr()->accept(this);
    }
    depthFlag[depth] = true;
    return {};
}

OpaqueType AstPrinter::visit(DeclStmt &stmt) {
    printNodePrefix("DeclStmt: " + stmt.location()->getStringRepr());
    {
        Scope _s(*this, true);
        stmt.getDecl()->accept(this);
    }
    depthFlag[depth] = true;
    return {};
}

OpaqueType AstPrinter::visit(DeclRefExpr &expr) {
    printNodePrefix("DeclRefExpr: "
                        + expr.getReferenceName() + " "
                        + expr.location()->getStringRepr() + " "
                        + printAstType(expr.getType()));
    depthFlag[depth] = true;
    return {};
}

OpaqueType AstPrinter::visit(UnaryExpr &expr) {
    printNodePrefix("UnaryExpr: '"
                        + Operator::getUnaryOperator(expr.getUnaryOp()) + "' "
                        + expr.location()->getStringRepr() + " "
                        + printAstType(expr.getType()));
    {
        Scope _s(*this, true);
        expr.getExpr()->accept(this);
    }
    depthFlag[depth] = true;
    return {};
}

OpaqueType AstPrinter::visit(BinaryExpr &expr) {
    printNodePrefix("BinaryExpr: '"
                        + Operator::getBinaryOperator(expr.getBinaryOp()) + "' "
                        + expr.location()->getStringRepr() + " "
                        + printAstType(expr.getType()));
    {
        Scope _(*this, false);
        expr.getLhs()->accept(this);
    }
    {
        Scope _(*this, true);
        expr.getRhs()->accept(this);
    }
    depthFlag[depth] = true;
    return {};
}

OpaqueType AstPrinter::visit(NewExpr &expr) {
    printNodePrefix("NewExpr: "
                        + expr.location()->getStringRepr() + " "
                        + printAstType(expr.getType()));
    depthFlag[depth] = true;
    return {};
}

OpaqueType AstPrinter::visit(ImplicitCastExpr &expr) {
    printNodePrefix("ImplicitCastExpr: "
                        + expr.location()->getStringRepr() + " "
                        + printAstType(expr.getType()) + " <"
                        + Operator::getImplicitConversion(expr.getConversion())
                        + ">");
    {
        Scope _(*this, true);
        expr.getFrom()->accept(this);
    }
    depthFlag[depth] = true;
    return {};
}

OpaqueType AstPrinter::visit(CastExpr &expr) {
    printNodePrefix("CastExpr: "
                        + expr.location()->getStringRepr() + " "
                        + printAstType(expr.getType()));
    {
        Scope _(*this, true);
        expr.getFrom()->accept(this);
    }
    depthFlag[depth] = true;
    return {};
}

OpaqueType AstPrinter::visit(IndexExpr &expr) {
    printNodePrefix("IndexExpr: "
                        + expr.location()->getStringRepr() + " "
                        + printAstType(expr.getType()));
    {
        Scope _(*this, false);
        expr.getBaseExpr()->accept(this);
    }
    {
        Scope _(*this, true);
        if (expr.getIndex())
            expr.getIndex()->accept(this);
    }
    depthFlag[depth] = true;
    return {};
}

OpaqueType AstPrinter::visit(SelectorExpr &expr) {
    printNodePrefix("SelectorExpr: "
                        + expr.getSelector() + " "
                        + expr.location()->getStringRepr() + " "
                        + printAstType(expr.getType()));
    {
        Scope _(*this, true);
        expr.getBaseExpr()->accept(this);
    }
    depthFlag[depth] = true;
    return {};
}

OpaqueType AstPrinter::visit(ArgumentExpr &expr) {
    printNodePrefix("ArgumentExpr: "
                        + expr.location()->getStringRepr() + " "
                        + printAstType(expr.getType()));
    auto hasArgument = !expr.getArguments().empty();
    {
        Scope _(*this, !hasArgument);
        expr.getBaseExpr()->accept(this);
    }
    {
        int it = 0;
        for (auto i = expr.getArguments().begin(); i != expr.getArguments().end(); ++i, ++it) {
            auto isLastNode = it == expr.getArguments().size() - 1;
            Scope _s(*this, isLastNode);
            (*i)->accept(this);
        }
    }
    depthFlag[depth] = true;
    return {};
}

OpaqueType AstPrinter::visit(NumberLiteral &literal) {
    printNodePrefix("NumberLiteral: '" + literal.getLiteral() + "' "
                        + literal.location()->getStringRepr() + " "
                        + printAstType(literal.getType()));
    depthFlag[depth] = true;
    return {};
}

OpaqueType AstPrinter::visit(StringLiteral &literal) {
    printNodePrefix("StringLiteral: \"" + literal.getLiteral() + "\" "
                        + literal.location()->getStringRepr() + " "
                        + printAstType(literal.getType()));
    depthFlag[depth] = true;
    return {};
}

OpaqueType AstPrinter::visit(BooleanLiteral &literal) {
    printNodePrefix("BooleanLiteral: \"" + literal.getLiteral() + "\" "
                        + literal.location()->getStringRepr() + " "
                        + printAstType(literal.getType()));
    depthFlag[depth] = true;
    return {};
}

OpaqueType AstPrinter::visit(NullLiteral &literal) {
    printNodePrefix("NullLiteral: \"" + literal.getLiteral() + "\" "
                        + literal.location()->getStringRepr() + " "
                        + printAstType(literal.getType()));
    depthFlag[depth] = true;
    return {};
}

OpaqueType AstPrinter::visit(ArrayLiteral &literal) {
    printNodePrefix("ArrayLiteral: "
                        + literal.location()->getStringRepr() + " "
                        + printAstType(literal.getType()));
    int it = 0;
    const auto &initLst = literal.getInitList();
    for (auto i = initLst.begin(); i != initLst.end(); ++i, ++it) {
        auto isLastNode = it == initLst.size() - 1;
        Scope _s(*this, isLastNode);
        (*i)->accept(this);
    }

    depthFlag[depth] = true;
    return {};
}

OpaqueType AstPrinter::visit(FunctionLiteral &literal) {
    printNodePrefix("FunctionLiteral: " + printAstType(literal.getType()));

    int it = 0;
    for (auto i = literal.getParamDecls().begin(); i != literal.getParamDecls().end(); ++i, ++it) {
        auto isLastNode = it == literal.getParamDecls().size() - 1;
        Scope _s(*this, literal.getBody() == nullptr && isLastNode);
        (*i)->accept(this);
    }
    if (literal.getBody()) {
        Scope _s(*this, true);
        literal.getBody()->accept(this);
    }

    depthFlag[depth] = true;
    return {};
}

}
