#include "Instruction.h"
#include "BasicBlock.h"
#include "FunctionType.h"
#include "InstructionId.h"
#include "User.h"
#include "Value.h"
#include "Function.h"

namespace IR {
    Instruction::Instruction (InstructionId instructionId, const Type *instructionType) : 
        User (ValueId::INSTRUCTION, instructionType), instructionId (instructionId) {}
    Instruction::Instruction (InstructionId instructionId, const Type *instructionType, size_t operandsCount) : 
        User (ValueId::INSTRUCTION, instructionType, operandsCount), instructionId (instructionId) {}
    
    InstructionId Instruction::GetInstructionId () const { return instructionId; }
    
    StateChanger::StateChanger (StateChangerId id) : Instruction (InstructionId::STATE_CHANGER, nullptr, 0), id (id) {}
    
    StateChangerId StateChanger::GetStateChangerId () const { return id; }
    
    UnaryOperator::UnaryOperator (UnaryOperatorId id, Value *operand) : 
        Instruction (InstructionId::UNARY_OPERATOR, operand->GetType (), 1), id (id) {
    
        operands [0] = Use (operand);
    }
    
    UnaryOperatorId UnaryOperator::GetUnaryOperatorId () const { return id; }
    
    BinaryOperator::BinaryOperator (BinaryOperatorId id, Value *firstOperand, Value *secondOperand) : 
        Instruction (InstructionId::BINARY_OPERATOR, firstOperand->GetType (), 2), id (id) {
    
        operands [0] = Use (firstOperand);
        operands [1] = Use (secondOperand);
    }
    
    BinaryOperatorId BinaryOperator::GetBinaryOperatorId () const { return id; }
    
    ReturnOperator::ReturnOperator (Value *operand) : Instruction (InstructionId::RETURN_OPERATOR, operand->GetType (), 1) {
        operands [0] = Use (operand);
    }
    
    CmpOperator::CmpOperator (CmpOperatorId id, Value *firstOperand, Value *secondOperand) :
        Instruction (InstructionId::CMP_OPERATOR, IR_TYPES_.GetInt1Ty (), 2), id (id) {
        
        operands [0] = Use (firstOperand);
        operands [1] = Use (secondOperand);
    }
    
    CmpOperatorId CmpOperator::GetCmpOperatorId () const { return id; }
    
    AllocaInstruction::AllocaInstruction (const Type *type, size_t stackAddress) : 
        Instruction (InstructionId::ALLOCA_INSTRUCTION, type), stackAddress (stackAddress) {}
    
    size_t AllocaInstruction::GetStackAddress () const { return stackAddress; }
    
    StoreInstruction::StoreInstruction (AllocaInstruction *variable, Value *operand) : 
        Instruction (InstructionId::STORE_INSTRUCTION, variable->GetType (), 2) {
        
        operands [0] = Use (variable);
        operands [1] = Use (operand);
    }
    
    LoadInstruction::LoadInstruction (AllocaInstruction *variable) : 
        Instruction (InstructionId::LOAD_INSTRUCTION, variable->GetType (), 1) {
    
        operands [0] = Use (variable);
    }
    
    BranchInstruction::BranchInstruction (BasicBlock *nextBlock) : 
        Instruction (InstructionId::BRANCH_INSTRUCTION, nullptr, 1), isConditional (false) {
        
        operands [0] = Use (nextBlock);
    }
    
    //TODO no type?
    BranchInstruction::BranchInstruction (Value *condition, BasicBlock *ifTrue, BasicBlock *ifFalse) : 
        Instruction (InstructionId::BRANCH_INSTRUCTION, nullptr, 3), isConditional (true) {
    
        operands [0] = Use (condition);
        operands [1] = Use (ifTrue);
        operands [2] = Use (ifFalse);
    }
    
    bool BranchInstruction::IsConditional () { return isConditional; }
    
    CastInstruction::CastInstruction (Value *castValue, const Type *targetType, CastId castId) : 
        Instruction (InstructionId::CAST_INSTRUCTION, targetType, 1), id (castId) {
        
        operands [0] = Use (castValue);
    }
    
    TruncCast::TruncCast (Value *castValue, const IntegerType *targetType) : 
        CastInstruction (castValue, targetType, CastId::TRUNC) {}

    CallInstruction::CallInstruction (Function *calleeFunction, std::vector <Value *> *arguments) : 
        Instruction (InstructionId::CALL_INSTRUCTION, calleeFunction->GetType (), calleeFunction->GetFunctionType()->params.size () + 1) {
    
        //TODO error logging
        if (calleeFunction->GetFunctionType ()->params.size () != arguments->size ())
            return;

        operands [0] = calleeFunction;

        for (size_t argumentIndex = 0; argumentIndex < arguments->size (); argumentIndex++)
            operands [argumentIndex + 1] = (*arguments) [argumentIndex];
    }
}
