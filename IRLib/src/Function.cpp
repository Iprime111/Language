#include "FunctionType.h"
#include "Function.h"
#include "IRContext.h"

Function::Function (char *name, FunctionType *type) : Value (ValueType::FUNCTION), name (name), type (*type) {
    InitBuffer (&basicBlocks);
}

char         *Function::GetName () { return name; }
FunctionType *Function::GetType () { return &type; }


Function *Function::Create (FunctionType *type, char *name, IRContext *context) {
    if (!context || !name)
        return nullptr;

    Function newFunction (name, type);

    if (WriteDataToBuffer (&context->functions, &newFunction, 1) != BufferErrorCode::NO_BUFFER_ERRORS)
        return nullptr;

    return &context->functions.data [context->functions.currentIndex - 1];
}
