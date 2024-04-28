#include <cassert>

#include "IrBuilder.h"
#include "Buffer.h"
#include "Ir.h"
#include "Register.h"

static Instruction *CreateBinaryOperation (IRBuilder *builder, Instruction newInstruction);

Instruction *CreateAdd (IRBuilder *builder) {
    assert (builder);

    return CreateBinaryOperation (builder, {.name = "add", .opcode = "\x48\x01\xd8", .opcodeLength = 3, 
                                            .inputRegisters = RegisterSet (2, RAX, RBX), .outputRegisters = RegisterSet (1, RAX)});
}

Instruction *CreateSub (IRBuilder *builder) {
    assert (builder);

    return CreateBinaryOperation (builder, {.name = "sub", .opcode = "\x48\x29\xd8", .opcodeLength = 3, 
                                            .inputRegisters = RegisterSet (2, RAX, RBX), .outputRegisters = RegisterSet (1, RAX)});
}

Instruction *CreateDiv (IRBuilder *builder) {
    assert (builder);

    return CreateBinaryOperation (builder, {.name = "imul", .opcode = "\x48\x0f\xaf\xc3", .opcodeLength = 4, 
                                            .inputRegisters = RegisterSet (2, RAX, RBX), .outputRegisters = RegisterSet (1, RAX)});
}

Instruction *CreateMul (IRBuilder *builder) {
    assert (builder);

    return CreateBinaryOperation (builder, {.name = "div", .opcode = "", .opcodeLength = 3, 
                                            .inputRegisters = RegisterSet (2, RAX, RBX), .outputRegisters = RegisterSet (1, RAX)});
}

static Instruction *CreateBinaryOperation (IRBuilder *builder, Instruction newInstruction) {
    if (WriteDataToBuffer (&builder->insertPoint->instructions, &newInstruction, 1) != BufferErrorCode::NO_BUFFER_ERRORS)
        return nullptr;

    return &builder->insertPoint->instructions.data [builder->insertPoint->instructions.currentIndex - 1];
}
