#ifndef OPCODES_H_
#define OPCODES_H_

#include "Instruction.h"
#include "MachineOpcodes.h"

class x86Opcodes : public MachineOpcodes {
    public:
        Opcode *ProcessFunctionEnter (Function *function) override;

    private:
        Opcode *ProcessStateChanger      (Instruction *instruction) override;
        Opcode *ProcessUnaryOperator     (Instruction *instruction) override;
        Opcode *ProcessBinaryOperator    (Instruction *instruction) override;
        Opcode *ProcessReturnOperator    (Instruction *instruction) override;
        Opcode *ProcessCmpOperator       (Instruction *instruction) override;
        Opcode *ProcessAllocaInstruction (Instruction *instruction) override;
        Opcode *ProcessStoreInstruction  (Instruction *instruction) override;
        Opcode *ProcessLoadInstruction   (Instruction *instruction) override;

        void PrintOperandLoad  (Instruction *instruction, size_t operandIndex, Opcode *opcode, size_t dataRegisterIndex);
        bool IsConstantOperand (Instruction *instruction, size_t operandIndex);
};

#endif
