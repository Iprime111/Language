#ifndef MACHINE_OPCODES_H_
#define MACHINE_OPCODES_H_

#include <string>
#include <unordered_map>

#include "Function.h"
#include "IRContext.h"
#include "Instruction.h"

namespace IR {
    class MachineOpcodes;
    
    class Opcode final {
        friend class MachineOpcodes;
    
        public:
            std::string opcodeContent = {};
            
            explicit Opcode (const Opcode &opcode);
            ~Opcode () = default;
        
        private:
            explicit Opcode ();
            explicit Opcode (const char *opcodeContent);
            explicit Opcode (const std::string &opcodeContent);
    };
    
    using InstructionCallback = Opcode *(MachineOpcodes::*) (Instruction *instruction);
    
    class MachineOpcodes {
        public:
            virtual ~MachineOpcodes ();
    
            Opcode *GetOpcodeByInstruction (Instruction *instruction);
    
            virtual Opcode *ProcessProgramEnter  ()                       = 0;
            virtual Opcode *ProcessFunctionEnter (Function   *function)   = 0;
            virtual Opcode *ProcessBlockEnter    (BasicBlock *basicBlock) = 0;

            const std::vector <Opcode *> &GetOpcodes () const;
    
        protected:
            IRContext *context;

            explicit MachineOpcodes (IRContext *context);
    
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
            virtual Opcode *ProcessOutInstruction    (Instruction *instruction) = 0;
            virtual Opcode *ProcessInInstruction     (Instruction *instruction) = 0;
    };
}
#endif
