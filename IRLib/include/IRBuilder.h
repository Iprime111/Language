#ifndef IR_BUILDER_H_
#define IR_BUILDER_H_

#include "IRContext.h"
#include "Value.h"

class IRBuilder final {
    public:
        IRBuilder (IRContext *context);

        IRContext *GetContext ();

        void SetInsertPoint (BasicBlock  *insertPoint);
        void SetInsertPoint (Instruction *insertPoint);

        Instruction *CreateInstruction (InstructionId id);

    private:
        Instruction *InsertInstruction (Instruction *newInstruction);

        IRContext *context     = nullptr;
        Value     *insertPoint = nullptr;
};




#endif 
