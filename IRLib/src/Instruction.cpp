#include "Instruction.h"
#include "User.h"

Instruction::Instruction (InstructionId instructionId, const Type *instructionType) : User (ValueId::INSTRUCTION, instructionType), instructionId (instructionId) {}
Instruction::Instruction (InstructionId instructionId, const Type *instructionType, size_t operandsCount) : 
    User (ValueId::INSTRUCTION, instructionType, operandsCount), instructionId (instructionId) {}

InstructionId Instruction::GetInstructionId () const { return instructionId; }

StateChanger::StateChanger (StateChangerId id) : Instruction (InstructionId::STATE_CHANGER, 0), id (id) {}

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
    Instruction (InstructionId::CMP_OPERATOR, firstOperand->GetType (), 2), id (id) {
    
    operands [0] = Use (firstOperand);
    operands [1] = Use (secondOperand);
}

CmpOperatorId CmpOperator::GetCmpOperatorId () const { return id; }

AllocaInstruction::AllocaInstruction (const Type *type, size_t stackAddress) : 
    Instruction (InstructionId::ALLOCATION_INSTRUCTION, type), stackAddress (stackAddress) {}

size_t AllocaInstruction::GetStackAddress () const { return stackAddress; }

StoreInstruction::StoreInstruction (AllocaInstruction *variable, Value *operand) : 
    Instruction (InstructionId::STORE_INSTRUCTION, variable->GetType (), 2) {
    
    operands [0] = Use (variable);
    operands [1] = Use (operand);
}

LoadInstruction::LoadInstruction (AllocaInstruction *variable) : Instruction (InstructionId::LOAD_INSTRUCTION, variable->GetType (), 1) {
    operands [0] = Use (variable);
}

