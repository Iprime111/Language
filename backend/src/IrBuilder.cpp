#include <cassert>

#include "IrBuilder.h"
#include "Buffer.h"
#include "Ir.h"

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
    return InsertInstruction (Instruction (InstructionId::ADD));
}

Instruction *IRBuilder::CreateSub () {
    return InsertInstruction (Instruction (InstructionId::ADD));
}

Instruction *IRBuilder::CreateMul () {
    return InsertInstruction (Instruction (InstructionId::ADD));
}

Instruction *IRBuilder::CreateDiv () {
    return InsertInstruction (Instruction (InstructionId::ADD));
}

Instruction *IRBuilder::InsertInstruction (Instruction newInstruction) {
    if (!insertPoint)
        return nullptr;

    ValueType insertPointType = insertPoint->GetType ();

    //TODO
    if (insertPointType == ValueType::BASIC_BLOCK) {

    } else if (insertPointType == ValueType::INSTRUCTION) {
        
    } else {
        return nullptr;
    }
}
