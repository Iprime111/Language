#include <vector>

#include "Argument.h"
#include "BasicBlock.h"
#include "Type.h"
#include "Function.h"
#include "IRContext.h"

namespace IR {
    Function::Function (const char *name, const Type *type, std::vector <Argument *> &&arguments) : 
        Value (ValueId::FUNCTION, type), name (name), args (arguments) {

        parent = nullptr;
    }
    
    Function::~Function () {
        for (std::list <BasicBlock *>::iterator iterator = basicBlocks.begin (); iterator != basicBlocks.end (); iterator++)
            delete *iterator;

        for (size_t argumentIndex = 0; argumentIndex < args.size (); argumentIndex++)
            delete args [argumentIndex];
    }
    
    const std::string              &Function::GetName () const { return name; }
    const std::vector <Argument *> &Function::GetArgs () const { return args; }
    
    size_t Function::GetAllocaSize () const { return allocaSize; }
    
    Function *Function::Create (IRContext *context, const Type *type, std::vector <const Type *> *argumentTypes, const char *name) {
        if (!context || !name)
            return nullptr;
    
        std::vector <Argument *> arguments = {};

        for (size_t argumentIndex = 0; argumentIndex < argumentTypes->size (); argumentIndex++)
            arguments.push_back (new Argument ((*argumentTypes) [argumentIndex], argumentIndex, ""));

        Function *newFunction = new Function (name, type, std::move (arguments));
    
        context->functions.push_back (newFunction);
    
        return newFunction;
    }
}
