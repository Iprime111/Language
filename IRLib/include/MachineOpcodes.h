#ifndef MACHINE_OPCODES_H_
#define MACHINE_OPCODES_H_

#include <unordered_map>

#include "Instruction.h"

//TODO destructor?
struct Opcode {
    size_t opcodeLength  = 0;
    char  *opcode        = nullptr;

    Opcode  (size_t opcodeLength, char *opcode);
    ~Opcode () = default;
};

template <typename KeyType>
using OpcodeUnorderedMap = std::unordered_map <KeyType, Opcode>;

class MachineOpcodes {
    public:
        virtual ~MachineOpcodes () = default;

        Opcode *GetOpcodeByInstruction (Instruction *instruction);

    protected:
        MachineOpcodes () = default;

        Opcode *InsertOpcode (size_t opcodeLength, char *opcode);

        OpcodeUnorderedMap <StateChangerId>          stateChangers          = {};
        OpcodeUnorderedMap <UnaryOperatorId>         unaryOperators         = {};
        OpcodeUnorderedMap <BinaryOperatorId>        binaryOperators        = {};
        OpcodeUnorderedMap <CmpOperatorId>           cmpOperators           = {};

    private:
        std::vector <Opcode> opcodes = {};

        virtual Opcode *ProcessStateChanger          (Instruction *instruction) = 0;
        virtual Opcode *ProcessUnaryOperator         (Instruction *instruction) = 0;
        virtual Opcode *ProcessBinaryOperator        (Instruction *instruction) = 0;
        virtual Opcode *ProcessReturnOperator        (Instruction *instruction) = 0;
        virtual Opcode *ProcessCmpOperator           (Instruction *instruction) = 0;
        virtual Opcode *ProcessAllocaInstruction (Instruction *instruction) = 0;
        virtual Opcode *ProcessStoreInstruction      (Instruction *instruction) = 0;
        virtual Opcode *ProcessLoadInstruction       (Instruction *instruction) = 0;
};

#endif
