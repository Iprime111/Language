#ifndef IR_H_
#define IR_H_

#include <cstddef>
#include <cstdint>

#include "Buffer.h"
#include "Register.h"

const size_t MAX_OPCODE_LENGTH = 16;

struct Value {

};

struct Instruction : public Value {
    char   *name                        = nullptr;
    uint8_t opcode [MAX_OPCODE_LENGTH]  = "";
    size_t  opcodeLength                = 0;

    RegisterSet inputRegisters  = {}; 
    RegisterSet outputRegisters = {}; 
};

struct Function : public Value {
    char  *name      = "";
    size_t argsCount = 0;

};

struct BasicBlock : public Value {
    Buffer <Instruction> instructions = {};
    size_t               blockLength  = 0;
    char                *name         = nullptr;
    Function            *function     = nullptr;
};

#endif
