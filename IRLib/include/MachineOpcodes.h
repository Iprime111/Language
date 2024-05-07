#ifndef MACHINE_OPCODES_H_
#define MACHINE_OPCODES_H_

#include <string>

#include "Function.h"
#include "Instruction.h"

class MachineOpcodes;

class Opcode {
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

class MachineOpcodes {
    public:
        virtual ~MachineOpcodes ();

        Opcode *GetOpcodeByInstruction (Instruction *instruction);
        virtual Opcode *ProcessFunctionEnter (Function *function) = 0;

    protected:
        MachineOpcodes () = default;

        Opcode *CreateOpcode ();
        Opcode *CreateOpcode (const char *opcodeContent);
        Opcode *CreateOpcode (const std::string &opcodeContent);

    private:
        std::vector <Opcode *> opcodes = {};

        virtual Opcode *ProcessStateChanger          (Instruction *instruction) = 0;
        virtual Opcode *ProcessUnaryOperator         (Instruction *instruction) = 0;
        virtual Opcode *ProcessBinaryOperator        (Instruction *instruction) = 0;
        virtual Opcode *ProcessReturnOperator        (Instruction *instruction) = 0;
        virtual Opcode *ProcessCmpOperator           (Instruction *instruction) = 0;
        virtual Opcode *ProcessAllocaInstruction     (Instruction *instruction) = 0;
        virtual Opcode *ProcessStoreInstruction      (Instruction *instruction) = 0;
        virtual Opcode *ProcessLoadInstruction       (Instruction *instruction) = 0;
};

#endif
