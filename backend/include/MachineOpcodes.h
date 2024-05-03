#include <cstdint>
#include <unordered_map>

#include "Ir.h"

constexpr size_t MAX_OPCODE_LENGTH  = 16;
constexpr size_t MAX_LISTING_LENGTH = 16;

struct Opcode {
    size_t opcodeLength  = 0;
    size_t listingLength = 0;

    uint8_t opcode  [MAX_OPCODE_LENGTH]  = {};
    uint8_t listing [MAX_LISTING_LENGTH] = {};

    Opcode (size_t opcodeLength, uint8_t *opcode);
};

class MachineOpcodes {
    public:
        virtual ~MachineOpcodes () = 0;

        virtual Opcode *GetOpcodeById (InstructionId id);

    protected:
        MachineOpcodes () = default;

        std::unordered_map <InstructionId, Opcode> idToOpcode = {};
};

class x86Opcodes final: MachineOpcodes {
    public:
        x86Opcodes ();

};

