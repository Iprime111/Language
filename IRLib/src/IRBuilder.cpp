#include <cassert>

#include "IRBuilder.h"
#include "Instruction.h"
#include "Value.h"

IRBuilder::IRBuilder (IRContext *context) : context (context) {}

IRContext *IRBuilder::GetContext () { return context; }

void IRBuilder::SetInsertPoint (BasicBlock  *newInsertPoint) { insertPoint = newInsertPoint; }
void IRBuilder::SetInsertPoint (Instruction *newInsertPoint) { insertPoint = newInsertPoint; }

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

Instruction *IRBuilder::CreateStoreInstruction (AllocaInstruction *variable, Value *operand) {
    return InsertInstruction (new StoreInstruction (variable, operand));
}

Instruction *IRBuilder::CreateLoadInstruction (AllocaInstruction *variable) {
    return InsertInstruction (new LoadInstruction (variable));
}

Instruction *IRBuilder::CreateAllocaInstruction (const Type *type) {
    
    Function *parentFunction = nullptr;
    
    ValueId insertPointType = insertPoint->GetValueId ();

    if (insertPointType == ValueId::INSTRUCTION)
        parentFunction = (Function *) insertPoint->parent->parent;
    else if (insertPointType == ValueId::BASIC_BLOCK)
        parentFunction = (Function *) insertPoint->parent;
    else
        return nullptr;

    //TODO alignment
    parentFunction->allocaSize += type->GetSize ();

    //TODO maybe save stack address before increment?
    return InsertInstruction (new AllocaInstruction (type, parentFunction->allocaSize));
}

Instruction *IRBuilder::InsertInstruction (Instruction *newInstruction) {
    if (!insertPoint)
        return nullptr;

    ValueId insertPointType = insertPoint->GetValueId ();
    
    Instruction *insertedInstruction = nullptr;

    if (insertPointType == ValueId::BASIC_BLOCK) {
        return ((BasicBlock *) insertPoint)->InsertTail (newInstruction);

    } else if (insertPointType == ValueId::INSTRUCTION) {
        BasicBlock *basicBlock = (BasicBlock *) insertPoint->parent;

        return basicBlock->InsertAfterPoint (newInstruction, (Instruction *) insertPoint);

    } else {
        return nullptr;
    }
}
