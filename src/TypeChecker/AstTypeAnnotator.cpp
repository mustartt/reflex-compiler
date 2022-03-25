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
    auto ordering = sortInterfaceInheritance();
    for (auto decl: ordering) {
        // add interface methods
        for (auto member: decl->getSignatures()) {
            member->accept(this);
            auto funcDecl = dynamic_cast<FunctionDecl *>(member->getDeclaration());
            auto theInterfaceType = dynamic_cast<InterfaceType *>(decl->getTyp());
            theInterfaceType->addInterfaceMethod(funcDecl->getName()->getIdent(),
                                                 dynamic_cast<MemberType *>(member->getTyp()));
        }
    }
}

void AstTypeAnnotator::annotateClassBody() {
    auto ordering = sortClassInheritance();
    for (auto decl: ordering) {
        // add class members
        for (auto member: decl->getMembers()) {
            member->accept(this);
            auto funcDecl = dynamic_cast<FunctionDecl *>(member->getDeclaration());
            auto theClassType = dynamic_cast<ClassType *>(decl->getTyp());
            theClassType->addMember(funcDecl->getName()->getIdent(),
                                    dynamic_cast<MemberType *>(member->getTyp()));
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
        paramTypes.push_back(p->getTyp());
    }
    TypeParser typeParser(typeContext);
    auto retType = typeParser.parseTypeExpr(visitable->getRetTyp());
    if (!retType)
        throw TypeParseError{"Unable to parse FunctionDecl type "
                                 + visitable->getName()->getIdent()};
    visitable->setTyp(typeContext->getFunctionTy(retType, paramTypes));
}
void AstTypeAnnotator::visit(VariableDecl *visitable) {
    auto type = parser.parseTypeExpr(visitable->getVariableType());
    if (!type)
        throw TypeParseError{"Unable to parse VariableDecl type "
                                 + visitable->getName()->getIdent()};
    visitable->setTyp(type);
}
void AstTypeAnnotator::visit(ParamDecl *visitable) {
    auto type = parser.parseTypeExpr(visitable->getParamType());
    if (!type)
        throw TypeParseError{"Unable to parse ParamDecl type "
                                 + visitable->getName()->getIdent()};
    visitable->setTyp(type);
}

}
