//
// Created by Admininstrator on 2022-04-23.
//

#include "ASTType.h"

namespace reflex {

ASTTypeExpr::ASTTypeExpr(const SourceLocation *loc) : ASTNode(loc) {}

ReferenceTypenameExpr::ReferenceTypenameExpr(const SourceLocation *loc) : ASTTypeExpr(loc) {}

std::string BaseTypenameExpr::getQualifiedString() const {
    return typeName;
}

BaseTypenameExpr::BaseTypenameExpr(const SourceLocation *loc, std::string type_name)
    : ReferenceTypenameExpr(loc), typeName(std::move(type_name)) {}

QualifiedTypenameExpr::QualifiedTypenameExpr(const SourceLocation *loc,
                                             std::string name,
                                             ReferenceTypenameExpr *prefix)
    : ReferenceTypenameExpr(loc), name(std::move(name)), prefix(prefix) {}

std::string QualifiedTypenameExpr::getQualifiedString() const {
    return prefix->getQualifiedString() + "::" + name;
}

ArrayTypeExpr::ArrayTypeExpr(const SourceLocation *loc, ASTTypeExpr *element_type, NumberLiteral *size)
    : ASTTypeExpr(loc), elementType(element_type), size(size) {}

FunctionTypeExpr::FunctionTypeExpr(const SourceLocation *loc,
                                   ASTTypeExpr *return_type,
                                   std::vector<ASTTypeExpr *> param_types)
    : ASTTypeExpr(loc), returnType(return_type), paramTypes(std::move(param_types)) {}
}
