#include "Value.h"
#include "User.h"

User::User (ValueId valueId, const Type *valueType) : Value (valueId, valueType), operandsCount (0) {}
User::User (ValueId valueId, const Type *valueType, size_t operandsCount) : Value (valueId, valueType), operandsCount (operandsCount) {
    operands = new Use [operandsCount];
}

User::~User () {
    delete [] operands;
}

const Use *User::GetOperandsList () const {
    return operands;
}

const Value *User::GetOperand (size_t operandIndex) const {
    if (operandIndex >= operandsCount)
        return nullptr;

    return operands [operandIndex].GetOperand ();
}

size_t User::GetOperandsCount () const { return operandsCount; }

