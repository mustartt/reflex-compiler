//
// Created by Admininstrator on 2022-04-25.
//

#ifndef REFLEX_SRC_AST_UTILS_GENERICS_H_
#define REFLEX_SRC_AST_UTILS_GENERICS_H_

#include <memory>

namespace reflex {

/// OpaqueType is a wrapper representing a type erased object
/// @note do not rely on underlying type being a shared pointer
using OpaqueType = std::shared_ptr<void>;

/// Interface for creating type erased object
template<class T>
class Generic final {
  public:
    Generic() = delete;

    /// Get the value of a type erased object
    /// @return the represented value of a type erased object
    static T Get(OpaqueType type) { return *std::static_pointer_cast<T>(type); }

    /// Create a type erased wrapper
    /// @return the represented value of a type erased object
    template<class ...Arg>
    static OpaqueType Create(Arg &&...args) {
        return std::make_shared<T>(std::forward<Arg>(args)...);
    }
};

}

#endif //REFLEX_SRC_AST_UTILS_GENERICS_H_
