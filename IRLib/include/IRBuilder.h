#ifndef IR_BUILDER_H_
#define IR_BUILDER_H_

#include "BasicBlock.h"
#include "Type.h"
#include "IRContext.h"
#include "Instruction.h"
#include "Value.h"

namespace IR {
    class IRBuilder final {
        public:
            explicit IRBuilder (IRContext *context);
    
            IRContext *GetContext ();
    
            void SetInsertPoint (BasicBlock  *insertPoint);
            void SetInsertPoint (Instruction *insertPoint);
            void SetEntryPoint  (Function    *entryPoint);
    
            Value      *GetInsertPoint () const;
            BasicBlock *GetInsertBlock () const;
    
            const IntegerType *GetInt64Ty  () const;
            const IntegerType *GetInt1Ty   () const;
            const FloatType   *GetDoubleTy () const;
    
            Instruction *CreateCmpOperator       (CmpOperatorId    id, Value *leftOperand, Value *rightOperand);
            Instruction *CreateBinaryOperator    (BinaryOperatorId id, Value *leftOperand, Value *rightOperand);
            Instruction *CreateUnaryOperator     (UnaryOperatorId  id, Value *operand);
            Instruction *CreateStateChanger      (StateChangerId   id);
            Instruction *CreateReturnOperator    (Value *operand);
            Instruction *CreateStoreInstruction  (Value *variable, Value *operand);
            Instruction *CreateLoadInstruction   (Value *variable);
            Instruction *CreateAllocaInstruction (const Type *type, const char *name);
            Instruction *CreateBranchInstruction (Value *condition, BasicBlock *ifTrue, BasicBlock *ifFalse);
            Instruction *CreateBranchInstruction (BasicBlock *nextBlock);
            Instruction *CreateCall              (Function *calleeFunction, std::vector <Value *> *arguments);
            Instruction *CreateOutInstruction    (Value *outExpression);
            Instruction *CreateInInstruction     ();
    
        private:
            Instruction *InsertInstruction (Instruction *newInstruction);
    
            IRContext *context     = nullptr;
            Value     *insertPoint = nullptr;
    };
}
#endif 
