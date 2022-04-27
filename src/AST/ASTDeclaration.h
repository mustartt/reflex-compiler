//
// Created by Admininstrator on 2022-04-23.
//

#ifndef REFLEX_SRC_AST_ASTDECLARATION_H_
#define REFLEX_SRC_AST_ASTDECLARATION_H_

#include "AST.h"

#include <string>
#include <vector>

#include <ASTUtils.h>
#include <ASTVisitor.h>

namespace reflex {

class SourceLocation;
class ReferenceTypenameExpr;
class ASTTypeExpr;
class Expression;
class BlockStmt;
class FunctionTypeExpr;
class Type;

class Declaration : public ASTNode, public ASTDeclVisitable {
  public:
    Declaration(const SourceLocation *loc, std::string declname);

    const std::string &getDeclname() const { return declname; }

    Type *getType() const { return type; }
    void setType(Type *typ) { Declaration::type = typ; }

  private:
    std::string declname;
    Type *type = nullptr;
};

class AggregateDecl : public Declaration {
  public:
    AggregateDecl(const SourceLocation *loc, Visibility visibility, std::string declname);

    Visibility getVisibility() const { return visibility; }
  private:
    Visibility visibility;
};

class FieldDecl;
class MethodDecl;

class ClassDecl : public AggregateDecl {
  public:
    ClassDecl(const SourceLocation *loc,
              Visibility visibility,
              std::string declname,
              ReferenceTypenameExpr *baseclass,
              std::vector<ReferenceTypenameExpr *> interfaces,
              std::vector<AggregateDecl *> decls = {},
              std::vector<FieldDecl *> fields = {},
              std::vector<MethodDecl *> methods = {});

    ReferenceTypenameExpr *getBaseclass() const { return baseclass; }
    const std::vector<ReferenceTypenameExpr *> &getInterfaces() const { return interfaces; }
    const std::vector<AggregateDecl *> &getDecls() const { return decls; }
    const std::vector<FieldDecl *> &getFields() const { return fields; }
    const std::vector<MethodDecl *> &getMethods() const { return methods; }

    void addMemberDecl(AggregateDecl *decl) { decls.push_back(decl); }
    void addMemberDecl(FieldDecl *decl) { fields.push_back(decl); }
    void addMemberDecl(MethodDecl *decl) { methods.push_back(decl); }

    ASTDeclVisitorDispatcher
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
                  Visibility visibility,
                  std::string declname,
                  std::vector<ReferenceTypenameExpr *> interfaces,
                  std::vector<AggregateDecl *> decls = {},
                  std::vector<MethodDecl *> methods = {});

    const std::vector<ReferenceTypenameExpr *> &getInterfaces() const { return interfaces; }
    const std::vector<AggregateDecl *> &getDecls() const { return decls; }
    const std::vector<MethodDecl *> &getMethods() const { return methods; }

    void addMemberDecl(MethodDecl *decl) { methods.push_back(decl); }
    void addMemberDecl(InterfaceDecl *decl) { decls.push_back(decl); }

    ASTDeclVisitorDispatcher
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

    ASTTypeExpr *getTypeDecl() const { return typeDecl; }
    Expression *getInitializer() const { return initializer; }

    ASTDeclVisitorDispatcher
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

    ClassDecl *getParent() const { return parent; }
    Visibility getVisibility() const { return visibility; }

    ASTDeclVisitorDispatcher
  private:
    ClassDecl *parent;
    Visibility visibility;
};

class FunctionDecl;
class ParamDecl : public VariableDecl {
  public:
    ParamDecl(const SourceLocation *loc,
              std::string declname,
              ASTTypeExpr *type_decl);

    FunctionDecl *getParent() const { return parent; }

    ASTDeclVisitorDispatcher
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

    const std::vector<ParamDecl *> &getParamDecls() const { return paramDecls; }
    ASTTypeExpr *getReturnTypeDecl() const { return returnTypeDecl; }
    BlockStmt *getBody() const { return body; }

    ASTDeclVisitorDispatcher
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

    AggregateDecl *getParent() const { return parent; }
    Visibility getVisibility() const { return visibility; }

    ASTDeclVisitorDispatcher
  private:
    AggregateDecl *parent;
    Visibility visibility;
};

class CompilationUnit : public Declaration {
  public:
    CompilationUnit(const SourceLocation *loc,
                    std::string declname,
                    std::vector<Declaration *> decls = {});

    void addDecl(Declaration *decl);
    Declaration *getDecl(size_t index);
    [[nodiscard]] const std::vector<Declaration *> &getDecls() const;

    ASTDeclVisitorDispatcher
  private:
    std::vector<Declaration *> decls;
};

}

#endif //REFLEX_SRC_AST_ASTDECLARATION_H_
