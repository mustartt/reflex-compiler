//
// Created by henry on 2022-05-07.
//

#include "SourceOutputStream.h"

namespace reflex {

ScopeIndent::ScopeIndent(SourceOutputStream &gen) : gen(gen) {
    ++gen.indent;
}

ScopeIndent::~ScopeIndent() {
    --gen.indent;
}

std::ostream &SourceOutputStream::printIndent() {
    for (size_t i = 0; i < indent; ++i) os << "  ";
    return os;
}

} // reflex
