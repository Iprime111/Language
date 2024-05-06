#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "Opcodes.h"
#include "Instruction.h"
#include "Value.h"

constexpr size_t MAX_INSTRUCTION_LENGTH = 128;

Opcode *x86Opcodes::ProcessStateChanger (Instruction *instruction) {
    assert (instruction);
 
    //WARNING works only for hlt
    char *hltOpcode = "mov rax, 0x3c\n"
                      "mov rsi, 0x00\n"
                      "syscall\n";

    return InsertOpcode (strlen (hltOpcode), hltOpcode);
}

Opcode *x86Opcodes::ProcessUnaryOperator (Instruction *instruction) {
    assert (instruction);

    UnaryOperator *unaryOperator = (UnaryOperator *) instruction;
    
    switch (unaryOperator->GetUnaryOperatorId ()) {

        //TODO
        case UnaryOperatorId::SIN:
        case UnaryOperatorId::COS:
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
    
    char *opcode = (char *) calloc (MAX_INSTRUCTION_LENGTH, sizeof (char));

    switch (binaryOperator->GetBinaryOperatorId ()) {

        case BinaryOperatorId::ADD:
            opcode = "pop rax\n"
                     "pop rbx\n"
                     "add rax, rbx\n"
                     "push rax\n";
            break;

        case BinaryOperatorId::SUB:
            opcode = "pop rax\n"
                     "pop rbx\n"
                     "sub rax, rbx\n"
                     "push rax\n";
            break;

        case BinaryOperatorId::MUL:
            opcode = "pop rax\n"
                     "pop rbx\n"
                     "imul rax, rbx\n"
                     "push rax\n";
            break;

        case BinaryOperatorId::DIV:
            opcode = "pop rax\n"
                     "pop rbx\n"
                     "cqo\n"
                     "idiv rbx\n"
                     "push rax\n";
            break;

        case BinaryOperatorId::AND:
        case BinaryOperatorId::OR:
            //TODO logic value type
            return nullptr;
    }

    return InsertOpcode (strlen (opcode), opcode);
}

Opcode *x86Opcodes::ProcessReturnOperator (Instruction *instruction) {
    char *opcode = "pop rax\n"
                   "leave\n"
                   "ret\n";

    return InsertOpcode (strlen (opcode), opcode);
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

    char *storeOpcode = (char *) calloc (MAX_INSTRUCTION_LENGTH, sizeof (char));

    //TODO types
    snprintf (storeOpcode, MAX_INSTRUCTION_LENGTH,  "pop rdx\n"
                                                    "mov qword ptr [rbp - %lu], rdx\n",
                                                    variable->GetStackAddress ());

    return InsertOpcode (strlen (storeOpcode), storeOpcode);
}

Opcode *x86Opcodes::ProcessLoadInstruction (Instruction *instruction) {
    assert (instruction);

    const AllocaInstruction *variable = (const AllocaInstruction *) instruction->GetOperand (0);

    char *loadOpcode = (char *) calloc (MAX_INSTRUCTION_LENGTH, sizeof (char));

    snprintf (loadOpcode, MAX_INSTRUCTION_LENGTH, "mov qword ptr rdx, [rbp - %lu]\n"
                                                  "push rdx\n",
                                                  variable->GetStackAddress ());
    
    return InsertOpcode (strlen (loadOpcode), loadOpcode);
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
