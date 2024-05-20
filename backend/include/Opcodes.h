#ifndef OPCODES_H_
#define OPCODES_H_

#include <unordered_map>

#include "Constant.h"
#include "IRContext.h"
#include "Instruction.h"
#include "MachineOpcodes.h"

class x86Opcodes final : public IR::MachineOpcodes {
    public:
        x86Opcodes (IR::IRContext *irContext);

        IR::Opcode *ProcessProgramEnter  ()                           override;
        IR::Opcode *ProcessFunctionEnter (IR::Function   *function)   override;
        IR::Opcode *ProcessBlockEnter    (IR::BasicBlock *basicBlock) override;

    private:
        std::unordered_map <const IR::Constant *, size_t> constantsIndices;

        IR::Opcode *ProcessStateChanger      (IR::Instruction *instruction) override;
        IR::Opcode *ProcessUnaryOperator     (IR::Instruction *instruction) override;
        IR::Opcode *ProcessBinaryOperator    (IR::Instruction *instruction) override;
        IR::Opcode *ProcessReturnOperator    (IR::Instruction *instruction) override;
        IR::Opcode *ProcessCmpOperator       (IR::Instruction *instruction) override;
        IR::Opcode *ProcessAllocaInstruction (IR::Instruction *instruction) override;
        IR::Opcode *ProcessStoreInstruction  (IR::Instruction *instruction) override;
        IR::Opcode *ProcessLoadInstruction   (IR::Instruction *instruction) override;
        IR::Opcode *ProcessBranchInstruction (IR::Instruction *instruction) override;
        IR::Opcode *ProcessCastInstruction   (IR::Instruction *instruction) override;
        IR::Opcode *ProcessCallInstruction   (IR::Instruction *instruction) override;
        IR::Opcode *ProcessOutInstruction    (IR::Instruction *instruction) override;
        IR::Opcode *ProcessInInstruction     (IR::Instruction *instruction) override;

        void PrintOperandLoad  (IR::Instruction *instruction, size_t operandIndex, IR::Opcode *opcode, size_t dataRegisterIndex);
        void PrintOperandStore (IR::Opcode *opcode, size_t dataRegisterIndex);
};

#endif
