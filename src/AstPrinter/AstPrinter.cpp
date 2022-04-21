//
// Created by henry on 2022-03-21.
//

#include "AstPrinter.h"
#include "AstNodes.h"
#include "Type.h"

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

void AstPrinter::visit(CompilationUnit *visitable) {
    printNodePrefix("CompilationUnit: "
                        + visitable->getLoc().getFormattedRepr() + " "
                        + printAstType(visitable->getTyp()));
    {
        int it = 0;
        for (auto i = visitable->getDecls().begin(); i != visitable->getDecls().end(); ++i, ++it) {
            auto isLastNode = it == visitable->getDecls().size() - 1;
            Scope _(*this, isLastNode);
            (*i)->accept(this);
        }
    }
    depthFlag[depth] = true;
}

void AstPrinter::visit(InterfaceDecl *visitable) {
    printNodePrefix("InterfaceDecl: "
                        + visitable->getLoc().getFormattedRepr() + " "
                        + printAstType(visitable->getTyp()));
    {
        int it = 0;
        for (auto i = visitable->getInterfaceMembers().begin(); i != visitable->getInterfaceMembers().end();
             ++i, ++it) {
            auto isLastNode = it == visitable->getInterfaceMembers().size() - 1;
            Scope _(*this, isLastNode);
            (*i)->accept(this);
        }
    }
    depthFlag[depth] = true;
}

void AstPrinter::visit(ClassDecl *visitable) {
    printNodePrefix("ClassDecl: "
                        + visitable->getLoc().getFormattedRepr() + " "
                        + printAstType(visitable->getTyp()));
    {
        int it = 0;
        for (auto i = visitable->getMembers().begin(); i != visitable->getMembers().end(); ++i, ++it) {
            auto isLastNode = it == visitable->getMembers().size() - 1;
            Scope _s(*this, isLastNode);
            (*i)->accept(this);
        }
    }
    depthFlag[depth] = true;
}

void AstPrinter::visit(FunctionDecl *visitable) {
    printNodePrefix("FunctionDecl: "
                        + visitable->getName()->getIdent() + " "
                        + visitable->getLoc().getFormattedRepr() + " "
                        + printAstType(visitable->getTyp()));
    {
        int it = 0;
        for (auto i = visitable->getParams().begin(); i != visitable->getParams().end(); ++i, ++it) {
            auto isLastNode = it == visitable->getParams().size() - 1;
            Scope _s(*this, visitable->getBody() == nullptr && isLastNode);
            (*i)->accept(this);
        }
        if (visitable->getBody()) {
            Scope _s(*this, true);
            visitable->getBody()->accept(this);
        }
    }
    depthFlag[depth] = true;
}

void AstPrinter::visit(MemberDecl *visitable) {
    printNodePrefix("MemberDecl: "
                        + visitable->getLoc().getFormattedRepr() + " "
                        + printAstType(visitable->getTyp()));
    {
        Scope _(*this, true);
        visitable->getDeclaration()->accept(this);
    }
    depthFlag[depth] = true;
}

void AstPrinter::visit(Block *visitable) {
    printNodePrefix("Block: "
                        + visitable->getLoc().getFormattedRepr() + " "
                        + printAstType(visitable->getTyp()));
    {
        int it = 0;
        for (auto i = visitable->getStmts().begin(); i != visitable->getStmts().end(); ++i, ++it) {
            auto isLastNode = it == visitable->getStmts().size() - 1;
            Scope _s(*this, isLastNode);
            (*i)->accept(this);
        }
    }
    depthFlag[depth] = true;
}

void AstPrinter::visit(ExpressionStmt *visitable) {
    printNodePrefix("ExpressionStmt: "
                        + visitable->getLoc().getFormattedRepr() + " "
                        + printAstType(visitable->getTyp()));
    {
        Scope _s(*this, true);
        visitable->getExpr()->accept(this);
    }
    depthFlag[depth] = true;
}

void AstPrinter::visit(IncDecStmt *visitable) {
    printNodePrefix("IncDecStmt: '"
                        + getPostfixOperator(visitable->getPostfixOp()) + "' "
                        + visitable->getLoc().getFormattedRepr() + " "
                        + printAstType(visitable->getTyp()));
    {
        Scope _s(*this, true);
        visitable->getExpr()->accept(this);
    }
    depthFlag[depth] = true;
}

void AstPrinter::visit(AssignmentStmt *visitable) {
    printNodePrefix("AssignmentStmt: '"
                        + getAssignOperator(visitable->getAssignOp()) + "' "
                        + visitable->getLoc().getFormattedRepr() + " "
                        + printAstType(visitable->getTyp()));
    {
        Scope _s(*this, false);
        visitable->getLhs()->accept(this);
    }
    {
        Scope _s(*this, true);
        visitable->getRhs()->accept(this);
    }
    depthFlag[depth] = true;
}

void AstPrinter::visit(EmptyStmt *visitable) {
    printNodePrefix("EmptyStmt: "
                        + visitable->getLoc().getFormattedRepr() + " "
                        + printAstType(visitable->getTyp()));
    depthFlag[depth] = true;
}

void AstPrinter::visit(WhileStmt *visitable) {
    printNodePrefix("WhileStmt: "
                        + visitable->getLoc().getFormattedRepr() + " "
                        + printAstType(visitable->getTyp()));
    {
        Scope _(*this, false);
        visitable->getCond()->accept(this);
    }
    {
        Scope _(*this, true);
        visitable->getBody()->accept(this);
    }
    depthFlag[depth] = true;
}

void AstPrinter::visit(ForStmt *visitable) {
    printNodePrefix("ForStmt: "
                        + visitable->getLoc().getFormattedRepr() + " "
                        + printAstType(visitable->getTyp()));
    {
        Scope _(*this, false);
        visitable->getClause()->accept(this);
    }
    {
        Scope _(*this, true);
        visitable->getBody()->accept(this);
    }
    depthFlag[depth] = true;
}

void AstPrinter::visit(ForNormalClause *visitable) {
    printNodePrefix("ForNormalClause: "
                        + visitable->getLoc().getFormattedRepr() + " "
                        + printAstType(visitable->getTyp()));
    {
        Scope _(*this, false);
        visitable->getInit()->accept(this);
        visitable->getCond()->accept(this);
    }
    {
        Scope _(*this, true);
        visitable->getPost()->accept(this);
    }
    depthFlag[depth] = true;
}

void AstPrinter::visit(ForRangeClause *visitable) {
    printNodePrefix("ForRangeClause: "
                        + visitable->getLoc().getFormattedRepr() + " "
                        + printAstType(visitable->getTyp()));
    {
        Scope _(*this, false);
        visitable->getVariable()->accept(this);
    }
    {
        Scope _(*this, true);
        visitable->getIterExpr()->accept(this);
    }
    depthFlag[depth] = true;
}

void AstPrinter::visit(IfStmt *visitable) {
    printNodePrefix("IfStmt: "
                        + visitable->getLoc().getFormattedRepr() + " "
                        + printAstType(visitable->getTyp()));
    {
        Scope _(*this, false);
        visitable->getCond()->accept(this);
    }
    auto hasElseBlock = visitable->getElseBlock();
    {
        Scope _(*this, hasElseBlock);
        visitable->getPrimaryBlock()->accept(this);
    }
    if (hasElseBlock) {
        Scope _(*this, true);
        visitable->getElseBlock()->accept(this);
    }
    depthFlag[depth] = true;
}

void AstPrinter::visit(ContinueStmt *visitable) {
    printNodePrefix("ContinueStmt: "
                        + visitable->getLoc().getFormattedRepr() + " "
                        + printAstType(visitable->getTyp()));
    depthFlag[depth] = true;
}

void AstPrinter::visit(BreakStmt *visitable) {
    printNodePrefix("BreakStmt: "
                        + visitable->getLoc().getFormattedRepr() + " "
                        + printAstType(visitable->getTyp()));
    depthFlag[depth] = true;
}

void AstPrinter::visit(ReturnStmt *visitable) {
    printNodePrefix("ReturnStmt: "
                        + visitable->getLoc().getFormattedRepr() + " "
                        + printAstType(visitable->getTyp()));
    {
        Scope _(*this, true);
        if (visitable->getReturnValue())
            visitable->getReturnValue()->accept(this);
    }
    depthFlag[depth] = true;
}

void AstPrinter::visit(VariableDecl *visitable) {
    printNodePrefix("VariableDecl: "
                        + visitable->getLoc().getFormattedRepr() + " "
                        + printAstType(visitable->getTyp()));
    {
        Scope _(*this, !visitable->getInitializer());
        visitable->getName()->accept(this);
    }
    {
        Scope _(*this, true);
        if (visitable->getInitializer())
            visitable->getInitializer()->accept(this);
    }
    depthFlag[depth] = true;
}

void AstPrinter::visit(ArgumentExpr *visitable) {
    printNodePrefix("ArgumentExpr: "
                        + visitable->getLoc().getFormattedRepr() + " "
                        + printAstType(visitable->getTyp()));
    auto hasArgument = !visitable->getArguments().empty();
    {
        Scope _(*this, !hasArgument);
        visitable->getBaseExpr()->accept(this);
    }
    {
        int it = 0;
        for (auto i = visitable->getArguments().begin(); i != visitable->getArguments().end(); ++i, ++it) {
            auto isLastNode = it == visitable->getArguments().size() - 1;
            Scope _s(*this, isLastNode);
            (*i)->accept(this);
        }
    }
    depthFlag[depth] = true;
}

void AstPrinter::visit(SelectorExpr *visitable) {
    printNodePrefix("SelectorExpr: "
                        + visitable->getSelector()->getIdent() + " "
                        + visitable->getLoc().getFormattedRepr() + " "
                        + printAstType(visitable->getTyp()));
    {
        Scope _(*this, true);
        visitable->getBaseExpr()->accept(this);
    }
    depthFlag[depth] = true;
}

void AstPrinter::visit(CastExpr *visitable) {
    printNodePrefix("CastExpr: "
                        + visitable->getLoc().getFormattedRepr() + " "
                        + printAstType(visitable->getTyp()));
    {
        Scope _(*this, true);
        visitable->getFrom()->accept(this);
    }
    depthFlag[depth] = true;
}

void AstPrinter::visit(NewExpr *visitable) {
    printNodePrefix("NewExpr: "
                        + visitable->getLoc().getFormattedRepr() + " "
                        + printAstType(visitable->getTyp()));
    {
        Scope _(*this, true);
        printNodePrefix("InstanceType: ", false);
        visitable->getInstanceTyp()->accept(this);
        output << visitable->getInstanceTyp()->getLoc().getFormattedRepr() << " "
               << printAstType(visitable->getTyp())
               << std::endl;
    }
    depthFlag[depth] = true;
}

void AstPrinter::visit(BinaryExpr *visitable) {
    printNodePrefix("BinaryExpr: '"
                        + getBinaryOperator(visitable->getOp()) + "' "
                        + visitable->getLoc().getFormattedRepr() + " "
                        + printAstType(visitable->getTyp()));
    {
        Scope _(*this, false);
        visitable->getLhs()->accept(this);
    }
    {
        Scope _(*this, true);
        visitable->getRhs()->accept(this);
    }
    depthFlag[depth] = true;
}

void AstPrinter::visit(UnaryExpr *visitable) {
    printNodePrefix("UnaryExpr: '"
                        + getUnaryOperator(visitable->getOp()) + "' "
                        + visitable->getLoc().getFormattedRepr() + " "
                        + printAstType(visitable->getTyp()));
    {
        Scope _(*this, true);
        visitable->getExpr()->accept(this);
    }
    depthFlag[depth] = true;
}

void AstPrinter::visit(FunctionLit *visitable) {
    printNodePrefix("FunctionLit: "
                        + visitable->getLoc().getFormattedRepr() + " "
                        + printAstType(visitable->getTyp()));
    {
        for (auto param: visitable->getParameters()) {
            Scope _(*this, false);
            param->accept(this);
        }
        Scope _(*this, true);
        visitable->getBody()->accept(this);
    }
    depthFlag[depth] = true;
}

void AstPrinter::visit(ParamDecl *visitable) {
    printNodePrefix("ParamDecl: "
                        + visitable->getLoc().getFormattedRepr() + " "
                        + printAstType(visitable->getTyp()));
    {
        Scope _(*this, true);
        visitable->getName()->accept(this);
    }
    depthFlag[depth] = true;
}

void AstPrinter::visit(ArrayLit *visitable) {
    printNodePrefix("ArrayLit: "
                        + visitable->getLoc().getFormattedRepr() + " "
                        + printAstType(visitable->getTyp()));
    {
        int it = 0;
        auto &initLst = visitable->getInitializerList();
        for (auto i = initLst.begin(); i != initLst.end(); ++i, ++it) {
            auto isLastNode = it == initLst.size() - 1;
            Scope _s(*this, isLastNode);
            (*i)->accept(this);
        }
    }
    depthFlag[depth] = true;
}

void AstPrinter::visit(FunctionTypeExpr *visitable) {
    output << "(func(";
    auto &params = visitable->getParamList();
    if (!params.empty()) {
        params[0]->accept(this);
        for (size_t i = 1; i < params.size(); ++i) {
            output << ",";
            params[i]->accept(this);
        }
    }
    output << ")->";
    visitable->getReturnTyp()->accept(this);
    output << ")";
}

void AstPrinter::visit(ArrayTypeExpr *visitable) {
    output << "(";
    visitable->getElementTyp()->accept(this);
    output << ")[]";
}

void AstPrinter::visit(IdentifierTypeExpr *visitable) {
    output << visitable->getTypename()->getIdent();
}

void AstPrinter::visit(NullLit *visitable) {
    printNodePrefix("NullLit: "
                        + visitable->getValue() + " "
                        + visitable->getLoc().getFormattedRepr() + " "
                        + printAstType(visitable->getTyp()));
    depthFlag[depth] = true;
}

void AstPrinter::visit(BoolLit *visitable) {
    printNodePrefix("BoolLit: "
                        + visitable->getValue() + " "
                        + visitable->getLoc().getFormattedRepr() + " "
                        + printAstType(visitable->getTyp()));
    depthFlag[depth] = true;
}

void AstPrinter::visit(StringLit *visitable) {
    printNodePrefix("StringLit: '" + visitable->getValue() + "' "
                        + visitable->getLoc().getFormattedRepr() + " "
                        + printAstType(visitable->getTyp()));
    depthFlag[depth] = true;
}

void AstPrinter::visit(NumberLit *visitable) {
    printNodePrefix("NumberLit: " + visitable->getValue() + " "
                        + visitable->getLoc().getFormattedRepr() + " "
                        + printAstType(visitable->getTyp()));
    depthFlag[depth] = true;
}

void AstPrinter::visit(ModuleSelector *visitable) {
    printNodePrefix("ModuleSelector: "
                        + visitable->getSelector() + " "
                        + visitable->getLoc().getFormattedRepr() + " "
                        + printAstType(visitable->getTyp()));
    {
        Scope _(*this, true);
        visitable->getBasename()->accept(this);
    }
    depthFlag[depth] = true;
}

void AstPrinter::visit(Identifier *visitable) {
    printNodePrefix("Identifier: "
                        + visitable->getIdent() + " "
                        + visitable->getLoc().getFormattedRepr() + " "
                        + printAstType(visitable->getTyp()));
    depthFlag[depth] = true;
}

void AstPrinter::visit(IndexExpr *visitable) {
    printNodePrefix("IndexExpr: "
                        + visitable->getLoc().getFormattedRepr() + " "
                        + printAstType(visitable->getTyp()));
    {
        Scope _(*this, false);
        visitable->getBaseExpr()->accept(this);
    }
    {
        Scope _(*this, true);
        if (visitable->getIndex())
            visitable->getIndex()->accept(this);
    }
    depthFlag[depth] = true;
}

}
