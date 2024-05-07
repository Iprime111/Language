#ifndef INSTRUCTION_H_
#define INSTRUCTION_H_

#include "FunctionType.h"
#include "User.h"

enum class InstructionId {
    STATE_CHANGER          = 1,
    UNARY_OPERATOR         = 2,
    BINARY_OPERATOR        = 3,
    RETURN_OPERATOR        = 4,
    CMP_OPERATOR           = 5,
    ALLOCA_INSTRUCTION     = 6,
    STORE_INSTRUCTION      = 7,
    LOAD_INSTRUCTION       = 8
};

class Instruction : public User {
    public:
        ~Instruction () = default;
        InstructionId GetInstructionId () const;

        Instruction *next = nullptr;
        Instruction *prev = nullptr;

    protected:
        Instruction (InstructionId instructionId, const Type *instructionType);
        Instruction (InstructionId instructionId, const Type *instructionType, size_t operandsCount);
    
    private:
        InstructionId instructionId = {};
};

enum class StateChangerId {
    HLT = 0,
};

class StateChanger final : public Instruction {
    public:
        StateChanger (StateChangerId id);

        StateChangerId GetStateChangerId () const;

    private:
        StateChangerId id = {};
};

enum class UnaryOperatorId {
    SIN   = 0,
    COS   = 1,
    NOT   = 2,
    SQRT  = 3,
    FLOOR = 4,
};

class UnaryOperator final : public Instruction {
    public:
        UnaryOperator (UnaryOperatorId id, Value *operand);

        UnaryOperatorId GetUnaryOperatorId () const;

    private:
        UnaryOperatorId id = {};
};

enum class BinaryOperatorId {
    ADD = 0,
    SUB = 1,
    MUL = 2,
    DIV = 3,
    AND = 4,
    OR  = 5,
};

class BinaryOperator final : public Instruction {
    public:
        BinaryOperator (BinaryOperatorId id, Value *firstOperand, Value *secondOperand);

        BinaryOperatorId GetBinaryOperatorId () const;

    private:
        BinaryOperatorId id = {};
};

class ReturnOperator final : public Instruction {
    public:
        ReturnOperator (Value *operand);
};

enum class CmpOperatorId {
    CMP_L  = 0,
    CMP_G  = 1,
    CMP_E  = 2,
    CMP_LE = 3,
    CMP_GE = 4,
    CMP_NE = 5,
};

class CmpOperator final : public Instruction {
    public:
        CmpOperator (CmpOperatorId id, Value *firstOperand, Value *secondOperand);

        CmpOperatorId GetCmpOperatorId () const;

    private:
        CmpOperatorId id = {};
};

class AllocaInstruction final : public Instruction {
    public:
        AllocaInstruction (const Type *type, size_t stackAddress);

        size_t GetStackAddress () const;

    private:
        size_t stackAddress = 0;
};

class StoreInstruction final : public Instruction {
    public:
        StoreInstruction (AllocaInstruction *variable, Value *operand);
};

class LoadInstruction final : public Instruction {
    public:
        LoadInstruction (AllocaInstruction *variable);
};

#endif
