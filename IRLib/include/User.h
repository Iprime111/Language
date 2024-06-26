#ifndef USER_H_
#define USER_H_

#include <cstddef>

#include "Use.h"
#include "Value.h"

namespace IR {
    class User : public Value {
        public:
            User (const User &user) = delete;
            User (User &&user)      = delete;
    
            User &operator= (const User &user) = delete;
            User &operator= (User &&user)      = delete;
    
            virtual ~User ();
    
            const Use   *GetOperandsList  () const;
            const Value *GetOperand       (size_t operandIndex) const;
    
            size_t GetOperandsCount () const;
    
        protected:
            Use   *operands      = nullptr;
            size_t operandsCount = 0;
    
            User (ValueId valueId, const Type *valueType, size_t operandsCount);
            User (ValueId valueId, const Type *valueType);
    };
}
#endif
