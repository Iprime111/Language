#include "MachineOpcodes.h"

Opcode::Opcode (size_t opcodeLength, char *opcode) : opcodeLength (opcodeLength), opcode (opcode) {}

Opcode *MachineOpcodes::InsertOpcode (size_t opcodeLength, char *opcode) {
    opcodes.push_back (Opcode (opcodeLength, opcode));
    
    return &opcodes [opcodes.size () - 1];
}

#define ProcessInstructionType(type, callback)  \
    case InstructionId::type:                   \
        return callback (instruction);

Opcode *MachineOpcodes::GetOpcodeByInstruction (Instruction *instruction) {
    switch (instruction->GetInstructionId ()) {

        ProcessInstructionType (STATE_CHANGER,          ProcessStateChanger)
        ProcessInstructionType (UNARY_OPERATOR,         ProcessUnaryOperator)
        ProcessInstructionType (BINARY_OPERATOR,        ProcessBinaryOperator)
        ProcessInstructionType (RETURN_OPERATOR,        ProcessReturnOperator)
        ProcessInstructionType (CMP_OPERATOR,           ProcessCmpOperator)
        ProcessInstructionType (ALLOCATION_INSTRUCTION, ProcessAllocaInstruction)
        ProcessInstructionType (STORE_INSTRUCTION,      ProcessStoreInstruction)
        ProcessInstructionType (LOAD_INSTRUCTION,       ProcessLoadInstruction)
    }

    return nullptr;
}
