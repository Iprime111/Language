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

const std::string INTEGER_DATA_REGISTERS [] = {"rax", "rbx"};

Opcode *x86Opcodes::ProcessFunctionEnter (Function *function) {

    size_t allocaSize = function->GetAllocaSize ();
    size_t stackFrameSize = allocaSize + (16 - allocaSize % 16);

    return CreateOpcode (std::string (function->GetName ()) + ":\n" +
                         "push rbp\n"
                         "mov rbp, rsp\n"
                         "sub rsp, " + std::to_string (stackFrameSize) + "\n");
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
            return nullptr;

        case UnaryOperatorId::NOT:
            //TODO logic value type

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
        case BinaryOperatorId::OR:
            //TODO logic value type
            return nullptr;
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
    CmpOperator *cmpOperator = (CmpOperator *) instruction;

    //TODO logic type
    switch (cmpOperator->GetCmpOperatorId ()) {

        case CmpOperatorId::CMP_L:
        case CmpOperatorId::CMP_G:
        case CmpOperatorId::CMP_E:
        case CmpOperatorId::CMP_LE:
        case CmpOperatorId::CMP_GE:
        case CmpOperatorId::CMP_NE:
          break;
    };

    return nullptr;
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

    return CreateOpcode ("mov qword ptr rax, [rbp - " + std::to_string (variable->GetStackAddress ()) + "]\n"
                         "push rax\n");
}

void x86Opcodes::PrintOperandLoad (Instruction *instruction, size_t operandIndex, Opcode *opcode, size_t dataRegisterIndex) {
    assert (instruction);
    assert (opcode);

    const Value *operand = instruction->GetOperand (operandIndex);
    if (!operand)
        return;

    ValueId valueId = operand->GetValueId ();

    if (valueId == ValueId::CONSTANT) {
        //TODO fix fucking alignment (maybe write string in hex?)
        const int64_t *constValue = (const int64_t *) ((const ConstantData *) operand)->GetBytes ();

        opcode->opcodeContent += "mov " + INTEGER_DATA_REGISTERS [dataRegisterIndex] + ", " + std::to_string (*constValue) + "\n";
    } else {
        opcode->opcodeContent += "pop " + INTEGER_DATA_REGISTERS [dataRegisterIndex] + "\n";
    }
}

bool x86Opcodes::IsConstantOperand (Instruction *instruction, size_t operandIndex) {
    assert (instruction);

    const Value *operand = instruction->GetOperand (operandIndex);

    if (!operand)
        return false;

    if (operand->GetValueId () == ValueId::CONSTANT)
        return true;
    else
        return false;
}
