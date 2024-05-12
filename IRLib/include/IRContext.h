#ifndef IR_CONTEXT_H_
#define IR_CONTEXT_H_

#include <vector>

#include "Constant.h"
#include "Function.h"
#include "FunctionType.h"
#include "Value.h"

namespace IR {
    struct IRContext final {
        std::list   <Function *>     functions;
        std::vector <ConstantData *> constants;
    
        size_t currentLabelIndex = 0;
    
        IRContext  () = default;
        ~IRContext ();
    };
}
#endif
