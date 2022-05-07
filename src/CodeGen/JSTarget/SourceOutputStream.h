//
// Created by henry on 2022-05-07.
//

#ifndef REFLEX_SRC_CODEGEN_JSTARGET_SOURCEOUTPUTSTREAM_H_
#define REFLEX_SRC_CODEGEN_JSTARGET_SOURCEOUTPUTSTREAM_H_

#include <ostream>

namespace reflex {

class SourceOutputStream;

class ScopeIndent {
  public:
    explicit ScopeIndent(SourceOutputStream &gen);
    ~ScopeIndent();
  private:
    SourceOutputStream &gen;
};

class SourceOutputStream {
    friend class ScopeIndent;
  public:
    explicit SourceOutputStream(std::ostream &os) : os(os) {}

    ScopeIndent createScopeIndent() { return ScopeIndent(*this); }
    std::ostream &printIndent();
    std::ostream &stream() { return os; }
    explicit operator std::ostream &() { return os; }

  private:
    std::ostream &os;
    size_t indent = 0;
};

} // reflex

#endif //REFLEX_SRC_CODEGEN_JSTARGET_SOURCEOUTPUTSTREAM_H_
