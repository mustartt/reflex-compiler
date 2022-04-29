//
// Created by Admininstrator on 2022-04-23.
//

#ifndef REFLEX_SRC_AST_AST_H_
#define REFLEX_SRC_AST_AST_H_

namespace reflex {

class SourceLocation;
class ScopeMember;

class ASTNode {
  public:
    explicit ASTNode(const SourceLocation *loc);
    virtual ~ASTNode() = default;

    [[nodiscard]] const SourceLocation *location() const;

  private:
    const SourceLocation *loc;
};

}

#endif //REFLEX_SRC_AST_AST_H_
