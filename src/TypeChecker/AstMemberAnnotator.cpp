//
// Created by henry on 2022-03-26.
//

#include "AstMemberAnnotator.h"
#include "AstNodes.h"
#include "SemanticError.h"
#include "Utils/TypeCheckerUtils.h"

namespace reflex {

AstMemberAnnotator::AstMemberAnnotator(TypeContextManager *typeContext, AstContextManager *astContext)
    : typeContext(typeContext), astContext(astContext), parser(typeContext) {}

void AstMemberAnnotator::annotate() {
    annotateClassAndInterfaceDecls();
    annotateFunctionDecls();
    annotateFunctionLits();
    annotateBasicLiterals();
    annotateCastAndNewExpr();
    annotateVariableDecls();
}

void AstMemberAnnotator::annotateClassAndInterfaceDecls() {
    for (auto &interface: astContext->getAstNodesByType<InterfaceDecl>()) {
        interface->accept(this);
    }
    for (auto &cls: astContext->getAstNodesByType<ClassDecl>()) {
        cls->accept(this);
    }
}

void AstMemberAnnotator::annotateFunctionDecls() {
    for (auto &decl: astContext->getAstNodesByType<FunctionDecl>()) {
        auto funcDecl = dynamic_cast<FunctionDecl *>(decl.get());
        if (!funcDecl->getTyp()) {
            funcDecl->accept(this);
        }
    }
}

void AstMemberAnnotator::annotateFunctionLits() {
    for (auto &decl: astContext->getAstNodesByType<FunctionLit>()) {
        auto funcDecl = dynamic_cast<FunctionLit *>(decl.get());
        if (!funcDecl->getTyp()) {
            funcDecl->accept(this);
        }
    }
}

void AstMemberAnnotator::annotateVariableDecls() {
    // todo: type deduction
    for (auto &decl: astContext->getAstNodesByType<VariableDecl>()) {
        auto varDecl = dynamic_cast<VariableDecl *>(decl.get());
        if (!varDecl->getTyp()) {
            varDecl->accept(this);
        }
    }
}

void AstMemberAnnotator::annotateBasicLiterals() {
    for (auto &decl: astContext->getAstNodesByType<NumberLit>()) {
        auto literal = dynamic_cast<NumberLit *>(decl.get());
        if (!literal->getTyp()) {
            literal->accept(this);
        }
    }
    for (auto &decl: astContext->getAstNodesByType<BoolLit>()) {
        auto literal = dynamic_cast<BoolLit *>(decl.get());
        if (!literal->getTyp()) {
            literal->accept(this);
        }
    }
    for (auto &decl: astContext->getAstNodesByType<NullLit>()) {
        auto literal = dynamic_cast<NullLit *>(decl.get());
        if (!literal->getTyp()) {
            literal->accept(this);
        }
    }
}

void AstMemberAnnotator::annotateCastAndNewExpr() {
    for (auto &decl: astContext->getAstNodesByType<CastExpr>()) {
        auto cast = dynamic_cast<CastExpr *>(decl.get());
        if (!cast->getTyp()) {
            cast->accept(this);
        }
    }
    for (auto &decl: astContext->getAstNodesByType<NewExpr>()) {
        auto newExpr = dynamic_cast<NewExpr *>(decl.get());
        if (!newExpr->getTyp()) {
            newExpr->accept(this);
        }
    }
}

void AstMemberAnnotator::visit(InterfaceDecl *visitable) {
    auto type = visitable->getTyp();
    if (type) {
        for (auto member: visitable->getInterfaceMembers()) {
            member->accept(this);
            auto interfaceTy = dynamic_cast<InterfaceType *>(type);
            auto memberIdent = member->getDeclaration()->getName();
            interfaceTy->addInterfaceMethod(memberIdent->getIdent(),
                                            dynamic_cast<MemberType *>(member->getTyp()));
        }
    }
}

void AstMemberAnnotator::visit(ClassDecl *visitable) {
    auto type = visitable->getTyp();
    if (type) {
        for (auto member: visitable->getMembers()) {
            member->accept(this);
            auto classTy = dynamic_cast<ClassType *>(type);
            auto memberIdent = member->getDeclaration()->getName();
            classTy->addMember(memberIdent->getIdent(),
                               dynamic_cast<MemberType *>(member->getTyp()));
        }
    }
}

void AstMemberAnnotator::visit(MemberDecl *visitable) {
    auto modifier = getVisibilityFromString(visitable->getModifier()->getIdent());
    auto declaration = visitable->getDeclaration();
    if (auto ptr = dynamic_cast<VariableDecl *>(declaration)) {
        ptr->accept(this);
        auto type = typeContext->createMemberTy(modifier, ptr->getTyp());
        if (!type) throw TypeParseError{"Unable to parse MemberDecl VariableType"};
        visitable->setTyp(type);
    }
    if (auto ptr = dynamic_cast<FunctionDecl *>(declaration)) {
        ptr->accept(this);
        auto type = typeContext->createMemberTy(modifier, ptr->getTyp());
        if (!type) throw TypeParseError{"Unable to parse MemberDecl FunctionTypeExpr"};
        visitable->setTyp(type);
    }
    // falls through for class and interface decl
}

void AstMemberAnnotator::visit(FunctionDecl *visitable) {
    std::vector<Type *> paramTypes;
    for (auto p: visitable->getParams()) {
        p->accept(this);
        auto ty = p->getTyp();
        if (!ty) throw TypeParseError{"Unable to parse FunctionDecl Parameter Type"};
        paramTypes.push_back(ty);
    }
    auto retType = parser.parseTypeExpr(visitable->getRetTyp());
    if (!retType)
        throw TypeParseError{"Unable to parse FunctionDecl type "
                                 + visitable->getName()->getIdent()};
    visitable->setTyp(typeContext->getFunctionTy(retType, paramTypes));
}

void AstMemberAnnotator::visit(FunctionLit *visitable) {
    std::vector<Type *> paramTypes;
    for (auto p: visitable->getParameters()) {
        p->accept(this);
        auto ty = p->getTyp();
        if (!ty) throw TypeParseError{"Unable to parse FunctionDecl Parameter Type"};
        paramTypes.push_back(ty);
    }
    auto retType = parser.parseTypeExpr(visitable->getReturnTyp());
    if (!retType)
        throw TypeParseError{"Unable to parse FunctionLit Return Type"};
    visitable->setTyp(typeContext->getFunctionTy(retType, paramTypes));
}

void AstMemberAnnotator::visit(VariableDecl *visitable) {
    auto type = parser.parseTypeExpr(visitable->getVariableType());
    if (!type)
        throw TypeParseError{"Unable to parse VariableDecl type "
                                 + visitable->getName()->getIdent()};
    visitable->setTyp(type);
    visitable->getName()->setTyp(type);
}

void AstMemberAnnotator::visit(ParamDecl *visitable) {
    auto type = parser.parseTypeExpr(visitable->getParamType());
    if (!type)
        throw TypeParseError{"Unable to parse ParamDecl type "
                                 + visitable->getName()->getIdent()};
    visitable->getName()->setTyp(type);
    visitable->setTyp(type);
}

void AstMemberAnnotator::visit(NewExpr *visitable) {
    auto type = parser.parseTypeExpr(visitable->getInstanceTyp());
    if (!type) throw TypeParseError{"Unable to parse NewExpr instance type"};
    visitable->setTyp(type);
}

void AstMemberAnnotator::visit(CastExpr *visitable) {
    auto type = parser.parseTypeExpr(visitable->getTargetTyp());
    if (!type) throw TypeParseError{"Unable to parse CastExpr target type"};
    visitable->setTyp(type);
}

bool isFloatNumber(const std::string &str) {
    return std::find(str.begin(), str.end(), '.') != str.end();
}

void AstMemberAnnotator::visit(NumberLit *visitable) {
    auto value = visitable->getValue();
    if (isFloatNumber(value)) {
        visitable->setTyp(typeContext->getPrimitiveTy(PrimType::Number));
    } else {
        visitable->setTyp(typeContext->getPrimitiveTy(PrimType::Integer));
    }
}

void AstMemberAnnotator::visit(BoolLit *visitable) {
    visitable->setTyp(typeContext->getPrimitiveTy(PrimType::Boolean));
}
void AstMemberAnnotator::visit(NullLit *visitable) {
    visitable->setTyp(typeContext->getPrimitiveTy(PrimType::Null));
}

}
