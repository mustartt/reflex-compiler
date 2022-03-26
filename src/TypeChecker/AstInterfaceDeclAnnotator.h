//
// Created by henry on 2022-03-26.
//

#ifndef REFLEX_SRC_TYPECHECKER_ASTINTERFACEDECLANNOTATOR_H_
#define REFLEX_SRC_TYPECHECKER_ASTINTERFACEDECLANNOTATOR_H_

#include <ostream>

#include <AstVisitor.h>
#include <TypeContextManager.h>
#include <AstNodes.h>
#include <list>

namespace reflex {

struct InterfaceNode {
  public:
    InterfaceNode *parent{};
    InterfaceDecl *self{};
    std::string qualifiedName;
    std::vector<std::string> derivedFrom;
    std::list<std::unique_ptr<InterfaceNode>> children;

    void debugHoistTree(std::ostream &os, size_t indent);
};

class AstInterfaceDeclAnnotator : public AstVisitor {
    TypeContextManager *typeContext;
    CompilationUnit *root;

    std::vector<InterfaceNode *> interfaceDecls{};
    std::unordered_map<InterfaceDecl *, InterfaceNode *> declToQualifierMap{};
    std::unordered_map<std::string, size_t> interfaceLst{};

    std::unique_ptr<InterfaceNode> hoistRoot = std::make_unique<InterfaceNode>();
  public:
    AstInterfaceDeclAnnotator(TypeContextManager *typeContext, CompilationUnit *root);

    void annotate();

    void visit(InterfaceDecl *visitable) override;

  private:
    std::unique_ptr<InterfaceNode> generateInterfaceHoistTree(Declaration *decl);
    std::vector<InterfaceNode *> getInterfaceTypeConstructionOrder();
    void getHoistedInterfaceTree();
};

}

#endif //REFLEX_SRC_TYPECHECKER_ASTINTERFACEDECLANNOTATOR_H_
