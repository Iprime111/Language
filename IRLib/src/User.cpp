#include "Value.h"
#include "User.h"

User::User (ValueType type) : Value (type) {}

Use *User::GetOperandsList () {
    return operands.data;
}

Value *User::GetOperand (size_t operandIndex) {
    if (operandIndex >= operands.currentIndex)
        return nullptr;

    return operands.data [operandIndex].GetOperand ();
}

size_t User::GetOperandsCount () { return operands.currentIndex; }

