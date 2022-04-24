//
// Created by Admininstrator on 2022-04-23.
//

#ifndef REFLEX_SRC_AST_ASTDECLARATION_H_
#define REFLEX_SRC_AST_ASTDECLARATION_H_

#include "AST.h"

#include <string>
#include <vector>

#include <ASTUtils.h>

namespace reflex {

class SourceLocation;
class ReferenceTypenameExpr;
class ASTTypeExpr;
class Expression;
class BlockStmt;
class FunctionTypeExpr;

class Declaration : public ASTNode {
  public:
    Declaration(const SourceLocation *loc, std::string declname);

  private:
    const SourceLocation *loc;
    std::string declname;
};

class AggregateDecl : public Declaration {
  public:
    AggregateDecl(const SourceLocation *loc, std::string declname);
};

class FieldDecl;
class MethodDecl;

class ClassDecl : public AggregateDecl {
  public:
    ClassDecl(const SourceLocation *loc,
              std::string declname,
              ReferenceTypenameExpr *baseclass,
              std::vector<ReferenceTypenameExpr *> interfaces,
              std::vector<AggregateDecl *> decls,
              std::vector<FieldDecl *> fields,
              std::vector<MethodDecl *> methods);

  private:
    ReferenceTypenameExpr *baseclass;
    std::vector<ReferenceTypenameExpr *> interfaces;

    std::vector<AggregateDecl *> decls;
    std::vector<FieldDecl *> fields;
    std::vector<MethodDecl *> methods;
};

class InterfaceDecl : public AggregateDecl {
  public:
    InterfaceDecl(const SourceLocation *loc,
                  std::string declname,
                  std::vector<ReferenceTypenameExpr *> interfaces,
                  std::vector<AggregateDecl *> decls,
                  std::vector<MethodDecl *> methods);

  private:
    std::vector<ReferenceTypenameExpr *> interfaces;

    std::vector<AggregateDecl *> decls;
    std::vector<MethodDecl *> methods;
};

class VariableDecl : public Declaration {
  public:
    VariableDecl(const SourceLocation *loc,
                 std::string declname,
                 ASTTypeExpr *type_decl,
                 Expression *initializer = nullptr);
  protected:
    ASTTypeExpr *typeDecl;
    Expression *initializer;
};

class FieldDecl : public VariableDecl {
  public:
    FieldDecl(const SourceLocation *loc,
              std::string declname,
              ASTTypeExpr *type_decl,
              ClassDecl *parent,
              Visibility visibility,
              Expression *initializer = nullptr);

  private:
    ClassDecl *parent;
    Visibility visibility;
};

class FunctionDecl;
class ParamDecl : public VariableDecl {
  public:
    ParamDecl(const SourceLocation *loc,
              std::string declname,
              ASTTypeExpr *type_decl,
              FunctionDecl *parent,
              Expression *initializer = nullptr);

  private:
    FunctionDecl *parent;
};

class FunctionDecl : public Declaration {
  public:
    FunctionDecl(const SourceLocation *loc,
                 std::string declname,
                 std::vector<ParamDecl *> param_decls,
                 ASTTypeExpr *return_type_decl,
                 BlockStmt *body);
  protected:
    std::vector<ParamDecl *> paramDecls;
    ASTTypeExpr *returnTypeDecl;
    BlockStmt *body;
};

class MethodDecl : public FunctionDecl {
  public:
    MethodDecl(const SourceLocation *loc,
               std::string declname,
               std::vector<ParamDecl *> param_decls,
               ASTTypeExpr *return_type_decl,
               BlockStmt *body,
               AggregateDecl *parent,
               Visibility visibility);
  private:
    AggregateDecl *parent;
    Visibility visibility;
};

}

#endif //REFLEX_SRC_AST_ASTDECLARATION_H_
