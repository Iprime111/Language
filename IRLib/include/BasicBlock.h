#ifndef BASIC_BLOCK_H_
#define BASIC_BLOCK_H_

#include <cstddef>

#include "User.h"
#include "Instruction.h"

class Function;

class BasicBlock final : public User {
    public:
        //TODO make this private and use push_back + copy constructor instead of emplace?
        BasicBlock (char *name, Value *blockParent);
        BasicBlock () = delete;
        ~BasicBlock ();

        char  *GetName   () const;
        size_t GetLength () const;

        Instruction *GetHead () const;
        Instruction *GetTail () const;

        Instruction *InsertTail       (Instruction *newInstruction);
        Instruction *InsertAfterPoint (Instruction *newInstruction, Instruction *insertPoint);

        static BasicBlock *Create (char *name, Function *function);

    private:
        char   *name         = nullptr;
        size_t  blockLength  = 0;

        Instruction *tail = nullptr;
        Instruction *head = nullptr;
};

#endif
