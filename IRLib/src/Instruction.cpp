#include "Instruction.h"
#include "BasicBlock.h"
#include "Type.h"
#include "InstructionId.h"
#include "User.h"
#include "Value.h"
#include "Function.h"

namespace IR {
    //--------------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------Instruction---------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------------------

    Instruction::Instruction (InstructionId instructionId, const Type *instructionType) : 
        User (ValueId::INSTRUCTION, instructionType), instructionId (instructionId) {}
    Instruction::Instruction (InstructionId instructionId, const Type *instructionType, size_t operandsCount) : 
        User (ValueId::INSTRUCTION, instructionType, operandsCount), instructionId (instructionId) {}
    
    InstructionId Instruction::GetInstructionId () const { return instructionId; }
    
    //--------------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------StateChanger--------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------------------

    StateChanger::StateChanger (StateChangerId id) : Instruction (InstructionId::STATE_CHANGER, nullptr, 0), id (id) {}
    
    StateChangerId StateChanger::GetStateChangerId () const { return id; }
    
    //--------------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------UnaryOperator-------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------------------

    UnaryOperator::UnaryOperator (UnaryOperatorId id, Value *operand) : 
        Instruction (InstructionId::UNARY_OPERATOR, operand->GetType (), 1), id (id) {
    
        operands [0] = Use (operand);
    }
    
    UnaryOperatorId UnaryOperator::GetUnaryOperatorId () const { return id; }
    
    //--------------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------BinaryOperator------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------------------

    BinaryOperator::BinaryOperator (BinaryOperatorId id, Value *firstOperand, Value *secondOperand) : 
        Instruction (InstructionId::BINARY_OPERATOR, firstOperand->GetType (), 2), id (id) {
    
        operands [0] = Use (firstOperand);
        operands [1] = Use (secondOperand);
    }
    
    BinaryOperatorId BinaryOperator::GetBinaryOperatorId () const { return id; }
    
    //--------------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------ReturnOperator------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------------------
    
    ReturnOperator::ReturnOperator (Value *operand) : Instruction (InstructionId::RETURN_OPERATOR, operand->GetType (), 1) {
        operands [0] = Use (operand);
    }
    
    //--------------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------CmpOperator---------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------------------
    
    CmpOperator::CmpOperator (CmpOperatorId id, Value *firstOperand, Value *secondOperand) :
        Instruction (InstructionId::CMP_OPERATOR, IR_TYPES_.GetInt1Ty (), 2), id (id) {
        
        operands [0] = Use (firstOperand);
        operands [1] = Use (secondOperand);
    }
    
    CmpOperatorId CmpOperator::GetCmpOperatorId () const { return id; }
    
    //--------------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------AllocaInstruction---------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------------------
    
    AllocaInstruction::AllocaInstruction (const Type *type, size_t stackAddress, const char *name) : 
        Instruction (InstructionId::ALLOCA_INSTRUCTION, type), stackAddress (stackAddress), name (name) {}
    
    const std::string &AllocaInstruction::GetName         () const { return name; }
    size_t             AllocaInstruction::GetStackAddress () const { return stackAddress; }
    
    //--------------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------StoreInstruction----------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------------------
    StoreInstruction::StoreInstruction (Value *variable, Value *operand) : 
        Instruction (InstructionId::STORE_INSTRUCTION, variable->GetType (), 2) {
        
        operands [0] = Use (variable);
        operands [1] = Use (operand);
    }
    
    //--------------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------LoadInstruction-----------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------------------
    
    LoadInstruction::LoadInstruction (Value *variable) : 
        Instruction (InstructionId::LOAD_INSTRUCTION, variable->GetType (), 1) {
    
        operands [0] = Use (variable);
    }
    
    //--------------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------BranchInstruction---------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------------------

    BranchInstruction::BranchInstruction (BasicBlock *nextBlock) : 
        Instruction (InstructionId::BRANCH_INSTRUCTION, nullptr, 1), isConditional (false) {
        
        operands [0] = Use (nextBlock);
    }
    
    BranchInstruction::BranchInstruction (Value *condition, BasicBlock *ifTrue, BasicBlock *ifFalse) : 
        Instruction (InstructionId::BRANCH_INSTRUCTION, nullptr, 3), isConditional (true) {
    
        operands [0] = Use (condition);
        operands [1] = Use (ifTrue);
        operands [2] = Use (ifFalse);
    }
    
    void BranchInstruction::SetTrueBlock (BasicBlock *block) { 
        if (!isConditional)
            return;

        operands [1] = Use (block);
    }

    void BranchInstruction::SetFalseBlock (BasicBlock *block){
        if (!isConditional)
            return;

        operands [2] = Use (block);
    }
    
    bool BranchInstruction::IsConditional () const { return isConditional; }
    
    //--------------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------CastInstruction-----------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------------------
    
    CastInstruction::CastInstruction (Value *castValue, const Type *targetType, CastId castId) : 
        Instruction (InstructionId::CAST_INSTRUCTION, targetType, 1), id (castId) {
        
        operands [0] = Use (castValue);
    }
    
    //--------------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------CallInstruction-----------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------------------
    
    CallInstruction::CallInstruction (Function *calleeFunction, std::vector <Value *> *arguments) : 
        Instruction (InstructionId::CALL_INSTRUCTION, calleeFunction->GetType (), calleeFunction->GetArgs ().size () + 1) {
    
        //TODO error logging
        if (calleeFunction->GetArgs ().size () != arguments->size ())
            return;

        operands [0] = Use (calleeFunction);

        for (size_t argumentIndex = 0; argumentIndex < arguments->size (); argumentIndex++)
            operands [argumentIndex + 1] = Use ((*arguments) [argumentIndex]);
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------InInstruction-------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------------------

    InInstruction::InInstruction () : Instruction (InstructionId::IN_INSTRUCTION, IR_TYPES_.GetInt64Ty ()){}

    //--------------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------OutInstruction------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------------------

    OutInstruction::OutInstruction (Value *outValue) : 
        Instruction (InstructionId::OUT_INSTRUCTION, nullptr, 1) {

        operands [0] = Use (outValue);
    }
}
