#ifndef BASIC_BLOCK_H_
#define BASIC_BLOCK_H_

#include <cstddef>
#include <list>

#include "User.h"
#include "Instruction.h"

namespace IR {
    class  Function;
    struct IRContext;
    
    class BasicBlock final : public User {
        public:
            std::list <Instruction *> instructions = {};
    
            BasicBlock (char *name, Value *blockParent, size_t labelIndex);
            BasicBlock () = delete;
    
            char  *GetName       () const;
            size_t GetLength     () const;
            size_t GetLabelIndex () const;
    
            Instruction *InsertTail       (Instruction *newInstruction);
            Instruction *InsertAfterPoint (Instruction *newInstruction, Instruction *insertPoint);
    
            static BasicBlock *Create (char *name, Function *function, IRContext *context);
    
        private:
            char   *name         = nullptr;
            size_t  blockLength  = 0;
    
            size_t blockIndex = 0;//TODO change to name postfix?
    };
}
#endif
