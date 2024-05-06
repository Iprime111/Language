#ifndef FUNCTION_H_
#define FUNCTION_H_

#include <vector>

#include "BasicBlock.h"
#include "FunctionType.h"
#include "Value.h"

class IRBuilder;

//TODO maybe derive from the Constant class
class Function final : public Value {
    friend class IRBuilder;

    public:
        std::vector <BasicBlock> basicBlocks = {};

        Function  () = delete;
        ~Function () = default;

              char         *GetName         () const;
        const FunctionType *GetFunctionType () const;

        BasicBlock *GetHead () const;
        BasicBlock *GetTail () const;

        size_t GetAllocaSize () const;

        static Function *Create (FunctionType *type, char *name, IRContext *context);

    private:
        char         *name = nullptr;
        FunctionType  type = {};

        BasicBlock   *head = nullptr;
        BasicBlock   *tail = nullptr;

        size_t allocaSize  = 0;

        Function (char *name, FunctionType *type);
};

#endif
