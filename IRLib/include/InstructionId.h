#ifndef INSTRUCTION_ID_H_
#define INSTRUCTION_ID_H_

namespace IR {
    enum class InstructionId {
        STATE_CHANGER      = 1,
        UNARY_OPERATOR     = 2,
        BINARY_OPERATOR    = 3,
        RETURN_OPERATOR    = 4,
        CMP_OPERATOR       = 5,
        ALLOCA_INSTRUCTION = 6,
        STORE_INSTRUCTION  = 7,
        LOAD_INSTRUCTION   = 8,
        BRANCH_INSTRUCTION = 9,
        CAST_INSTRUCTION   = 10,
        CALL_INSTRUCTION   = 11,
    };
    
    enum class StateChangerId {
        HLT = 0,
    };
    
    enum class UnaryOperatorId {
        SIN   = 0,
        COS   = 1,
        SQRT  = 3,
        FLOOR = 4,
    };
    
    enum class BinaryOperatorId {
        ADD = 0,
        SUB = 1,
        MUL = 2,
        DIV = 3,
        AND = 4,
        OR  = 5,
    };
    
    enum class CmpOperatorId {
        CMP_L  = 0,
        CMP_G  = 1,
        CMP_E  = 2,
        CMP_LE = 3,
        CMP_GE = 4,
        CMP_NE = 5,
    };
    
    enum class CastId {
        TRUNC = 0,
    };
}
#endif
