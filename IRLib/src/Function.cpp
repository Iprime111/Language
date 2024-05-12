#include "BasicBlock.h"
#include "FunctionType.h"
#include "Function.h"
#include "IRContext.h"

namespace IR {
    Function::Function (const char *name, FunctionType *type) : Value (ValueId::FUNCTION, type->returnValue), name (name), functionType (*type) {
        parent = nullptr;
    }
    
    Function::~Function () {
        for (std::list <BasicBlock *>::iterator iterator = basicBlocks.begin (); iterator != basicBlocks.end (); iterator++) {
            delete *iterator;
        }
    }
    
    const char         *Function::GetName         () const { return name; }
    const FunctionType *Function::GetFunctionType () const { return &functionType; }
    
    size_t Function::GetAllocaSize () const { return allocaSize; }
    
    Function *Function::Create (IRContext *context, FunctionType *type, const char *name) {
        if (!context || !name)
            return nullptr;
    
        //TODO Allocating this shit like this to have an ability to derive a Function class from Constant in future
        Function *newFunction = new Function (name, type);
    
        context->functions.push_back (newFunction);
    
        return newFunction;
    }
}
