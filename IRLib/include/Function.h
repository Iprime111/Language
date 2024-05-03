#ifndef FUNCTION_H_
#define FUNCTION_H_

#include "BasicBlock.h"
#include "FunctionType.h"
#include "Value.h"

//TODO maybe derive from the Constant class
class Function final : public Value {
    public:
        Buffer <BasicBlock> basicBlocks = {};

        Function () = delete;

        char         *GetName ();
        FunctionType *GetType ();

        static Function *Create (FunctionType *type, char *name, IRContext *context);

    private:
        char         *name = nullptr;
        FunctionType  type = {};

        Function (char *name, FunctionType *type);
};

#endif
