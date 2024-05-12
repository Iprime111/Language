#include "Value.h"
#include "User.h"

namespace IR {
    User::User (const User &user) : Value (user.GetValueId (), user.GetType ()), operandsCount (user.operandsCount) {
        operands = new Use [operandsCount];
    
        for (size_t operandIndex = 0; operandIndex < operandsCount; operandIndex++) {
            operands [operandIndex] = user.operands [operandIndex];
        }
    }
    
    User::User (User &&user) : Value (user.GetValueId (), user.GetType ()), operandsCount (user.operandsCount), operands (user.operands) {}
    User::User (ValueId valueId, const Type *valueType) : Value (valueId, valueType), operandsCount (0) {}
    
    User &User::operator= (const User &user) {
        return *this = User (user);
    }
    
    User &User::operator= (User &&user) {
        return *this = User (user);
    }
    
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
}
