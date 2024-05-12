#ifndef FUNCTION_H_
#define FUNCTION_H_

#include <list>

#include "BasicBlock.h"
#include "FunctionType.h"
#include "Value.h"

namespace IR {
    class IRBuilder;
    
    //TODO maybe derive from the Constant class
    class Function final : public Value {
        friend class IRBuilder;
    
        public:
            std::list <BasicBlock *> basicBlocks = {};
    
            Function  () = delete;
            ~Function ();
    
            const char         *GetName         () const;
            const FunctionType *GetFunctionType () const;
    
            size_t GetAllocaSize () const;
    
            static Function *Create (IRContext *context, FunctionType *type, const char *name);
    
        private:
            const char   *name;
            FunctionType  functionType;
    
            size_t allocaSize  = 0;
    
            Function (const char *name, FunctionType *type);
    };
}
#endif
