//
// Created by henry on 2022-03-24.
//

#include <iostream>
#include <unordered_map>
#include <AstContextManager.h>
#include <TypeParser.h>

#include "AstTypeAnnotator.h"
#include "AstNodes.h"
#include "TopologicalSort/TopSort.h"

namespace reflex {

TypeParseError::TypeParseError(const std::string &arg)
    : runtime_error("TypeParserError: " + arg) {}

ClassTypeError::ClassTypeError(const std::string &arg)
    : runtime_error("ClassTypeError: " + arg) {}

InterfaceTypeError::InterfaceTypeError(const std::string &arg)
    : runtime_error("InterfaceTypeError: " + arg) {}

AstTypeAnnotator::AstTypeAnnotator(reflex::AstContextManager *astContext,
                                   TypeContextManager *typeContext)
    : astContext(astContext), typeContext(typeContext), parser(typeContext) {}

std::vector<ClassDecl *> AstTypeAnnotator::sortClassInheritance() {
    std::vector<ClassDecl *> classDeclNodes;
    std::unordered_map<std::string, size_t> classLst;
    for (auto &classDecl: astContext->getAstNodesByType<ClassDecl>()) {
        auto decl = dynamic_cast<ClassDecl *>(classDecl.get());
        auto classname = decl->getClassname();
        if (classLst.count(classname))
            throw ClassTypeError{"Duplicate classname " + classname};
        classLst[classname] = classDeclNodes.size();
        classDeclNodes.push_back(decl);
    }
    TopologicalSort<ClassDecl *> sorter(classDeclNodes);
    for (size_t i = 0; i < classDeclNodes.size(); ++i) {
        auto basename = classDeclNodes[i]->getBaseClassname();
        if (basename) {
            if (!classLst.count(basename.value()))
                throw ClassTypeError{"Unknown base class " + basename.value()};
            sorter.addDirectedEdge(classLst[basename.value()], i);
        }
    }
    return sorter.sort();
}

std::vector<InterfaceDecl *> AstTypeAnnotator::sortInterfaceInheritance() {
    std::vector<InterfaceDecl *> interfaceDeclNodes;
    std::unordered_map<std::string, size_t> interfaceLst;
    for (auto &interfaceDecl: astContext->getAstNodesByType<InterfaceDecl>()) {
        auto decl = dynamic_cast<InterfaceDecl *>(interfaceDecl.get());
        auto interfaceName = decl->getInterfaceName();
        if (interfaceLst.count(interfaceName))
            throw InterfaceTypeError{"Duplicate interface " + interfaceName};
        interfaceLst[interfaceName] = interfaceDeclNodes.size();
        interfaceDeclNodes.push_back(decl);
    }
    TopologicalSort<InterfaceDecl *> sorter(interfaceDeclNodes);
    for (size_t i = 0; i < interfaceDeclNodes.size(); ++i) {
        for (auto interface: interfaceDeclNodes[i]->getInterfaces()) {
            auto basename = interface->getIdent();
            if (!interfaceLst.count(basename))
                throw InterfaceTypeError{"Unknown interface " + basename};
            sorter.addDirectedEdge(interfaceLst[basename], i);
        }
    }
    return sorter.sort();
}

void AstTypeAnnotator::annotate() {
    annotateInterfaceDecls();
    annotateClassDecls();
    annotateInterfaceBody();
    annotateClassBody();
    annotateFunctionDecls();
    annotateFunctionLits();
    annotateVariableDecls();

    annotateCastAndNewExpr();

    annotateBasicLiterals();
}

void AstTypeAnnotator::annotateClassDecls() {
    auto ordering = sortClassInheritance();
    for (auto decl: ordering) {
        decl->accept(this);
    }
}

void AstTypeAnnotator::annotateInterfaceDecls() {
    auto ordering = sortInterfaceInheritance();
    for (auto decl: ordering) {
        decl->accept(this);
    }
}

void AstTypeAnnotator::annotateInterfaceBody() {
    try {
        auto ordering = sortInterfaceInheritance();
        for (auto decl: ordering) {
            // add interface methods
            for (auto member: decl->getInterfaceMethods()) {
                member->accept(this);
                auto funcDecl = dynamic_cast<FunctionDecl *>(member->getDeclaration());
                auto theInterfaceType = dynamic_cast<InterfaceType *>(decl->getTyp());
                theInterfaceType->addInterfaceMethod(funcDecl->getName()->getIdent(),
                                                     dynamic_cast<MemberType *>(member->getTyp()));
            }
        }
    } catch (CyclicError &err) {
        throw InterfaceTypeError{"Cyclic inheritance detected"};
    }
}

void AstTypeAnnotator::annotateClassBody() {
    try {
        auto ordering = sortClassInheritance();
        for (auto decl: ordering) {
            // add class members
            for (auto member: decl->getMembers()) {
                member->accept(this);
                auto isTypeDecl = dynamic_cast<AggregateType *>(member->getDeclaration());
                if (isTypeDecl) continue;
                auto theClassType = dynamic_cast<ClassType *>(decl->getTyp());
                if (auto var = dynamic_cast<VariableDecl *>(member->getDeclaration())) {
                    theClassType->addMember(var->getName()->getIdent(),
                                            dynamic_cast<MemberType *>(member->getTyp()));
                } else if (auto func = dynamic_cast<FunctionDecl *>(member->getDeclaration())) {
                    theClassType->addMember(func->getName()->getIdent(),
                                            dynamic_cast<MemberType *>(member->getTyp()));
                }
            }
        }
    } catch (CyclicError &err) {
        throw ClassTypeError{"Cyclic inheritance detected"};
    }
}

void AstTypeAnnotator::annotateFunctionDecls() {
    for (auto &decl: astContext->getAstNodesByType<FunctionDecl>()) {
        auto funcDecl = dynamic_cast<FunctionDecl *>(decl.get());
        if (!funcDecl->getTyp()) {
            funcDecl->accept(this);
        }
    }
}

void AstTypeAnnotator::annotateFunctionLits() {
    for (auto &decl: astContext->getAstNodesByType<FunctionLit>()) {
        auto funcDecl = dynamic_cast<FunctionLit *>(decl.get());
        if (!funcDecl->getTyp()) {
            funcDecl->accept(this);
        }
    }
}

void AstTypeAnnotator::annotateVariableDecls() {
    for (auto &decl: astContext->getAstNodesByType<VariableDecl>()) {
        auto varDecl = dynamic_cast<VariableDecl *>(decl.get());
        if (!varDecl->getTyp()) {
            varDecl->accept(this);
        }
    }
}

void AstTypeAnnotator::annotateBasicLiterals() {
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

void AstTypeAnnotator::annotateCastAndNewExpr() {
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

void AstTypeAnnotator::visit(InterfaceDecl *visitable) {
    // find inherited interface types
    std::vector<InterfaceType *> inherited;
    for (auto ident: visitable->getInterfaces()) {
        auto inheritedInterfaceName = ident->getIdent();
        auto interfaceTyp = typeContext->getInterfaceTyp(inheritedInterfaceName);
        if (!interfaceTyp)
            throw InterfaceTypeError{"Unknown interface " + inheritedInterfaceName};
        inherited.push_back(interfaceTyp.value());
    }
    auto interfaceName = visitable->getName()->getIdent();
    auto theInterface = typeContext->createOrGetInterfaceTy(interfaceName, inherited);
    visitable->setTyp(theInterface);
}

void AstTypeAnnotator::visit(ClassDecl *visitable) {
    // find baseclass type
    ClassType *baseclass = nullptr;
    auto basename = visitable->getBaseClassname();
    if (basename) {
        auto baseType = typeContext->getClassTyp(basename.value());
        if (!baseType)
            throw ClassTypeError{"Unknown base class " + basename.value()};
        baseclass = baseType.value();
    }
    std::vector<InterfaceType *> inherited;
    for (auto ident: visitable->getInterfaces()) {
        auto inheritedInterfaceName = ident->getIdent();
        auto interfaceTyp = typeContext->getInterfaceTyp(inheritedInterfaceName);
        if (!interfaceTyp)
            throw InterfaceTypeError{"Unknown interface " + inheritedInterfaceName};
        inherited.push_back(interfaceTyp.value());
    }
    auto classname = visitable->getName()->getIdent();
    auto theClassType = typeContext->createOrGetClassTy(classname, baseclass, inherited);
    visitable->setTyp(theClassType);
}

void AstTypeAnnotator::visit(MemberDecl *visitable) {
    auto modifier = getVisibilityFromString(visitable->getModifier()->getIdent());
    auto declaration = visitable->getDeclaration();
    if (auto ptr = dynamic_cast<VariableDecl *>(declaration)) {
        ptr->accept(this);
        auto type = typeContext->createMemberTy(nullptr, modifier, ptr->getTyp());
        if (!type) throw TypeParseError{"Unable to parse MemberDecl VariableType"};
        visitable->setTyp(type);
    }
    if (auto ptr = dynamic_cast<FunctionDecl *>(declaration)) {
        ptr->accept(this);
        auto type = typeContext->createMemberTy(nullptr, modifier, ptr->getTyp());
        if (!type) throw TypeParseError{"Unable to parse MemberDecl FunctionTypeExpr"};
        visitable->setTyp(type);
    }
    // falls through for class and interface decl
}

void AstTypeAnnotator::visit(FunctionDecl *visitable) {
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

void AstTypeAnnotator::visit(FunctionLit *visitable) {
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

void AstTypeAnnotator::visit(VariableDecl *visitable) {
    auto type = parser.parseTypeExpr(visitable->getVariableType());
    if (!type)
        throw TypeParseError{"Unable to parse VariableDecl type "
                                 + visitable->getName()->getIdent()};
    visitable->setTyp(type);
    visitable->getName()->setTyp(type);
}

void AstTypeAnnotator::visit(ParamDecl *visitable) {
    auto type = parser.parseTypeExpr(visitable->getParamType());
    if (!type)
        throw TypeParseError{"Unable to parse ParamDecl type "
                                 + visitable->getName()->getIdent()};
    visitable->getName()->setTyp(type);
    visitable->setTyp(type);
}

void AstTypeAnnotator::visit(NewExpr *visitable) {
    auto type = parser.parseTypeExpr(visitable->getInstanceTyp());
    if (!type) throw TypeParseError{"Unable to parse NewExpr instance type"};
    visitable->setTyp(type);
}

void AstTypeAnnotator::visit(CastExpr *visitable) {
    auto type = parser.parseTypeExpr(visitable->getTargetTyp());
    if (!type) throw TypeParseError{"Unable to parse CastExpr target type"};
    visitable->setTyp(type);
}

bool isFloatNumber(const std::string &str) {
    return std::find(str.begin(), str.end(), '.') != str.end();
}

void AstTypeAnnotator::visit(NumberLit *visitable) {
    auto value = visitable->getValue();
    if (isFloatNumber(value)) {
        visitable->setTyp(typeContext->getPrimitiveTy(PrimType::Number));
    } else {
        visitable->setTyp(typeContext->getPrimitiveTy(PrimType::Integer));
    }
}

void AstTypeAnnotator::visit(BoolLit *visitable) {
    visitable->setTyp(typeContext->getPrimitiveTy(PrimType::Boolean));
}
void AstTypeAnnotator::visit(NullLit *visitable) {
    visitable->setTyp(typeContext->getPrimitiveTy(PrimType::Null));
}

}
