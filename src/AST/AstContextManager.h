//
// Created by henry on 2022-03-20.
//

#ifndef REFLEX_SRC_AST_ASTCONTEXTMANAGER_H_
#define REFLEX_SRC_AST_ASTCONTEXTMANAGER_H_

#include "Ast.h"

#include <unordered_map>
#include <vector>
#include <memory>
#include <typeindex>

namespace reflex {

class AstContextManager {
    using AstContextList = std::vector<std::unique_ptr<AstExpr>>;
    std::unordered_map<std::type_index, AstContextList> ctx;
  public:
    AstContextManager() = default;
    AstContextManager(const AstContextManager &) = delete;
    AstContextManager(AstContextManager &&) = default;

    template<class AstType>
    class AstWalker {
        AstContextList *lst;
      public:
        explicit AstWalker(AstContextList *lst) : lst(lst) {}
        class Iterator {
            AstContextList::iterator iter;
          public:
            explicit Iterator(const AstContextList::iterator &iter) : iter(iter) {}
            bool operator!=(const Iterator &other) { return other.iter != iter; }
            AstType *operator*() { static_cast<AstType *>((*iter).get()); }
            Iterator operator++() {
                ++iter;
                return this;
            }
        };
        Iterator begin() { return {lst->begin()}; }
        Iterator end() { return {lst->end()}; }
    };

    template<class AstType, class... Arguments>
    AstType *create(Arguments &&...args);

    template<class AstType>
    AstWalker<AstType> getAstByType();
};

template<typename AstType, typename... Arguments>
AstType *AstContextManager::create(Arguments &&... args) {
    auto node = std::make_unique<AstType>(std::forward<Arguments>(args)...);
    auto tmp = node.get();
    ctx[typeid(AstType)].push_back(std::move(node));
    return tmp;
}

template<class AstType>
AstContextManager::AstWalker<AstType> AstContextManager::getAstByType() {
    return {&ctx[typeid(AstType)]};
}

}

#endif //REFLEX_SRC_AST_ASTCONTEXTMANAGER_H_
