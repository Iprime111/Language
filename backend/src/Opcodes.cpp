#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include "Opcodes.h"
#include "Argument.h"
#include "Constant.h"
#include "Function.h"
#include "IRContext.h"
#include "Instruction.h"
#include "InstructionId.h"
#include "MachineOpcodes.h"
#include "Value.h"

static const std::string INTEGER_DATA_REGISTERS [] = {"rax", "rbx"};
static const size_t      REQUIRED_STACK_ALIGNMENT  = 16;

x86Opcodes::x86Opcodes (IR::IRContext *irContext) : MachineOpcodes (irContext) {}

IR::Opcode *x86Opcodes::ProcessProgramEnter () {
    
    return CreateOpcode ("%include \"../build/bin/Resources/stdlib.asm\"\n"
                         "section .text\n"
                         "global _start\n\n"
                         "_start:\n"
                         "\tcall " + context->entryPoint->GetName () + "\n\n"
                         "\tmov rax, 0x3c ; exit\n"
                         "\tmov rsi, 0x00 ; exit code\n"
                         "\tsyscall\n");
}

IR::Opcode *x86Opcodes::ProcessFunctionEnter (IR::Function *function) {
    assert (function);

    size_t allocaSize = function->GetAllocaSize ();
    size_t stackFrameSize = allocaSize + (REQUIRED_STACK_ALIGNMENT - allocaSize % REQUIRED_STACK_ALIGNMENT);

    return CreateOpcode ("\n" + std::string (function->GetName ()) + ":\n" +
                         "\tpush rbp\n"
                         "\tmov rbp, rsp\n"
                         "\tsub rsp, " + std::to_string (stackFrameSize) + "\n");
}

IR::Opcode *x86Opcodes::ProcessBlockEnter (IR::BasicBlock *basicBlock) {
    assert (basicBlock);

    return CreateOpcode ("\n" + basicBlock->GetName () + ":\n");
}

IR::Opcode *x86Opcodes::ProcessStateChanger (IR::Instruction *instruction) {
    assert (instruction);

    //WARNING works only for hlt
    return CreateOpcode ("\n\tmov rax, 0x3c\n"
                           "\tmov rsi, 0x00\n"
                           "\tsyscall\n");
}

IR::Opcode *x86Opcodes::ProcessUnaryOperator (IR::Instruction *instruction) {
    assert (instruction);

    IR::UnaryOperator *unaryOperator = (IR::UnaryOperator *) instruction;
    
    switch (unaryOperator->GetUnaryOperatorId ()) {

        //TODO
        case IR::UnaryOperatorId::SIN:
        case IR::UnaryOperatorId::COS:
            //TODO fuck...
            break;

        case IR::UnaryOperatorId::FLOOR:
        case IR::UnaryOperatorId::SQRT:
            //TODO double type
            break;
    }

    return nullptr;
}

IR::Opcode *x86Opcodes::ProcessBinaryOperator (IR::Instruction *instruction) {
    IR::BinaryOperator *binaryOperator = (IR::BinaryOperator *) instruction;
    
    IR::Opcode *opcode = CreateOpcode ("\n");


    PrintOperandLoad (instruction, 1, opcode, 1);
    PrintOperandLoad (instruction, 0, opcode, 0);
    
    switch (binaryOperator->GetBinaryOperatorId ()) {

        case IR::BinaryOperatorId::ADD:
            opcode->opcodeContent += "\tadd rax, rbx\n";
            break;

        case IR::BinaryOperatorId::SUB:
            opcode->opcodeContent += "\tsub rax, rbx\n";
            break;

        case IR::BinaryOperatorId::MUL:
            opcode->opcodeContent += "\timul rax, rbx\n";
            break;

        case IR::BinaryOperatorId::DIV:
            opcode->opcodeContent += "\tcqo\n"
                                     "\tidiv rbx\n";
            break;
    }

    opcode->opcodeContent += "\tpush rax\n";

    return opcode;
}

IR::Opcode *x86Opcodes::ProcessReturnOperator (IR::Instruction *instruction) {
    assert (instruction);

    IR::Opcode *opcode = CreateOpcode ();
    PrintOperandLoad (instruction, 0, opcode, 0);

    opcode->opcodeContent += "\n\tleave\n"
                               "\tret\n";

    return opcode;
}

IR::Opcode *x86Opcodes::ProcessCmpOperator (IR::Instruction *instruction) {
    IR::Opcode *opcode = CreateOpcode (); 

    PrintOperandLoad (instruction, 1, opcode, 1);
    PrintOperandLoad (instruction, 0, opcode, 0);

    opcode->opcodeContent += "\n\tcmp rax, rbx\n";

    return opcode;
}

IR::Opcode *x86Opcodes::ProcessAllocaInstruction (IR::Instruction *instruction) {
    return nullptr;
}

IR::Opcode *x86Opcodes::ProcessStoreInstruction (IR::Instruction *instruction) {
    assert (instruction);

    const IR::Value *operand        = instruction->GetOperand (0); 
    IR::ValueId      operandValueId = operand->GetValueId ();


    if (operandValueId == IR::ValueId::ARGUMENT) {
        const IR::Argument *functionArgument = static_cast <const IR::Argument *> (operand);
        size_t              argumentAddress = functionArgument->GetType ()->GetSize () * (functionArgument->GetIndex () + 2);

        IR::Opcode *opcode = CreateOpcode ("\n");
        PrintOperandLoad (instruction, 1, opcode, 0);
        
        opcode->opcodeContent += "\tmov qword [rbp + " + std::to_string (argumentAddress) + "], rax\n";

        return opcode;
    } else if (operandValueId == IR::ValueId::INSTRUCTION &&
            static_cast <const IR::Instruction *> (operand)->GetInstructionId () == IR::InstructionId::ALLOCA_INSTRUCTION) {

        const IR::AllocaInstruction *variable = static_cast <const IR::AllocaInstruction *> (instruction->GetOperand (0));

        IR::Opcode *opcode = CreateOpcode ("\n");
        PrintOperandLoad (instruction, 1, opcode, 0);

        opcode->opcodeContent += "\tmov qword [rbp - " + std::to_string (variable->GetStackAddress ()) + "], rax\n";

        return opcode;
    }

    return nullptr;
}

IR::Opcode *x86Opcodes::ProcessLoadInstruction (IR::Instruction *instruction) {
    assert (instruction);
    
    const IR::Value *operand        = instruction->GetOperand (0);
    IR::ValueId      operandValueId = operand->GetValueId ();

    if (operandValueId == IR::ValueId::ARGUMENT) {
        const IR::Argument *functionArgument = static_cast <const IR::Argument *> (operand);
        size_t              argumentAddress = functionArgument->GetType ()->GetSize () * (functionArgument->GetIndex () + 2);

        return CreateOpcode ("\n\tmov qword rax, [rbp + " + std::to_string (argumentAddress) + "]\n"
                               "\tpush rax\n");

    } else if (operandValueId == IR::ValueId::INSTRUCTION && 
            static_cast <const IR::Instruction *> (operand)->GetInstructionId () == IR::InstructionId::ALLOCA_INSTRUCTION) {

        const IR::AllocaInstruction *variable = static_cast <const IR::AllocaInstruction *> (operand);

        return CreateOpcode ("\n\tmov qword rax, [rbp - " + std::to_string (variable->GetStackAddress ()) + "]\n"
                               "\tpush rax\n");
    }

    return nullptr;
}

IR::Opcode *x86Opcodes::ProcessBranchInstruction (IR::Instruction *instruction) {
    assert (instruction);
    
    if (!static_cast <IR::BranchInstruction *> (instruction)->IsConditional ()) {
        return CreateOpcode ("\n\tjmp " + static_cast <const IR::BasicBlock *> (instruction->GetOperand (0))->GetName () + "\n");
    }

    IR::Opcode *opcode = CreateOpcode ();
    opcode->opcodeContent += "\n\t";

    const IR::Instruction *condition = static_cast <const IR::Instruction *> (instruction->GetOperand (0));

    if (condition->GetInstructionId () == IR::InstructionId::CMP_OPERATOR) {
        switch (static_cast <const IR::CmpOperator *> (condition)->GetCmpOperatorId ()) {
            case IR::CmpOperatorId::CMP_L:
                opcode->opcodeContent += "jl";
                break;

            case IR::CmpOperatorId::CMP_G:
                opcode->opcodeContent += "jg";
                break;

            case IR::CmpOperatorId::CMP_E:
                opcode->opcodeContent += "je";
                break;

            case IR::CmpOperatorId::CMP_LE:
                opcode->opcodeContent += "jle";
                break;

            case IR::CmpOperatorId::CMP_GE:
                opcode->opcodeContent += "jge";
                break;

            case IR::CmpOperatorId::CMP_NE:
                opcode->opcodeContent += "jne";
              break;
        }

        opcode->opcodeContent += " "      + static_cast <const IR::BasicBlock *> (instruction->GetOperand (1))->GetName () + "\n";
        opcode->opcodeContent += "\tjmp " + static_cast <const IR::BasicBlock *> (instruction->GetOperand (2))->GetName () + "\n";
    } 

    return opcode;
}

IR::Opcode *x86Opcodes::ProcessCastInstruction (IR::Instruction *instruction) {
    return nullptr;
}

IR::Opcode *x86Opcodes::ProcessCallInstruction (IR::Instruction *instruction) {

    IR::Opcode         *newOpcode = CreateOpcode ("\n");
    const IR::Function *callee    = static_cast <const IR::Function *> (instruction->GetOperand (0));

    newOpcode->opcodeContent += "\tcall " + callee->GetName () + "\n";

    size_t argumentsSize = 0;
    size_t operandsCount = instruction->GetOperandsCount ();

    for (size_t argumentIndex = 1; argumentIndex < operandsCount; argumentIndex++)
        argumentsSize += instruction->GetOperand (argumentIndex)->GetType ()->GetSize ();

    newOpcode->opcodeContent += "\tadd rsp, " + std::to_string (argumentsSize) + "\n"
                                "\tpush rax\n";

    return newOpcode;
}

IR::Opcode *x86Opcodes::ProcessOutInstruction (IR::Instruction *instruction) {
    IR::Opcode *outOpcode = CreateOpcode ("\n");

    const IR::Value *operand = instruction->GetOperand (0); 

    if (operand->GetValueId () == IR::ValueId::CONSTANT) {
        int64_t constValue = 0;
        memcpy (&constValue, static_cast <const IR::ConstantData *> (operand)->GetBytes (), operand->GetType ()->GetSize ());

        outOpcode->opcodeContent += "\tpush " + std::to_string (constValue) + "\n";
    }

    outOpcode->opcodeContent += "\tcall _Print\n"
                                "\tadd rsp, 8\n";

    return outOpcode;
}

IR::Opcode *x86Opcodes::ProcessInInstruction (IR::Instruction *instruction) {
    return CreateOpcode ("\n\tcall _Scan\n"
                           "\tpush rax\n");
}

bool x86Opcodes::PrintOperandLoad (IR::Instruction *instruction, size_t operandIndex, IR::Opcode *opcode, size_t dataRegisterIndex) {
    assert (instruction);
    assert (opcode);

    const IR::Value *operand = instruction->GetOperand (operandIndex);
    if (!operand)
        return false;

    IR::ValueId valueId = operand->GetValueId ();

    if (valueId == IR::ValueId::CONSTANT) {
        int64_t constValue = 0;
        memcpy (&constValue, static_cast <const IR::ConstantData *> (operand)->GetBytes (), operand->GetType ()->GetSize ());

        opcode->opcodeContent += "\tmov " + INTEGER_DATA_REGISTERS [dataRegisterIndex] + ", " + std::to_string (constValue) + "\n";
        return false;
    } else {
        opcode->opcodeContent += "\tpop " + INTEGER_DATA_REGISTERS [dataRegisterIndex] + "\n";
        return true;
    }
}
