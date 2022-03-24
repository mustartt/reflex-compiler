#include <iostream>
#include <fstream>

#include "Type.h"
#include "TypeContextManager.h"

int main(int argc, char *argv[]) {
    using namespace reflex;

    TypeContextManager ctx;
    auto f1 = ctx.getFunctionTy(ctx.getPrimitiveTy(PrimType::Integer),
                                {ctx.getPrimitiveTy(PrimType::Integer)});
    auto c1 = ctx.createClassTy(nullptr, {});
    c1->addMember("method1", ctx.createMemberTy(c1, Visibility::Public, f1));

    ctx.dump(std::cout);
    return 0;
}
