//
// Created by henry on 2022-05-03.
//

#ifndef REFLEX_SRC_CODEGEN_CODEGENTARGET_H_
#define REFLEX_SRC_CODEGEN_CODEGENTARGET_H_

namespace reflex {

class CodeGenTarget {
  public:
    virtual ~CodeGenTarget() = default;
    virtual void CodeGen() = 0;
};

}

#endif //REFLEX_SRC_CODEGEN_CODEGENTARGET_H_
