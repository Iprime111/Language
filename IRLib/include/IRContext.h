#ifndef IR_CONTEXT_H_
#define IR_CONTEXT_H_

#include <vector>

#include "Constant.h"
#include "Function.h"
#include "FunctionType.h"
#include "Value.h"

struct IRContext final {
    // TODO there's no need in `= {}` initialisation. Espesially, in case of std::vector
    std::vector <Function *>     functions = {};
    std::vector <ConstantData *> constants = {};
    TypesImplementation          types     = {};

    IRContext  () = default;
    ~IRContext ();
};

#endif
