//
// Created by henry on 2022-03-26.
//

#include <iostream>
#include "AstInterfaceDeclAnnotator.h"
#include "TopologicalSort/TopSort.h"
#include "SemanticError.h"

namespace reflex {

AstInterfaceDeclAnnotator::AstInterfaceDeclAnnotator(TypeContextManager *typeContext, CompilationUnit *root)
    : typeContext(typeContext), root(root) {}

bool isInterfaceOrClassDecl(Declaration *decl) {
    return typeid(*decl) == typeid(ClassDecl)
        || typeid(*decl) == typeid(InterfaceDecl);
}

void printIndent(std::ostream &os, size_t indent) {
    for (size_t i = 0; i < indent; ++i) os << "  ";
}

void InterfaceNode::debugHoistTree(std::ostream &os, size_t indent) {
    printIndent(os, indent);
    os << "interface " << qualifiedName;
    if (!derivedFrom.empty()) {
        os << " :";
        for (auto &derived: derivedFrom) {
            os << " " << derived;
        }
    }
    os << " {" << std::endl;
    for (auto &child: children) {
        child->debugHoistTree(os, indent + 1);
    }
    printIndent(os, indent);
    os << "}" << std::endl;
}

std::unique_ptr<InterfaceNode> AstInterfaceDeclAnnotator::generateInterfaceHoistTree(Declaration *decl) {
    if (auto interface = dynamic_cast<InterfaceDecl *>(decl)) {
        auto hoistNode = std::make_unique<InterfaceNode>();
        hoistNode->self = interface;
        hoistNode->qualifiedName = interface->getInterfaceName();
        for (auto derived: interface->getInterfaces()) {
            hoistNode->derivedFrom.push_back(derived->getIdent());
        }
        for (auto member: interface->getInterfaceMembers()) {
            if (isInterfaceOrClassDecl(member->getDeclaration())) {
                auto childNode = generateInterfaceHoistTree(member->getDeclaration());
                if (childNode) {
                    childNode->parent = hoistNode.get();
                    hoistNode->children.push_back(std::move(childNode));
                }
            }
        }
        return hoistNode;
    }
    if (auto cls = dynamic_cast<ClassDecl *>(decl)) {
        auto hoistNode = std::make_unique<InterfaceNode>();
        hoistNode->qualifiedName = cls->getClassname();
        for (auto member: cls->getMembers()) {
            if (isInterfaceOrClassDecl(member->getDeclaration())) {
                auto childNode = generateInterfaceHoistTree(member->getDeclaration());
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

void flattenHoistTree(std::unique_ptr<InterfaceNode> &root) {
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

        for (auto &derived: child->derivedFrom) {
            auto res = std::find_if(root->children.begin(), root->children.end(),
                                    [&](const auto &childPtr) {
                                      return derived.starts_with(childPtr->qualifiedName);
                                    });
            if (res != root->children.end()) {
                derived = root->qualifiedName + "::" + derived;
            }
        }
        root->parent->children.push_back(std::move(child));
    }
}

void AstInterfaceDeclAnnotator::getHoistedInterfaceTree() {
    for (auto decl: root->getDecls()) {
        if (isInterfaceOrClassDecl(decl)) {
            auto childNode = generateInterfaceHoistTree(decl);
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

std::vector<InterfaceNode *> AstInterfaceDeclAnnotator::getInterfaceTypeConstructionOrder() {
    for (auto &interface: hoistRoot->children) {
        const auto &interfaceName = interface->qualifiedName;
        if (interfaceLst.count(interfaceName))
            throw InterfaceTypeError{"Duplicate interface " + interfaceName};
        interfaceLst[interfaceName] = interfaceDecls.size();
        interfaceDecls.push_back(interface.get());
        declToQualifierMap[interface->self] = interface.get();
    }
    TopologicalSort<InterfaceNode *> sorter(interfaceDecls);
    for (size_t i = 0; i < interfaceDecls.size(); ++i) {
        for (auto &basename: interfaceDecls[i]->derivedFrom) {
            if (!interfaceLst.count(basename))
                throw InterfaceTypeError{"Unknown interface " + basename};
            sorter.addDirectedEdge(interfaceLst[basename], i);
        }
    }
    return sorter.sort();
}

void AstInterfaceDeclAnnotator::annotate() {
    getHoistedInterfaceTree();
    try {
        auto ordering = getInterfaceTypeConstructionOrder();
        for (auto i: ordering) {
            i->self->accept(this);
        }
    } catch (CyclicError &err) {
        throw InterfaceTypeError{"Cyclic inheritance detected"};
    }
}

void AstInterfaceDeclAnnotator::visit(InterfaceDecl *visitable) {
    std::vector<InterfaceType *> inherited;
    auto interfaceNode = declToQualifierMap[visitable];
    for (auto &inheritedInterfaceName: interfaceNode->derivedFrom) {
        auto interfaceTyp = typeContext->getInterfaceTyp(inheritedInterfaceName);
        if (!interfaceTyp)
            throw InterfaceTypeError{"Unknown interface " + inheritedInterfaceName};
        inherited.push_back(interfaceTyp.value());
    }
    auto &interfacename = interfaceNode->qualifiedName;
    auto theInterface = typeContext->createOrGetInterfaceTy(interfacename, inherited);
    visitable->setTyp(theInterface);
}

}
