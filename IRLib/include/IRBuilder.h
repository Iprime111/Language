#ifndef IR_BUILDER_H_
#define IR_BUILDER_H_

#include "IRContext.h"
#include "Instruction.h"
#include "Value.h"

class IRBuilder final {
    public:
        IRBuilder (IRContext *context);

        IRContext *GetContext ();

        void SetInsertPoint (BasicBlock  *insertPoint);
        void SetInsertPoint (Instruction *insertPoint);

        Instruction *CreateCmpOperator           (CmpOperatorId    id, Value *leftOperand, Value *rightOperand);
        Instruction *CreateBinaryOperator        (BinaryOperatorId id, Value *leftOperand, Value *rightOperand);
        Instruction *CreateUnaryOperator         (UnaryOperatorId  id, Value *operand);
        Instruction *CreateStateChanger          (StateChangerId   id);
        Instruction *CreateReturnOperator        (Value *operand);
        Instruction *CreateStoreInstruction      (AllocaInstruction *variable, Value *operand);
        Instruction *CreateLoadInstruction       (AllocaInstruction *variable);
        Instruction *CreateAllocaInstruction     (const Type *type);

    private:
        Instruction *InsertInstruction (Instruction *newInstruction);

        IRContext *context     = nullptr;
        Value     *insertPoint = nullptr;
};

#endif 
