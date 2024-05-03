#ifndef USER_H_
#define USER_H_

#include <cstddef>

#include "Buffer.h"
#include "Use.h"
#include "Value.h"

class User : public Value {
    public:
        virtual ~User () = 0;

        Use   *GetOperandsList  ();
        Value *GetOperand       (size_t operandIndex);
        size_t GetOperandsCount ();

    protected:
        User (ValueType type);

    private:
        Buffer <Use> operands = {};
};

#endif
