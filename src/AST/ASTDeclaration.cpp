//
// Created by Admininstrator on 2022-04-23.
//

#include "ASTDeclaration.h"

namespace reflex {

Declaration::Declaration(const SourceLocation *loc, std::string declname)
    : ASTNode(loc), declname(std::move(declname)) {}

AggregateDecl::AggregateDecl(const SourceLocation *loc, Visibility visibility, std::string declname)
    : Declaration(loc, std::move(declname)), visibility(visibility) {}

ClassDecl::ClassDecl(const SourceLocation *loc,
                     Visibility visibility,
                     std::string declname,
                     ReferenceTypenameExpr *baseclass,
                     std::vector<ReferenceTypenameExpr *> interfaces,
                     std::vector<AggregateDecl *> decls,
                     std::vector<FieldDecl *> fields,
                     std::vector<MethodDecl *> methods)
    : AggregateDecl(loc, visibility, std::move(declname)),
      baseclass(baseclass),
      interfaces(std::move(interfaces)),
      decls(std::move(decls)),
      fields(std::move(fields)),
      methods(std::move(methods)) {}

InterfaceDecl::InterfaceDecl(const SourceLocation *loc,
                             Visibility visibility,
                             std::string declname,
                             std::vector<ReferenceTypenameExpr *> interfaces,
                             std::vector<AggregateDecl *> decls,
                             std::vector<MethodDecl *> methods)
    : AggregateDecl(loc, visibility, std::move(declname)),
      interfaces(std::move(interfaces)),
      decls(std::move(decls)),
      methods(std::move(methods)) {}

VariableDecl::VariableDecl(const SourceLocation *loc,
                           std::string declname,
                           ASTTypeExpr *type_decl,
                           Expression *initializer)
    : Declaration(loc, std::move(declname)),
      typeDecl(type_decl),
      initializer(initializer) {}

FieldDecl::FieldDecl(const SourceLocation *loc,
                     std::string declname,
                     ASTTypeExpr *type_decl,
                     ClassDecl *parent,
                     Visibility visibility,
                     Expression *initializer)
    : VariableDecl(loc, std::move(declname), type_decl, initializer),
      parent(parent),
      visibility(visibility) {}

ParamDecl::ParamDecl(const SourceLocation *loc,
                     std::string declname,
                     ASTTypeExpr *type_decl)
    : VariableDecl(loc, std::move(declname), type_decl, nullptr), parent(nullptr) {}

FunctionDecl::FunctionDecl(const SourceLocation *loc,
                           std::string declname,
                           std::vector<ParamDecl *> param_decls,
                           ASTTypeExpr *return_type_decl,
                           BlockStmt *body) : Declaration(loc, std::move(declname)),
                                              paramDecls(std::move(param_decls)),
                                              returnTypeDecl(return_type_decl),
                                              body(body) {}

MethodDecl::MethodDecl(const SourceLocation *loc,
                       std::string declname,
                       std::vector<ParamDecl *> param_decls,
                       ASTTypeExpr *return_type_decl,
                       BlockStmt *body,
                       AggregateDecl *parent,
                       Visibility visibility)
    : FunctionDecl(loc,
                   std::move(declname),
                   std::move(param_decls),
                   return_type_decl,
                   body),
      parent(parent),
      visibility(visibility) {}

CompilationUnit::CompilationUnit(const SourceLocation *loc,
                                 std::string declname,
                                 std::vector<Declaration *> decls)
    : Declaration(loc, std::move(declname)), decls(std::move(decls)) {}

void CompilationUnit::addDecl(Declaration *decl) {
    decls.push_back(decl);
}

Declaration *CompilationUnit::getDecl(size_t index) {
    return decls.at(index);
}

const std::vector<Declaration *> &CompilationUnit::getDecls() const {
    return decls;
}

}
