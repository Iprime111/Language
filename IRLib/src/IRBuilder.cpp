#include <cassert>

#include "IRBuilder.h"
#include "Value.h"

IRBuilder::IRBuilder (IRContext *context) : context (context) {}

IRContext *IRBuilder::GetContext () { return context; }

IRContext *GetContext ();

Instruction *IRBuilder::CreateInstruction (InstructionId id) {
    Instruction newInstruction = Instruction (id);

    return InsertInstruction (&newInstruction);
}

Instruction *IRBuilder::InsertInstruction (Instruction *newInstruction) {
    if (!insertPoint)
        return nullptr;

    ValueType insertPointType = insertPoint->GetType ();
    
    Instruction *insertedInstruction = nullptr;

    if (insertPointType == ValueType::BASIC_BLOCK) {
        return ((BasicBlock *) insertPoint)->InsertTail (newInstruction);

    } else if (insertPointType == ValueType::INSTRUCTION) {
        BasicBlock *basicBlock = (BasicBlock *) insertPoint->parent;

        return basicBlock->InsertAfterPoint (newInstruction, (Instruction *) insertPoint);

    } else {
        return nullptr;
    }
}
