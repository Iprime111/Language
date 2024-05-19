#ifndef IR_CONTEXT_H_
#define IR_CONTEXT_H_

#include <unordered_map>
#include <vector>

#include "Constant.h"
#include "Function.h"
#include "Type.h"
#include "Value.h"

namespace IR {
    struct IRContext final {
        std::list   <Function *>     functions = {};
        std::vector <ConstantData *> constants = {};

        std::unordered_map <std::string, size_t> blockNames = {};
    
        Function *entryPoint = nullptr;

        IRContext  () = default;
        ~IRContext ();
    };
}
#endif
