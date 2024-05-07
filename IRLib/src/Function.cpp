#include "BasicBlock.h"
#include "FunctionType.h"
#include "Function.h"
#include "IRContext.h"

Function::Function (char *name, FunctionType *type) : Value (ValueId::FUNCTION, type->returnValue), name (name), functionType (*type) {
    parent = nullptr;
}

Function::~Function () {
    for (size_t blockIndex = 0; blockIndex < basicBlocks.size (); blockIndex++) {
        delete basicBlocks [blockIndex];
    }
}

      char         *Function::GetName         () const { return name; }
const FunctionType *Function::GetFunctionType () const { return &functionType; }

size_t Function::GetAllocaSize () const { return allocaSize; }

Function *Function::Create (FunctionType *type, char *name, IRContext *context) {
    if (!context || !name)
        return nullptr;

    //TODO Allocating this shit like this to have an ability to derive a Function class from Constant in future
    Function *newFunction = new Function (name, type);

    context->functions.push_back (newFunction);

    return newFunction;
}
