#ifndef BASIC_BLOCK_H_
#define BASIC_BLOCK_H_

#include <cstddef>
#include <list>
#include <string>

#include "User.h"
#include "Instruction.h"

namespace IR {
    class  Function;
    struct IRContext;
    
    class BasicBlock final : public User {
        public:
            std::list <Instruction *> instructions = {};
    
            BasicBlock  () = delete;
            ~BasicBlock ();
    
            const std::string &GetName   () const;
            size_t       GetLength () const;
    
            Instruction *InsertTail       (Instruction *newInstruction);
            Instruction *InsertAfterPoint (Instruction *newInstruction, Instruction *insertPoint);
    
            static BasicBlock *Create (const char *name, Function *function, IRContext *context);
    
        private:
            std::string name;
            size_t      blockLength;
    
            explicit BasicBlock (std::string &name, Value *blockParent);
    };
}
#endif
