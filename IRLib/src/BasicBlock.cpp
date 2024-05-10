#include "BasicBlock.h"
#include "User.h"
#include "Function.h"
#include "Value.h"

BasicBlock::BasicBlock (char *name, Value *blockParent) : User (ValueId::BASIC_BLOCK, nullptr), name (name), blockLength (0), 
                                                          head (nullptr), tail (nullptr) {
    parent = blockParent; // TODO why don't you use initializer list for that?
}

BasicBlock::~BasicBlock () {
    Instruction *currentInstruction = head;

    while (currentInstruction) {
        delete currentInstruction;

        currentInstruction = currentInstruction->next;
    }
}

char  *BasicBlock::GetName   () const { return name; }
size_t BasicBlock::GetLength () const { return blockLength; }

Instruction *BasicBlock::GetHead () const { return head; }
Instruction *BasicBlock::GetTail () const { return tail; } 

Instruction *BasicBlock::InsertTail (Instruction *newInstruction) {
    if (!newInstruction)
        return nullptr;

    if (tail) {
        InsertAfterPoint (newInstruction, tail);

    } else {
        newInstruction->prev   = newInstruction->next = nullptr;
        newInstruction->parent = this;

        head  = newInstruction; 
    }

    tail = newInstruction;
    
    return newInstruction;
}

Instruction *BasicBlock::InsertAfterPoint (Instruction *newInstruction, Instruction *insertPoint) {
    if (!newInstruction || !insertPoint)
        return nullptr;

    newInstruction->parent = insertPoint->parent;

    newInstruction->next   = insertPoint->next;
    newInstruction->prev   = insertPoint;
    insertPoint->next      = newInstruction;

    return newInstruction;
}

BasicBlock *BasicBlock::Create (char *name, Function *function) {
    if (!name || !function)
        return nullptr;

    BasicBlock *newBlock = new BasicBlock (name, function);

    function->basicBlocks.push_back (newBlock); //TODO: It's better to use std::list rather than std::vector 
                                                // because in optimization pipeline `insert in the middle` operation is usual to meet

    return newBlock;
}

