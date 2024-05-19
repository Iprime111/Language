#ifndef IR_H_
#define IR_H_

#include "Type.h"
namespace IR {
    struct IRContext;
    
    enum class ValueId {
        VALUE        = 1 << 0,
        INSTRUCTION  = 1 << 1,
        BASIC_BLOCK  = 1 << 2,
        FUNCTION     = 1 << 3,
        CONSTANT     = 1 << 4,
        ARGUMENT     = 1 << 5,
    };
    
    class Value {
        public:
            Value *parent = nullptr;
    
            virtual ~Value () = default;
    
                 ValueId GetValueId () const;
            const Type * GetType    () const;
    
        protected:
            explicit Value (ValueId valueId, const Type *valueType);
    
        private:
            ValueId     valueId   = ValueId::VALUE;
            const Type *valueType = nullptr;
    
    };
}
#endif
