#ifndef IR_BUILDER_H_
#define IR_BUILDER_H_

#include "BasicBlock.h"
#include "FunctionType.h"
#include "IRContext.h"
#include "Instruction.h"
#include "Value.h"

namespace IR {
    class IRBuilder final {
        public:
            IRBuilder (IRContext *context);
    
            IRContext *GetContext ();
    
            void SetInsertPoint (BasicBlock  *insertPoint);
            void SetInsertPoint (Instruction *insertPoint);
    
            Value      *GetInsertPoint () const;
            BasicBlock *GetInsertBlock () const;
    
            const IntegerType *GetInt64Ty () const;
            const IntegerType *GetInt1Ty  () const;
    
            Instruction *CreateCmpOperator       (CmpOperatorId    id, Value *leftOperand, Value *rightOperand);
            Instruction *CreateBinaryOperator    (BinaryOperatorId id, Value *leftOperand, Value *rightOperand);
            Instruction *CreateUnaryOperator     (UnaryOperatorId  id, Value *operand);
            Instruction *CreateStateChanger      (StateChangerId   id);
            Instruction *CreateReturnOperator    (Value *operand);
            Instruction *CreateStoreInstruction  (AllocaInstruction *variable, Value *operand);
            Instruction *CreateLoadInstruction   (AllocaInstruction *variable);
            Instruction *CreateAllocaInstruction (const Type *type);
            Instruction *CreateBranchInstruction (Value *condition, BasicBlock *ifTrue, BasicBlock *ifFalse);
            Instruction *CreateBranchInstruction (BasicBlock *nextBlock);
            Instruction *CreateTruncCast         (Value *castValue, const IntegerType *targetType);
            Instruction *CreateCall              (Function *calleeFunction, std::vector <Value *> *arguments);
    
        private:
            Instruction *InsertInstruction (Instruction *newInstruction);
    
            IRContext *context     = nullptr;
            Value     *insertPoint = nullptr;
    };
}
#endif 
