//
// Created by Admininstrator on 2022-04-23.
//

#ifndef REFLEX_SRC_AST_ASTTYPE_H_
#define REFLEX_SRC_AST_ASTTYPE_H_

#include "AST.h"

#include <string>
#include <vector>

namespace reflex {

class SourceLocation;
class NumberLiteral;

class ASTTypeExpr : public ASTNode {
  public:
    explicit ASTTypeExpr(const SourceLocation *loc);
};

class ReferenceTypenameExpr : public ASTTypeExpr {
  public:
    explicit ReferenceTypenameExpr(const SourceLocation *loc);

    [[nodiscard]] virtual std::string getQualifiedString() const = 0;
};

class BaseTypenameExpr : public ReferenceTypenameExpr {
  public:
    BaseTypenameExpr(const SourceLocation *loc, std::string type_name);

    [[nodiscard]] std::string getQualifiedString() const override;
    const std::string &getTypeName() const { return typeName; }
  private:
    std::string typeName;
};

class QualifiedTypenameExpr : public ReferenceTypenameExpr {
  public:
    QualifiedTypenameExpr(const SourceLocation *loc, std::string name, ReferenceTypenameExpr *prefix);

    [[nodiscard]] std::string getQualifiedString() const override;

  private:
    ReferenceTypenameExpr *prefix;
    std::string name;
};

class ArrayTypeExpr : public ASTTypeExpr {
  public:
    ArrayTypeExpr(const SourceLocation *loc, ASTTypeExpr *element_type, NumberLiteral *size);

    NumberLiteral *getSize() const { return size; }
    ASTTypeExpr *getElementType() const { return elementType; }

  private:
    NumberLiteral *size;
    ASTTypeExpr *elementType;
};

class FunctionTypeExpr : public ASTTypeExpr {
  public:
    FunctionTypeExpr(const SourceLocation *loc,
                     ASTTypeExpr *return_type,
                     std::vector<ASTTypeExpr *> param_types);

    ASTTypeExpr *getReturnType() const { return returnType; }
    const std::vector<ASTTypeExpr *> &getParamTypes() const { return paramTypes; }

  private:
    ASTTypeExpr *returnType;
    std::vector<ASTTypeExpr *> paramTypes;
};

}

#endif //REFLEX_SRC_AST_ASTTYPE_H_
