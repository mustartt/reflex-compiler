//
// Created by henry on 2022-03-26.
//

#include <iostream>
#include "Utils/TopSort.h"
#include "Utils/TypeCheckerUtils.h"
#include "AstClassDeclAnnotator.h"
#include "SemanticError.h"

namespace reflex {

AstClassDeclAnnotator::AstClassDeclAnnotator(TypeContextManager *typeContext, CompilationUnit *root) : typeContext(
    typeContext), root(root) {}

void ClassNode::debugHoistTree(std::ostream &os, size_t indent) {
    printIndent(os, indent);
    os << "class " << qualifiedName;
    if (derivedFrom) {
        os << " (" << derivedFrom.value() << ")";
    }
    if (!interfaces.empty()) {
        os << " :";
        for (auto &interface: interfaces) {
            os << " " << interface;
        }
    }
    os << " {" << std::endl;
    for (auto &child: children) {
        child->debugHoistTree(os, indent + 1);
    }
    printIndent(os, indent);
    os << "}" << std::endl;
}

void AstClassDeclAnnotator::flattenHoistTree(std::unique_ptr<ClassNode> &root) {
    if (root->children.empty()) return;
    // flatten children
    for (auto &child: root->children) {
        flattenHoistTree(child);
    }
    // hoist to parent
    while (!root->children.empty() && root->parent) {
        auto child = std::move(root->children.back());
        root->children.pop_back();
        child->qualifiedName = root->qualifiedName + "::" + child->qualifiedName;
        if (child->derivedFrom) {
            auto &derived = child->derivedFrom.value();
            auto res = std::find_if(root->children.begin(), root->children.end(),
                                    [&](const auto &childPtr) {
                                      return derived.starts_with(childPtr->qualifiedName);
                                    });
            if (res != root->children.end()) {
                derived = root->qualifiedName + "::" + derived;
            }
        }
        for (auto &interface: child->interfaces) {
            auto res = std::find_if(root->children.begin(), root->children.end(),
                                    [&](const auto &childPtr) {
                                      return interface.starts_with(childPtr->qualifiedName);
                                    });
            if (res != root->children.end()) {
                interface = root->qualifiedName + "::" + interface;
            }
        }
        root->parent->children.push_back(std::move(child));
    }
}

std::vector<ClassNode *> AstClassDeclAnnotator::getClassTypeConstructionOrder() {
    for (auto &cls: hoistRoot->children) {
        const auto &classname = cls->qualifiedName;
        if (classLst.count(classname))
            throw ClassTypeError{"Duplicate class " + classname};
        classLst[classname] = classDecls.size();
        classDecls.push_back(cls.get());
        declToQualifierMap[cls->self] = cls.get();
    }
    TopologicalSort<ClassNode *> sorter(classDecls);
    for (size_t i = 0; i < classDecls.size(); ++i) {
        if (classDecls[i]->derivedFrom) {
            auto &basename = classDecls[i]->derivedFrom.value();
            if (!classLst.count(basename))
                throw InterfaceTypeError{"Unknown class " + basename};
            sorter.addDirectedEdge(classLst[basename], i);
        }
    }
    return sorter.sort();
}

std::unique_ptr<ClassNode> AstClassDeclAnnotator::generateClassHoistTree(Declaration *decl) {
    if (auto interface = dynamic_cast<InterfaceDecl *>(decl)) {
        auto hoistNode = std::make_unique<ClassNode>();
        hoistNode->self = nullptr;
        hoistNode->qualifiedName = interface->getInterfaceName();
        return hoistNode;
    }
    if (auto cls = dynamic_cast<ClassDecl *>(decl)) {
        auto hoistNode = std::make_unique<ClassNode>();
        hoistNode->qualifiedName = cls->getClassname();
        hoistNode->self = cls;
        if (cls->getBaseClassname()) {
            hoistNode->derivedFrom = cls->getBaseClassname().value();
        }
        for (auto interface: cls->getInterfaces()) {
            hoistNode->interfaces.push_back(interface->getIdent());
        }
        for (auto member: cls->getMembers()) {
            if (isInterfaceOrClassDecl(member->getDeclaration())) {
                auto childNode = generateClassHoistTree(member->getDeclaration());
                if (childNode) {
                    childNode->parent = hoistNode.get();
                    hoistNode->children.push_back(std::move(childNode));
                }
            }
        }
        return hoistNode;
    }
    return {nullptr};
}

void AstClassDeclAnnotator::getHoistedClassTree() {
    for (auto decl: root->getDecls()) {
        if (isClassDecl(decl)) {
            auto childNode = generateClassHoistTree(decl);
            if (childNode) {
                childNode->parent = hoistRoot.get();
                hoistRoot->children.push_back(std::move(childNode));
            }
        }
    }
    flattenHoistTree(hoistRoot);
    hoistRoot->children.remove_if([](const auto &child) {
      return !child->self; // clean up class decl nodes
    });
    hoistRoot->debugHoistTree(std::cout, 0);
}

void AstClassDeclAnnotator::annotate() {
    getHoistedClassTree();

    try {
        auto ordering = getClassTypeConstructionOrder();
        for (auto i: ordering) {
            i->self->accept(this);
        }
    } catch (CyclicError &err) {
        throw ClassTypeError{"Cyclic Class inheritance detected"};
    }
}

void AstClassDeclAnnotator::visit(ClassDecl *visitable) {
    ClassType *baseclass = nullptr;
    auto classNode = declToQualifierMap[visitable];
    if (classNode->derivedFrom) {
        auto classTyp = typeContext->getClassTyp(classNode->derivedFrom.value());
        if (!classTyp)
            throw ClassTypeError{"Unknown class " + classNode->derivedFrom.value()};
        baseclass = classTyp.value();
    }
    std::vector<InterfaceType *> implements;
    for (auto &interfacename: classNode->interfaces) {
        auto interfaceTyp = typeContext->getInterfaceTyp(interfacename);
        if (!interfaceTyp)
            throw ClassTypeError{"Unknown interface " + interfacename};
        implements.push_back(interfaceTyp.value());
    }
    auto &classname = classNode->qualifiedName;
    auto theClass = typeContext->createOrGetClassTy(classname, baseclass, implements);
    visitable->setTyp(theClass);
}

}
