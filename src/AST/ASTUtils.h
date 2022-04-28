//
// Created by Admininstrator on 2022-04-23.
//

#ifndef REFLEX_SRC_AST_ASTUTILS_H_
#define REFLEX_SRC_AST_ASTUTILS_H_

#include <string>
#include <exception>

namespace reflex {

enum class Visibility {
  Public, Private, Protected, Static
};

std::string getVisibilityString(Visibility visibility);
Visibility getVisibilityFromString(const std::string &ident);

}

#endif //REFLEX_SRC_AST_ASTUTILS_H_
