#include <cassert>

#include "Ir.h"
#include "Buffer.h"
#include "FunctionType.h"
#include "BackendCore.h"

BasicBlock::BasicBlock (char *name) : instructions ({}), blockLength (0), name (name) {
    InitBuffer (&instructions);
}

BasicBlock *BasicBlock::Create (char *name, Function *function) {
    if (!name || !function)
        return nullptr;

    BasicBlock newBlock (name);

    if (WriteDataToBuffer (&function->basicBlocks, &newBlock, 1) != BufferErrorCode::NO_BUFFER_ERRORS)
        return nullptr;

    return &function->basicBlocks.data [function->basicBlocks.currentIndex - 1];
}

Function::Function (char *name, FunctionType *type) : name (name), type (*type) {
    InitBuffer (&basicBlocks);
}

Function *Function::Create (FunctionType *type, char *name, TranslationContext *context) {
    if (!context || !name)
        return nullptr;

    Function newFunction (name, type);

    if (WriteDataToBuffer (&context->functions, &newFunction, 1) != BufferErrorCode::NO_BUFFER_ERRORS)
        return nullptr;

    return &context->functions.data [context->functions.currentIndex - 1];
}
