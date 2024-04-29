#ifndef IR_H_
#define IR_H_

#include <cstddef>
#include <cstdint>

#include "Buffer.h"
#include "FunctionType.h"
#include "Register.h"

const size_t MAX_OPCODE_LENGTH = 16;

class Function;
struct TranslationContext;

enum class ValueType {
    VALUE        = 1 << 0,
    INSTRUCTION  = 1 << 1,
    BASIC_BLOCK  = 1 << 2,
    FUNCTION     = 1 << 3,
    CONSTANT_I64 = 1 << 4,
};

class Value { // TODO: def-use chain
              // TODO  UML diagram

    //TODO GetParent (), GetNext (), GetPrev ()
    public:
        Value *parent    = nullptr;
        Value *nextBlock = nullptr;
        Value *prevBlock = nullptr;

        ValueType valueType = ValueType::VALUE;

        virtual ~Value () = default;
};

class Instruction final : public Value {
    public:
        char   *name                        = nullptr;
        uint8_t opcode [MAX_OPCODE_LENGTH]  = "";
        size_t  opcodeLength                = 0;

        RegisterSet explicitOperands = {};
        RegisterSet implicitOperands = {};
        RegisterSet outputRegisters  = {};
};


class BasicBlock final : public Value {
    
    public:
        Buffer <Instruction> instructions = {};
        size_t               blockLength  = 0;
        char                *name         = nullptr;

        BasicBlock () = delete;

        static BasicBlock *Create (char *name, Function *function);

    private:
        BasicBlock (char *name);
        
        //TODO reinitialize valueType
};

class Function final : public Value {
    public:
        char               *name        = nullptr;
        FunctionType        type        = {};
        Buffer <BasicBlock> basicBlocks = {};
        
        Function () = delete;

        static Function *Create (FunctionType *type, char *name, TranslationContext *context);

    private:
        Function (char *name, FunctionType *type);
};

struct ConstantI64 final : public Value {
    int64_t value = 0;
};

#endif
