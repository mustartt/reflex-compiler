//
// Created by Admininstrator on 2022-04-23.
//

#include "ASTUtils.h"

#include <stdexcept>

namespace reflex {

std::string getVisibilityString(Visibility visibility) {
    switch (visibility) {
        case Visibility::Public: return "public";
        case Visibility::Private: return "private";
        case Visibility::Protected: return "protected";
        case Visibility::Static: return "static";
    }
}

Visibility getVisibilityFromString(const std::string &ident) {
    if (ident == "public") return Visibility::Public;
    if (ident == "private") return Visibility::Private;
    if (ident == "protected") return Visibility::Protected;
    if (ident == "static") return Visibility::Static;
    throw std::runtime_error("Unknown access modifier " + ident);
}

}
