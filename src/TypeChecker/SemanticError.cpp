//
// Created by henry on 2022-03-26.
//

#include "SemanticError.h"

namespace reflex {

TypeParseError::TypeParseError(const std::string &arg)
    : runtime_error("TypeParserError: " + arg) {}

ClassTypeError::ClassTypeError(const std::string &arg)
    : runtime_error("ClassTypeError: " + arg) {}

InterfaceTypeError::InterfaceTypeError(const std::string &arg)
    : runtime_error("InterfaceTypeError: " + arg) {}

SemanticError::SemanticError(const std::string &arg)
    : std::runtime_error("Semantic Error: " + arg) {}
}
