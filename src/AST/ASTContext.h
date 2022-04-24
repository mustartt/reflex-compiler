//
// Created by Admininstrator on 2022-04-23.
//

#ifndef REFLEX_SRC_AST_ASTCONTEXT_H_
#define REFLEX_SRC_AST_ASTCONTEXT_H_

#include "AST.h"

#include <typeindex>
#include <vector>
#include <memory>
#include <unordered_map>

namespace reflex {

class ASTContext {
    using AstContextList = std::vector<std::unique_ptr<ASTNode>>;
  public:
    ASTContext() = default;
    ASTContext(const ASTContext &) = delete;
    ASTContext(ASTContext &&) noexcept = default;

    template<class ASTType, class... Arguments>
    ASTType *create(Arguments &&...args);

  private:
    std::unordered_map<std::type_index, AstContextList> ctx;
};

template<typename ASTType, typename... Arguments>
ASTType *ASTContext::create(Arguments &&... args) {
    auto node = std::make_unique<ASTType>(std::forward<Arguments>(args)...);
    auto tmp = node.get();
    ctx[typeid(ASTType)].push_back(std::move(node));
    return tmp;
}

}

#endif //REFLEX_SRC_AST_ASTCONTEXT_H_
