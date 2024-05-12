#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include "Opcodes.h"
#include "Constant.h"
#include "Function.h"
#include "Instruction.h"
#include "Value.h"

static const std::string INTEGER_DATA_REGISTERS [] = {"rax", "rbx"};
static const size_t      REQUIRED_STACK_ALIGNMENT  = 16;

Opcode *x86Opcodes::ProcessProgramEnter () {
    return CreateOpcode ("section .text\n"
                         "global _start\n");
}

Opcode *x86Opcodes::ProcessFunctionEnter (Function *function) {
    assert (function);

    size_t allocaSize = function->GetAllocaSize ();
    size_t stackFrameSize = allocaSize + (REQUIRED_STACK_ALIGNMENT - allocaSize % REQUIRED_STACK_ALIGNMENT);

    return CreateOpcode (std::string (function->GetName ()) + ":\n" +
                         "push rbp\n"
                         "mov rbp, rsp\n"
                         "sub rsp, " + std::to_string (stackFrameSize) + "\n");
}

Opcode *x86Opcodes::ProcessBlockEnter (BasicBlock *basicBlock) {
    assert (basicBlock);

    return CreateOpcode ("BLOCK_" + std::to_string (basicBlock->GetLabelIndex ()) + ":\n");
}

Opcode *x86Opcodes::ProcessStateChanger (Instruction *instruction) {
    assert (instruction);

    //WARNING works only for hlt
    return CreateOpcode ("mov rax, 0x3c\n"
                         "mov rsi, 0x00\n"
                         "syscall\n");
}

Opcode *x86Opcodes::ProcessUnaryOperator (Instruction *instruction) {
    assert (instruction);

    UnaryOperator *unaryOperator = (UnaryOperator *) instruction;
    
    switch (unaryOperator->GetUnaryOperatorId ()) {

        //TODO
        case UnaryOperatorId::SIN:
        case UnaryOperatorId::COS:
            //TODO fuck...
            break;

        case UnaryOperatorId::FLOOR:
        case UnaryOperatorId::SQRT:
            //TODO double type
            break;
    }

    return nullptr;
}

Opcode *x86Opcodes::ProcessBinaryOperator (Instruction *instruction) {
    BinaryOperator *binaryOperator = (BinaryOperator *) instruction;
    
    Opcode *opcode = CreateOpcode ();

    PrintOperandLoad (instruction, 0, opcode, 0);
    PrintOperandLoad (instruction, 1, opcode, 0);
    
    switch (binaryOperator->GetBinaryOperatorId ()) {

        case BinaryOperatorId::ADD:
            opcode->opcodeContent += "add rax, rbx\n"
                                     "push rax\n";
            break;

        case BinaryOperatorId::SUB:
            opcode->opcodeContent += "sub rax, rbx\n"
                                     "push rax\n";
            break;

        case BinaryOperatorId::MUL:
            opcode->opcodeContent += "imul rax, rbx\n"
                                     "push rax\n";
            break;

        case BinaryOperatorId::DIV:
            opcode->opcodeContent += "cqo\n"
                                     "idiv rbx\n"
                                     "push rax\n";
            break;

        case BinaryOperatorId::AND:
            break;//TODO

        case BinaryOperatorId::OR:
            break;//TODO
    }

    return opcode;
}

Opcode *x86Opcodes::ProcessReturnOperator (Instruction *instruction) {
    assert (instruction);

    Opcode *opcode = CreateOpcode ();
    PrintOperandLoad (instruction, 0, opcode, 0);

    opcode->opcodeContent += "leave\n"
                             "ret\n";

    return opcode;
}

Opcode *x86Opcodes::ProcessCmpOperator (Instruction *instruction) {
    Opcode *opcode = CreateOpcode (); 

    PrintOperandLoad (instruction, 0, opcode, 0);
    PrintOperandLoad (instruction, 1, opcode, 1);

    opcode->opcodeContent += "cmp rax, rbx\n";

    return opcode;
}

Opcode *x86Opcodes::ProcessAllocaInstruction (Instruction *instruction) {
    return nullptr;
}

Opcode *x86Opcodes::ProcessStoreInstruction (Instruction *instruction) {
    assert (instruction);

    const AllocaInstruction *variable = (const AllocaInstruction *) instruction->GetOperand (0);

    Opcode *opcode = CreateOpcode ();
    PrintOperandLoad (instruction, 1, opcode, 0);

    opcode->opcodeContent += "mov qword ptr [rbp - " + std::to_string (variable->GetStackAddress ()) + "], rax\n";

    //TODO types
    //TODO use std::format?
    return opcode;
}

Opcode *x86Opcodes::ProcessLoadInstruction (Instruction *instruction) {
    assert (instruction);

    const AllocaInstruction *variable = (const AllocaInstruction *) instruction->GetOperand (0);

    if (instruction->prev && instruction->prev->GetInstructionId () == InstructionId::STORE_INSTRUCTION &&
        instruction->prev->GetOperand (0) == instruction->GetOperand (0))
            return CreateOpcode ("push rax\n"); //value is already in rax

    return CreateOpcode ("mov qword ptr rax, [rbp - " + std::to_string (variable->GetStackAddress ()) + "]\n"
                         "push rax\n");
}

Opcode *x86Opcodes::ProcessBranchInstruction (Instruction *instruction) {
    assert (instruction);

    if (!((BranchInstruction *) instruction)->IsConditional ()) {
        return CreateOpcode ("jmp " + std::to_string (((const BasicBlock *) instruction->GetOperand (0))->GetLabelIndex ()) + "\n");
    }

    const Instruction *condition = (const Instruction *) instruction->GetOperand (0);
    
    Opcode *opcode = CreateOpcode ();

    if (condition->GetInstructionId () == InstructionId::CMP_OPERATOR) {
        switch (((const CmpOperator *) condition)->GetCmpOperatorId ()) {
            case CmpOperatorId::CMP_L:
                opcode->opcodeContent += "jl";
                break;

            case CmpOperatorId::CMP_G:
                opcode->opcodeContent += "jg";
                break;

            case CmpOperatorId::CMP_E:
                opcode->opcodeContent += "je";
                break;

            case CmpOperatorId::CMP_LE:
                opcode->opcodeContent += "jle";
                break;

            case CmpOperatorId::CMP_GE:
                opcode->opcodeContent += "jge";
                break;

            case CmpOperatorId::CMP_NE:
                opcode->opcodeContent += "jne";
              break;
        }

        //TODO optimize jumps
        opcode->opcodeContent += " BLOCK_"    + std::to_string (((const BasicBlock *) instruction->GetOperand (1))->GetLabelIndex ()) + "\n";
        opcode->opcodeContent += "jmp BLOCK_" + std::to_string (((const BasicBlock *) instruction->GetOperand (2))->GetLabelIndex ()) + "\n";
    } else {
        //TODO
    }

    return opcode;
}

void x86Opcodes::PrintOperandLoad (Instruction *instruction, size_t operandIndex, Opcode *opcode, size_t dataRegisterIndex) {
    assert (instruction);
    assert (opcode);

    const Value *operand = instruction->GetOperand (operandIndex);
    if (!operand)
        return;

    ValueId valueId = operand->GetValueId ();

    if (valueId == ValueId::CONSTANT) {
        int64_t constValue = 0;
        memcpy (&constValue, ((const ConstantData *) operand)->GetBytes (), operand->GetType ()->GetSize ());

        opcode->opcodeContent += "mov " + INTEGER_DATA_REGISTERS [dataRegisterIndex] + ", " + std::to_string (constValue) + "\n";
    } else {
        opcode->opcodeContent += "pop " + INTEGER_DATA_REGISTERS [dataRegisterIndex] + "\n";
    }
}
