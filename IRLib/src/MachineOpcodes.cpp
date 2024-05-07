#include "MachineOpcodes.h"

Opcode::Opcode (const Opcode &opcode)             : opcodeContent (opcode.opcodeContent) {}
Opcode::Opcode (const char *opcodeContent)        : opcodeContent (opcodeContent) {}
Opcode::Opcode (const std::string &opcodeContent) : opcodeContent (opcodeContent) {}
Opcode::Opcode ()                                 : opcodeContent ("") {}

MachineOpcodes::~MachineOpcodes () {
    for (size_t opcodeIndex = 0; opcodeIndex < opcodes.size (); opcodeIndex++) {
        delete opcodes [opcodeIndex];
    }
}

Opcode *MachineOpcodes::CreateOpcode () {
    Opcode *newOpcode = new Opcode ();

    opcodes.push_back (newOpcode);

    return newOpcode;
}

Opcode *MachineOpcodes::CreateOpcode (const char *opcodeContent) {
    Opcode *newOpcode = new Opcode (opcodeContent);

    opcodes.push_back (newOpcode);

    return newOpcode;
}

Opcode *MachineOpcodes::CreateOpcode (const std::string &opcodeContent) {
    Opcode *newOpcode = new Opcode (opcodeContent);

    opcodes.push_back (newOpcode);

    return newOpcode;
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
