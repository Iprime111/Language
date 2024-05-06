#include "BasicBlock.h"
#include "FunctionType.h"
#include "Function.h"
#include "IRContext.h"

Function::Function (char *name, FunctionType *type) : Value (ValueId::FUNCTION, type->returnValue), name (name), type (*type) {
    parent = nullptr;
}

      char         *Function::GetName         () const { return name; }
const FunctionType *Function::GetFunctionType () const { return &type; }

BasicBlock *Function::GetHead () const { return head; }
BasicBlock *Function::GetTail () const { return tail; }

Function *Function::Create (FunctionType *type, char *name, IRContext *context) {
    if (!context || !name)
        return nullptr;

    //TODO Allocating this shit like this to have an ability to derive a Function class from Constant in future
    Function *newFunction = new Function (name, type);

    context->functions.push_back (newFunction);

    return newFunction;
}
