#ifndef IR_BUILDER_H_
#define IR_BUILDER_H_

#include "BackendCore.h"
#include "Ir.h"

class IRBuilder final {
    public:
        TranslationContext *context = nullptr;

        void SetInsertPoint (BasicBlock  *insertPoint);
        void SetInsertPoint (Instruction *insertPoint);

        Instruction *CreateAdd ();
        Instruction *CreateSub ();
        Instruction *CreateMul ();
        Instruction *CreateDiv ();

    private:
        Instruction *CreateBinaryOperation (Instruction newInstruction);

        Value *insertPoint = nullptr;
};




#endif 
