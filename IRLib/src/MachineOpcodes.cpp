#include "MachineOpcodes.h"
#include "IRContext.h"
#include "InstructionId.h"

namespace IR {
    Opcode::Opcode (const Opcode &opcode)             : opcodeContent (opcode.opcodeContent) {}
    Opcode::Opcode (const char *opcodeContent)        : opcodeContent (opcodeContent) {}
    Opcode::Opcode (const std::string &opcodeContent) : opcodeContent (opcodeContent) {}
    Opcode::Opcode ()                                 : opcodeContent ("") {}
    
    #define StoreCallback(type, function) instructionCallbacks [InstructionId::type] = &MachineOpcodes::function;
    
    MachineOpcodes::MachineOpcodes (IRContext *context) : context (context) {

        StoreCallback (STATE_CHANGER,      ProcessStateChanger)
        StoreCallback (UNARY_OPERATOR,     ProcessUnaryOperator)
        StoreCallback (BINARY_OPERATOR,    ProcessBinaryOperator)
        StoreCallback (RETURN_OPERATOR,    ProcessReturnOperator)
        StoreCallback (CMP_OPERATOR,       ProcessCmpOperator)
        StoreCallback (ALLOCA_INSTRUCTION, ProcessAllocaInstruction)
        StoreCallback (STORE_INSTRUCTION,  ProcessStoreInstruction)
        StoreCallback (LOAD_INSTRUCTION,   ProcessLoadInstruction)
        StoreCallback (BRANCH_INSTRUCTION, ProcessBranchInstruction)
        StoreCallback (CAST_INSTRUCTION,   ProcessCastInstruction)
        StoreCallback (CALL_INSTRUCTION,   ProcessCallInstruction)
        StoreCallback (IN_INSTRUCTION,     ProcessInInstruction)
        StoreCallback (OUT_INSTRUCTION,    ProcessOutInstruction)
    }
    
    #undef StoreCallback
    
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
    
    Opcode *MachineOpcodes::GetOpcodeByInstruction (Instruction *instruction) {
    
        InstructionCallback callback = instructionCallbacks [instruction->GetInstructionId ()];
        
        if (callback)
            return (this->*callback) (instruction);
    
        return nullptr;
    }

    const std::vector <Opcode *> &MachineOpcodes::GetOpcodes () const { return opcodes; }
}
