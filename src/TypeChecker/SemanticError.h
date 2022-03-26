//
// Created by henry on 2022-03-26.
//

#ifndef REFLEX_SRC_TYPECHECKER_SEMANTICERROR_H_
#define REFLEX_SRC_TYPECHECKER_SEMANTICERROR_H_

#include <stdexcept>
#include <string>

namespace reflex {

class TypeParseError : public std::runtime_error {
  public:
    explicit TypeParseError(const std::string &arg);
};

class ClassTypeError : public std::runtime_error {
  public:
    explicit ClassTypeError(const std::string &arg);
};

class InterfaceTypeError : public std::runtime_error {
  public:
    explicit InterfaceTypeError(const std::string &arg);
};

class SemanticError : public std::runtime_error {
  public:
    explicit SemanticError(const std::string &arg);
};

}

#endif //REFLEX_SRC_TYPECHECKER_SEMANTICERROR_H_
