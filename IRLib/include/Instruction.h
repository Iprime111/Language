#ifndef INSTRUCTION_H_
#define INSTRUCTION_H_

#include "User.h"

enum class InstructionId {
    NO_ID = 0,
    ADD   = 1,
    SUB   = 2,
    MUL   = 3,
    DIV   = 4,
};

class Instruction final : public User {
    public:
        Instruction (InstructionId instructionId);

        InstructionId GetId ();
    private:
        InstructionId instructionId = InstructionId::NO_ID;
};

#endif
