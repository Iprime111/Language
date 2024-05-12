#ifndef MACHINE_OPCODES_H_
#define MACHINE_OPCODES_H_

#include <string>
#include <unordered_map>

#include "Function.h"
#include "Instruction.h"

namespace IR {
    class MachineOpcodes;
    
    class Opcode final {
        friend class MachineOpcodes;
    
        public:
            std::string opcodeContent = {};
            
             Opcode (const Opcode &opcode);
            ~Opcode () = default;
        
        private:
            Opcode ();
            Opcode (const char *opcodeContent);
            Opcode (const std::string &opcodeContent);
    };
    
    using InstructionCallback = Opcode *(MachineOpcodes::*) (Instruction *instruction);
    
    class MachineOpcodes {
        public:
            virtual ~MachineOpcodes ();
    
            Opcode *GetOpcodeByInstruction (Instruction *instruction);
    
            virtual Opcode *ProcessProgramEnter  ()                       = 0;
            virtual Opcode *ProcessFunctionEnter (Function   *function)   = 0;
            virtual Opcode *ProcessBlockEnter    (BasicBlock *basicBlock) = 0;
    
        protected:
            MachineOpcodes ();
    
            Opcode *CreateOpcode ();
            Opcode *CreateOpcode (const char *opcodeContent);
            Opcode *CreateOpcode (const std::string &opcodeContent);
    
        private:
            std::vector        <Opcode *>                           opcodes;
            std::unordered_map <InstructionId, InstructionCallback> instructionCallbacks;
    
            virtual Opcode *ProcessStateChanger      (Instruction *instruction) = 0;
            virtual Opcode *ProcessUnaryOperator     (Instruction *instruction) = 0;
            virtual Opcode *ProcessBinaryOperator    (Instruction *instruction) = 0;
            virtual Opcode *ProcessReturnOperator    (Instruction *instruction) = 0;
            virtual Opcode *ProcessCmpOperator       (Instruction *instruction) = 0;
            virtual Opcode *ProcessAllocaInstruction (Instruction *instruction) = 0;
            virtual Opcode *ProcessStoreInstruction  (Instruction *instruction) = 0;
            virtual Opcode *ProcessLoadInstruction   (Instruction *instruction) = 0;
            virtual Opcode *ProcessBranchInstruction (Instruction *instruction) = 0;
            virtual Opcode *ProcessCastInstruction   (Instruction *instruction) = 0;
            virtual Opcode *ProcessCallInstruction   (Instruction *instruction) = 0;
    };
}
#endif
