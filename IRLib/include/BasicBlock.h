#ifndef BASIC_BLOCK_H_
#define BASIC_BLOCK_H_

#include <cstddef>

#include "Buffer.h"
#include "User.h"
#include "Instruction.h"

class Function;

class BasicBlock final : public User {
    public:
        Buffer <Instruction> instructions = {};

        BasicBlock () = delete;

        char  *GetName   ();
        size_t GetLength ();

        Instruction *InsertTail       (Instruction *newInstruction);
        Instruction *InsertAfterPoint (Instruction *newInstruction, Instruction *point);

        static BasicBlock *Create (char *name, Function *function);

    private:
        char   *name         = nullptr;
        size_t  blockLength  = 0;

        Instruction *tail = nullptr;

        BasicBlock (char *name);
        
};

#endif
