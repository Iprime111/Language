#include "BasicBlock.h"
#include "User.h"
#include "Function.h"

BasicBlock::BasicBlock (char *name) : User (ValueType::BASIC_BLOCK), instructions ({}), name (name), blockLength (0) {
    InitBuffer (&instructions);
}

char  *BasicBlock::GetName   () { return name; }
size_t BasicBlock::GetLength () { return blockLength; }


Instruction *BasicBlock::InsertTail (Instruction *newInstruction) {
    if (!newInstruction)
        return nullptr;

    Instruction *insertedInstruction = InsertAfterPoint (newInstruction, tail);

    tail = insertedInstruction;
    
    return tail;
}

Instruction *BasicBlock::InsertAfterPoint (Instruction *newInstruction, Instruction *insertPoint) {
    if (!newInstruction || !insertPoint)
        return nullptr;

    if (WriteDataToBuffer (&instructions, newInstruction, 1) != BufferErrorCode::NO_BUFFER_ERRORS)
        return nullptr;

    Instruction *insertedInstruction = &instructions.data [instructions.currentIndex - 1];

    insertedInstruction->parent = insertPoint->parent;

    insertedInstruction->next   = insertPoint->next;
    insertedInstruction->prev   = insertPoint;
    insertPoint->next           = insertedInstruction;

    return insertedInstruction;
}

BasicBlock *BasicBlock::Create (char *name, Function *function) {
    if (!name || !function)
        return nullptr;

    BasicBlock newBlock (name);

    if (WriteDataToBuffer (&function->basicBlocks, &newBlock, 1) != BufferErrorCode::NO_BUFFER_ERRORS)
        return nullptr;

    return &function->basicBlocks.data [function->basicBlocks.currentIndex - 1];
}

