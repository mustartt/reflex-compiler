//
// Created by henry on 2022-04-24.
//

#include "AST.h"

#include "SourceManager.h"

namespace reflex {

ASTNode::ASTNode(const SourceLocation *loc) : loc(loc) {}
const SourceLocation *ASTNode::location() const { return loc; }

}

