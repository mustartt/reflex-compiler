//
// Created by henry on 2022-03-26.
//

#ifndef REFLEX_SRC_TYPECHECKER_ASTCLASSDECLANNOTATOR_H_
#define REFLEX_SRC_TYPECHECKER_ASTCLASSDECLANNOTATOR_H_

#include <string>
#include <optional>
#include <memory>
#include <list>
#include <ostream>
#include <unordered_map>
#include <AstNodes.h>
#include <TypeContextManager.h>

namespace reflex {

struct ClassNode {
  public:
    ClassNode *parent{};
    ClassDecl *self{};
    std::string qualifiedName;
    std::optional<std::string> derivedFrom;
    std::vector<std::string> interfaces;
    std::list<std::unique_ptr<ClassNode>> children;

    void debugHoistTree(std::ostream &os, size_t indent);
};

class AstClassDeclAnnotator : public AstVisitor {
    TypeContextManager *typeContext;
    CompilationUnit *root;

    std::vector<ClassNode *> classDecls{};
    std::unordered_map<ClassDecl *, ClassNode *> declToQualifierMap{};
    std::unordered_map<std::string, size_t> classLst{};

    std::unique_ptr<ClassNode> hoistRoot = std::make_unique<ClassNode>();
  public:
    AstClassDeclAnnotator(TypeContextManager *typeContext, CompilationUnit *root);

    void annotate();

    void visit(ClassDecl *visitable) override;

  private:
    std::unique_ptr<ClassNode> generateClassHoistTree(Declaration *decl);
    std::vector<ClassNode *> getClassTypeConstructionOrder();
    void getHoistedClassTree();
    void flattenHoistTree(std::unique_ptr<ClassNode> &root);
};

}

#endif //REFLEX_SRC_TYPECHECKER_ASTCLASSDECLANNOTATOR_H_
