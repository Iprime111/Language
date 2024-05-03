#include "Instruction.h"

Instruction::Instruction (InstructionId instructionId) : User (ValueType::INSTRUCTION), instructionId (instructionId) {}

InstructionId Instruction::GetId () { return instructionId; }
