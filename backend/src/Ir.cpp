#include <cassert>

#include "Ir.h"
#include "Buffer.h"
#include "FunctionType.h"
#include "BackendCore.h"

//------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------Value------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------

Value::Value (ValueType valueType) : valueType (valueType) {}

Value *Value::GetParent () { return parent; }
Value *Value::GetNext   () { return next; }
Value *Value::GetPrev   () { return prev; }

ValueType Value::GetType () { return valueType; }

//------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------Instruction------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------

Instruction::Instruction (InstructionId instructionId) : Value (ValueType::INSTRUCTION), instructionId (instructionId) {}

InstructionId Instruction::GetId () { return instructionId; }

//------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------BasicBlock-------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------

BasicBlock::BasicBlock (char *name) : Value (ValueType::BASIC_BLOCK), instructions ({}), name (name), blockLength (0) {
    InitBuffer (&instructions);
}

char  *BasicBlock::GetName   () { return name; }
size_t BasicBlock::GetLength () { return blockLength; }


BasicBlock *BasicBlock::Create (char *name, Function *function) {
    if (!name || !function)
        return nullptr;

    BasicBlock newBlock (name);

    if (WriteDataToBuffer (&function->basicBlocks, &newBlock, 1) != BufferErrorCode::NO_BUFFER_ERRORS)
        return nullptr;

    return &function->basicBlocks.data [function->basicBlocks.currentIndex - 1];
}

//------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------Function---------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------

Function::Function (char *name, FunctionType *type) : Value (ValueType::FUNCTION), name (name), type (*type) {
    InitBuffer (&basicBlocks);
}

char         *Function::GetName () { return name; }
FunctionType *Function::GetType () { return &type; }


Function *Function::Create (FunctionType *type, char *name, TranslationContext *context) {
    if (!context || !name)
        return nullptr;

    Function newFunction (name, type);

    if (WriteDataToBuffer (&context->functions, &newFunction, 1) != BufferErrorCode::NO_BUFFER_ERRORS)
        return nullptr;

    return &context->functions.data [context->functions.currentIndex - 1];
}
