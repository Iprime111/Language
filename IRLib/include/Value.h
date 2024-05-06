#ifndef IR_H_
#define IR_H_

#include "FunctionType.h"

struct IRContext;

enum class ValueId {
    VALUE        = 1 << 0,
    INSTRUCTION  = 1 << 1,
    BASIC_BLOCK  = 1 << 2,
    FUNCTION     = 1 << 3,
    CONSTANT     = 1 << 4,
};

class Value {
    public:
        Value *parent = nullptr;

        virtual ~Value () = default;

             ValueId GetValueId () const;
        const Type * GetType    () const;

    protected:
        Value (ValueId valueId, const Type *valueType);

    private:
        ValueId     valueId   = ValueId::VALUE;
        const Type *valueType = nullptr;

};

#endif
