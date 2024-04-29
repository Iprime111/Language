#include <cassert>

#include "IrBuilder.h"
#include "Buffer.h"
#include "Ir.h"
#include "Register.h"

static Instruction *CreateBinaryOperation (IRBuilder *builder, Instruction newInstruction);

// TODO

//class MachineOpcodes {
//    public:
//        virtual std::string get_opcode_by_name();
//    private:
//        std::map<std::string, std::string> name_to_opcode;
//};
//
//class x86Opcodes: MachineOpcodes {
//    ....
//};
//
//IRBuilder {
//    MachineOpcodes *opcodes;
//};

Instruction *IRBuilder::CreateAdd () {
    return CreateBinaryOperation ({
            .name = "add", 
            .opcode = "\x48\x01\xd8",  // TODO move to data tructure
            .opcodeLength = 3,
            .explicitOperands  = RegisterSet (2, RAX, RBX),
            .implicitOperands  = RegisterSet (), 
            .outputRegisters   = RegisterSet (1, RAX)});
}

Instruction *IRBuilder::CreateSub () {
    return CreateBinaryOperation ({
            .name = "sub", 
            .opcode = "\x48\x29\xd8", 
            .opcodeLength = 3,
            .explicitOperands  = RegisterSet (2, RAX, RBX),
            .implicitOperands  = RegisterSet (), 
            .outputRegisters   = RegisterSet (1, RAX)});
}

Instruction *IRBuilder::CreateMul () {
    return CreateBinaryOperation ({
            .name = "mul", 
            .opcode = "\x48\x0f\xaf\xc3", 
            .opcodeLength = 4,
            .explicitOperands  = RegisterSet (2, RAX, RBX),
            .implicitOperands  = RegisterSet (), 
            .outputRegisters   = RegisterSet (1, RAX)});
}

Instruction *IRBuilder::CreateDiv () {
    return CreateBinaryOperation ({
            .name = "div", 
            .opcode = "\x48\x99\x48\xf7\xfb", 
            .opcodeLength = 5, 
            .explicitOperands  = RegisterSet (1, RBX),
            .implicitOperands  = RegisterSet (1, RAX), 
            .outputRegisters   = RegisterSet (1, RAX)});
}

Instruction *IRBuilder::CreateBinaryOperation (Instruction newInstruction) {
    //FIXME works only if insertPoint is BasicBlock
    if (WriteDataToBuffer (&((BasicBlock *) insertPoint)->instructions, &newInstruction, 1) != BufferErrorCode::NO_BUFFER_ERRORS)
        return nullptr;

    ((BasicBlock *) insertPoint)->blockLength += newInstruction.opcodeLength;

    return &builder->insertPoint->instructions.data [builder->insertPoint->instructions.currentIndex - 1];
}
