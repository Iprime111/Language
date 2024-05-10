#ifndef IR_H_
#define IR_H_

#include "FunctionType.h"

struct IRContext;

// TODO just saying for you not to told me I never said this
// I'd probably put that and other developer-only enums in namespace details
// there's a convention that user should not acess anything from namespace details
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
