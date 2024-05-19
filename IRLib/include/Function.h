#ifndef FUNCTION_H_
#define FUNCTION_H_

#include <list>
#include <vector>

#include "Argument.h"
#include "BasicBlock.h"
#include "Type.h"
#include "Value.h"

namespace IR {
    class IRBuilder;
    
    //TODO move/copy constructors
    //TODO maybe derive from the Constant class
    class Function final : public Value {
        friend class IRBuilder;
    
        public:
            std::list <BasicBlock *> basicBlocks = {};
    
            Function  () = delete;
            ~Function ();
    
            const std::string              &GetName () const;
            const std::vector <Argument *> &GetArgs () const;
    
            size_t GetAllocaSize () const;
    
            static Function *Create (IRContext *context, const Type *type, std::vector <const Type *> *argumentTypes, const char *name);
    
        private:
            std::string              name;
            std::vector <Argument *> args;

            size_t allocaSize  = 0;
    
            explicit Function (const char *name, const Type *type, std::vector <Argument *> &&arguments);
    };
}
#endif
