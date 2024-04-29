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
};

enum class InstructionId {
    NO_ID = 0,
    ADD   = 1,
    SUB   = 2,
    MUL   = 3,
    DIV   = 4,
};

//TODO split to files

class Value { // TODO: def-use chain
              // TODO  UML diagram

    public:
        virtual ~Value () = 0;

        Value *GetParent ();
        Value *GetNext   ();
        Value *GetPrev   ();

        ValueType GetType ();

    protected:
        Value (ValueType valueType);

    private:
        Value *parent = nullptr;
        Value *next   = nullptr;
        Value *prev   = nullptr;

        ValueType valueType = ValueType::VALUE;

};

class Instruction final : public Value {
    public:
        Instruction (InstructionId instructionId);

        InstructionId GetId ();
    private:
        InstructionId instructionId = InstructionId::NO_ID;
};


class BasicBlock final : public Value {
    
    public:
        Buffer <Instruction> instructions = {};

        BasicBlock () = delete;

        char  *GetName   ();
        size_t GetLength ();

        static BasicBlock *Create (char *name, Function *function);

    private:
        char   *name         = nullptr;
        size_t  blockLength  = 0;

        BasicBlock (char *name);
        
};

class Function final : public Value {
    public:
        Buffer <BasicBlock> basicBlocks = {};

        Function () = delete;

        char         *GetName ();
        FunctionType *GetType ();

        static Function *Create (FunctionType *type, char *name, TranslationContext *context);

    private:
        char         *name = nullptr;
        FunctionType  type = {};

        Function (char *name, FunctionType *type);
};

struct ConstantI64 final : public Value {
    int64_t value = 0;
};

#endif
