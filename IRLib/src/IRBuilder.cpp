#include <cassert>

#include "IRBuilder.h"
#include "BasicBlock.h"
#include "InstructionId.h"
#include "Type.h"
#include "Instruction.h"
#include "Value.h"

namespace IR {
    IRBuilder::IRBuilder (IRContext *context) : context (context) {}
    
    IRContext *IRBuilder::GetContext () { return context; }
    
    void IRBuilder::SetInsertPoint (BasicBlock  *newInsertPoint) { insertPoint = newInsertPoint;     }
    void IRBuilder::SetInsertPoint (Instruction *newInsertPoint) { insertPoint = newInsertPoint;     }
    void IRBuilder::SetEntryPoint  (Function    *entryPoint)     { context->entryPoint = entryPoint; }
    
    Value *IRBuilder::GetInsertPoint () const { return insertPoint; }
    
    BasicBlock *IRBuilder::GetInsertBlock () const {
        if (insertPoint->GetValueId () == ValueId::BASIC_BLOCK)
            return static_cast <BasicBlock *> (insertPoint);
        else
            return static_cast <BasicBlock *> (insertPoint->parent);
    }
    
    const IntegerType *IRBuilder::GetInt64Ty () const {return IR_TYPES_.GetInt64Ty (); }
    const IntegerType *IRBuilder::GetInt1Ty  () const {return IR_TYPES_.GetInt1Ty  (); }
    
    //TODO type checks
    Instruction *IRBuilder::CreateBinaryOperator (BinaryOperatorId id, Value *leftOperand, Value *rightOperand) {
        return InsertInstruction (new BinaryOperator (id, leftOperand, rightOperand));
    }
    
    Instruction *IRBuilder::CreateUnaryOperator (UnaryOperatorId id, Value *operand) {
        return InsertInstruction (new UnaryOperator (id, operand));
    }
    
    Instruction *IRBuilder::CreateStateChanger (StateChangerId id) {
        return InsertInstruction (new StateChanger (id));
    }
    
    Instruction *IRBuilder::CreateCmpOperator (CmpOperatorId id, Value *leftOperand, Value *rightOperand) {
        return InsertInstruction (new CmpOperator (id, leftOperand, rightOperand));
    }
    
    Instruction *IRBuilder::CreateReturnOperator (Value *operand) {
        return InsertInstruction (new ReturnOperator (operand));
    }

    Instruction *IRBuilder::CreateStoreInstruction (Value *variable, Value *operand) {
        ValueId variableId = variable->GetValueId ();

        if (variableId == ValueId::ARGUMENT || (variableId == ValueId::INSTRUCTION && 
            static_cast <Instruction *> (variable)->GetInstructionId () == InstructionId::ALLOCA_INSTRUCTION))
                return InsertInstruction (new StoreInstruction (variable, operand));

        return nullptr;
    }

    Instruction *IRBuilder::CreateLoadInstruction (Value *variable) {
        ValueId variableId = variable->GetValueId ();

        if (variableId == ValueId::ARGUMENT || (variableId == ValueId::INSTRUCTION && 
            static_cast <Instruction *> (variable)->GetInstructionId () == InstructionId::ALLOCA_INSTRUCTION))
                return InsertInstruction (new LoadInstruction (variable));

        return nullptr;
    }
    
    Instruction *IRBuilder::CreateAllocaInstruction (const Type *type) {
        
        Function *parentFunction = nullptr;
        
        ValueId insertPointType = insertPoint->GetValueId ();
    
        if (insertPointType == ValueId::INSTRUCTION)
            parentFunction = static_cast <Function *> (insertPoint->parent->parent);
        else if (insertPointType == ValueId::BASIC_BLOCK)
            parentFunction = static_cast <Function *> (insertPoint->parent);
        else
            return nullptr;
    
        //TODO alignment
        parentFunction->allocaSize += type->GetSize ();
    
        return InsertInstruction (new AllocaInstruction (type, parentFunction->allocaSize));
    }
    
    Instruction *IRBuilder::CreateBranchInstruction (Value *condition, BasicBlock *ifTrue, BasicBlock *ifFalse) {
        return InsertInstruction (new BranchInstruction (condition, ifTrue, ifFalse));
    }
    
    Instruction *IRBuilder::CreateBranchInstruction (BasicBlock *nextBlock) {
        return InsertInstruction (new BranchInstruction (nextBlock));
    }
    
    Instruction *IRBuilder::CreateCall (Function *calleeFunction, std::vector <Value *> *arguments) {

        if (calleeFunction->GetArgs ().size () == arguments->size ())
            return InsertInstruction (new CallInstruction (calleeFunction, arguments));
        
        return nullptr;
    }

    Instruction *IRBuilder::CreateOutInstruction (Value *outExpression) {
        return InsertInstruction (new OutInstruction (outExpression));
    }

    Instruction *IRBuilder::CreateInInstruction () {
        return InsertInstruction (new InInstruction ());
    }
    
    Instruction *IRBuilder::InsertInstruction (Instruction *newInstruction) {
        if (!insertPoint)
            return nullptr;
    
        ValueId insertPointType = insertPoint->GetValueId ();
        
        Instruction *insertedInstruction = nullptr;
    
        if (insertPointType == ValueId::BASIC_BLOCK) {
            return static_cast <BasicBlock *> (insertPoint)->InsertTail (newInstruction);
    
        } else if (insertPointType == ValueId::INSTRUCTION) {
            BasicBlock *basicBlock = static_cast <BasicBlock *> (insertPoint->parent);
    
            return basicBlock->InsertAfterPoint (newInstruction, static_cast <Instruction *>(insertPoint));
    
        } else {
            return nullptr;
        }
    }
}
