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

static const std::string XMM_DATA_REGISTERS []     = {"xmm0", "xmm1"};
static const size_t      REQUIRED_STACK_ALIGNMENT  = 16;

x86Opcodes::x86Opcodes (IR::IRContext *irContext) : MachineOpcodes (irContext) {}

IR::Opcode *x86Opcodes::ProcessProgramEnter () {
    
    IR::Opcode *newOpcode = CreateOpcode ("%include \"../build/bin/Resources/stdlib.asm\"\n"
                                          "section .data\n");

    size_t constantsCount = context->constants.size ();

    for (size_t constantIndex = 0; constantIndex < constantsCount; constantIndex++) {
        IR::ConstantData *constant = context->constants [constantIndex];
        
        double constValue = 0;
        memcpy (&constValue, constant->GetBytes (), constant->GetType ()->GetSize ());

        constantsIndices [constant] = constantIndex;

        newOpcode->opcodeContent += "\t_CONSTANT_" + std::to_string (constantIndex) + " dq " + std::to_string (constValue) + "\n";
    }

    newOpcode->opcodeContent += "\nsection .text\n"
                                "global _start\n\n"
                                "_start:\n"
                                "\tcall " + context->entryPoint->GetName () + "\n\n"
                                "\tmov rax, 0x3c ; exit\n"
                                "\tmov rsi, 0x00 ; exit code\n"
                                "\tsyscall\n";

    return newOpcode;
}

IR::Opcode *x86Opcodes::ProcessFunctionEnter (IR::Function *function) {
    assert (function);

    size_t allocaSize = function->GetAllocaSize ();
    size_t stackFrameSize = allocaSize + (REQUIRED_STACK_ALIGNMENT - allocaSize % REQUIRED_STACK_ALIGNMENT);

    return CreateOpcode ("\n" + std::string (function->GetName ()) + ":\n" +
                         "\tpush rbp\n"
                         "\tmov rbp, rsp\n"
                         "\tsub rsp, " + std::to_string (stackFrameSize) + " ; set up stack frame\n");
}

IR::Opcode *x86Opcodes::ProcessBlockEnter (IR::BasicBlock *basicBlock) {
    assert (basicBlock);

    return CreateOpcode ("\n" + basicBlock->GetName () + ":\n");
}

IR::Opcode *x86Opcodes::ProcessStateChanger (IR::Instruction *instruction) {
    assert (instruction);

    return CreateOpcode ("\n\tmov rax, 0x3c\n"
                           "\tmov rsi, 0x00\n"
                           "\tsyscall ; exit syscall\n");
}

IR::Opcode *x86Opcodes::ProcessUnaryOperator (IR::Instruction *instruction) {
    assert (instruction);

    IR::UnaryOperator *unaryOperator = (IR::UnaryOperator *) instruction;
    
    IR::Opcode *newOpcode = CreateOpcode ("\n");
    PrintOperandLoad (instruction, 0, newOpcode, 0);

    switch (unaryOperator->GetUnaryOperatorId ()) {

        //TODO
        case IR::UnaryOperatorId::SIN:
        case IR::UnaryOperatorId::COS:
            return nullptr;

        case IR::UnaryOperatorId::FLOOR:
            newOpcode->opcodeContent += "\tcvttsd2si rax, " + XMM_DATA_REGISTERS [0] + "\n"
                                        "\tcvtsi2sd "       + XMM_DATA_REGISTERS [0] + ", rax ; floor value\n";
            break;

        case IR::UnaryOperatorId::SQRT:
            newOpcode->opcodeContent += "\tsqrtsd " + XMM_DATA_REGISTERS [0] + ", " + XMM_DATA_REGISTERS [0] + "\n";
            break;
    }

    PrintOperandStore (newOpcode, 0);
    return newOpcode;
}

IR::Opcode *x86Opcodes::ProcessBinaryOperator (IR::Instruction *instruction) {
    IR::BinaryOperator *binaryOperator = (IR::BinaryOperator *) instruction;
    
    IR::Opcode *opcode = CreateOpcode ("\n");


    PrintOperandLoad (instruction, 1, opcode, 1);
    PrintOperandLoad (instruction, 0, opcode, 0);
    
    switch (binaryOperator->GetBinaryOperatorId ()) {

        case IR::BinaryOperatorId::ADD:
            opcode->opcodeContent += "\taddsd ";
            break;

        case IR::BinaryOperatorId::SUB:
            opcode->opcodeContent += "\tsubsd ";
            break;

        case IR::BinaryOperatorId::MUL:
            opcode->opcodeContent += "\tmulsd ";
            break;

        case IR::BinaryOperatorId::DIV:
            opcode->opcodeContent += "\tdivsd ";
            break;
    }

    opcode->opcodeContent += XMM_DATA_REGISTERS [0] + ", " + XMM_DATA_REGISTERS [1] + "\n";

    PrintOperandStore (opcode, 0);
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

    opcode->opcodeContent += "\n\tcomisd " + XMM_DATA_REGISTERS [0] + ", " + XMM_DATA_REGISTERS [1] + " ; compare\n";

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
        
        opcode->opcodeContent += "\tmovsd [rbp + " + std::to_string (argumentAddress) + "], " + XMM_DATA_REGISTERS [0] + 
                                 " ; store function argument \"" + functionArgument->GetName () + "\"\n";

        return opcode;
    } else if (operandValueId == IR::ValueId::INSTRUCTION &&
            static_cast <const IR::Instruction *> (operand)->GetInstructionId () == IR::InstructionId::ALLOCA_INSTRUCTION) {

        const IR::AllocaInstruction *variable = static_cast <const IR::AllocaInstruction *> (instruction->GetOperand (0));

        IR::Opcode *opcode = CreateOpcode ("\n");
        PrintOperandLoad (instruction, 1, opcode, 0);

        opcode->opcodeContent += "\tmovsd [rbp - " + std::to_string (variable->GetStackAddress ()) + "], " + XMM_DATA_REGISTERS [0] + 
                                 " ; store local variable \"" + variable->GetName () + "\"\n";

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

        IR::Opcode *newOpcode = CreateOpcode ("\n\tmovsd " + XMM_DATA_REGISTERS [0] + ", [rbp + " + std::to_string (argumentAddress) + 
                                             "] ; load function argument \"" + functionArgument->GetName () + "\"\n");
        PrintOperandStore (newOpcode, 0);

        return newOpcode;

    } else if (operandValueId == IR::ValueId::INSTRUCTION && 
            static_cast <const IR::Instruction *> (operand)->GetInstructionId () == IR::InstructionId::ALLOCA_INSTRUCTION) {

        const IR::AllocaInstruction *variable = static_cast <const IR::AllocaInstruction *> (operand);

        IR::Opcode *newOpcode = CreateOpcode ("\n\tmovsd " + XMM_DATA_REGISTERS [0] + ", [rbp - " + std::to_string (variable->GetStackAddress ()) + 
                                              "] ; load local variable \"" + variable->GetName () +"\"\n");

        PrintOperandStore (newOpcode, 0);
        
        return newOpcode;
    }

    return nullptr;
}

IR::Opcode *x86Opcodes::ProcessBranchInstruction (IR::Instruction *instruction) {
    assert (instruction);
    
    if (!static_cast <IR::BranchInstruction *> (instruction)->IsConditional ()) {
        return CreateOpcode ("\n\tjmp " + static_cast <const IR::BasicBlock *> (instruction->GetOperand (0))->GetName () + 
                             " ; jump to specified block\n");
    }

    IR::Opcode *opcode = CreateOpcode ();
    opcode->opcodeContent += "\n\t";

    const IR::Instruction *condition = static_cast <const IR::Instruction *> (instruction->GetOperand (0));

    if (condition->GetInstructionId () == IR::InstructionId::CMP_OPERATOR) {
        switch (static_cast <const IR::CmpOperator *> (condition)->GetCmpOperatorId ()) {
            case IR::CmpOperatorId::CMP_L:
                opcode->opcodeContent += "jb ";
                break;

            case IR::CmpOperatorId::CMP_G:
                opcode->opcodeContent += "ja ";
                break;

            case IR::CmpOperatorId::CMP_E:
                opcode->opcodeContent += "je ";
                break;

            case IR::CmpOperatorId::CMP_LE:
                opcode->opcodeContent += "jbe ";
                break;

            case IR::CmpOperatorId::CMP_GE:
                opcode->opcodeContent += "jae ";
                break;

            case IR::CmpOperatorId::CMP_NE:
                opcode->opcodeContent += "jne ";
              break;
        }

        opcode->opcodeContent += static_cast <const IR::BasicBlock *> (instruction->GetOperand (1))->GetName () + 
                                 " ; jump to true block if condition is true\n";
        
        opcode->opcodeContent += "\tjmp " + static_cast <const IR::BasicBlock *> (instruction->GetOperand (2))->GetName () + 
                                 " ; jump to false block if not\n";
    } 

    return opcode;
}

IR::Opcode *x86Opcodes::ProcessCastInstruction (IR::Instruction *instruction) {
    return nullptr;
}

IR::Opcode *x86Opcodes::ProcessCallInstruction (IR::Instruction *instruction) {

    IR::Opcode         *newOpcode = CreateOpcode ("\n");
    const IR::Function *callee    = static_cast <const IR::Function *> (instruction->GetOperand (0));

    newOpcode->opcodeContent += "\tcall " + callee->GetName () + "; make call\n";

    size_t argumentsSize = 0;
    size_t operandsCount = instruction->GetOperandsCount ();

    for (size_t argumentIndex = 1; argumentIndex < operandsCount; argumentIndex++)
        argumentsSize += instruction->GetOperand (argumentIndex)->GetType ()->GetSize ();

    newOpcode->opcodeContent += "\tadd rsp, " + std::to_string (argumentsSize) + "; balance stack (remove pushed args)\n";

    PrintOperandStore (newOpcode, 0);

    return newOpcode;
}

IR::Opcode *x86Opcodes::ProcessOutInstruction (IR::Instruction *instruction) {
    IR::Opcode *outOpcode = CreateOpcode ("\n");

    const IR::Value *operand = instruction->GetOperand (0); 

    if (operand->GetValueId () == IR::ValueId::CONSTANT) {
        PrintOperandLoad (instruction, 0, outOpcode, 0);
        PrintOperandStore (outOpcode,  0);
    }

    outOpcode->opcodeContent += "\tcall _Print ; print number\n"
                                "\tadd rsp, 8  ; balance stack\n";

    return outOpcode;
}

IR::Opcode *x86Opcodes::ProcessInInstruction (IR::Instruction *instruction) {
    IR::Opcode *newOpcode = CreateOpcode ("\n\tcall _Scan ; scan value from stdin \n");

    PrintOperandStore (newOpcode, 0);

    return newOpcode;
}

void x86Opcodes::PrintOperandLoad (IR::Instruction *instruction, size_t operandIndex, IR::Opcode *opcode, size_t dataRegisterIndex) {
    assert (instruction);
    assert (opcode);

    const IR::Value *operand = instruction->GetOperand (operandIndex);
    if (!operand)
        return;

    IR::ValueId valueId = operand->GetValueId ();

    if (valueId == IR::ValueId::CONSTANT) {
        opcode->opcodeContent += "\tmovsd " + XMM_DATA_REGISTERS [dataRegisterIndex] + ", [_CONSTANT_" + 
            std::to_string (constantsIndices [static_cast <const IR::Constant *> (operand)]) + "] ; load constant\n";
    } else {
        opcode->opcodeContent += "\tmovsd " + XMM_DATA_REGISTERS [dataRegisterIndex] + ", [rsp]\n"
                                 "\tadd rsp, 8 ; load value from stack\n";
    }
}

void x86Opcodes::PrintOperandStore (IR::Opcode *opcode, size_t dataRegisterIndex) {
    opcode->opcodeContent += "\tsub rsp, 8\n"
                             "\tmovsd [rsp], " + XMM_DATA_REGISTERS [dataRegisterIndex] + "; store value to stack\n";
}
