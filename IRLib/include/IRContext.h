#ifndef IR_CONTEXT_H_
#define IR_CONTEXT_H_

#include "Buffer.h"
#include "Constant.h"
#include "Function.h"
#include "FunctionType.h"
#include "Value.h"

struct IRContext final {
    Buffer <Function>       functions = {};
    Buffer <ConstantData>   constants = {};
    TypesImplementation     types     = {};

    IRContext  ();
    ~IRContext ();
};

#endif
